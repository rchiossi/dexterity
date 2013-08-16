#include <stdint.h>
#include <malloc.h>
#include <stdlib.h>

#include "bytestream.h"
#include "leb128.h"
#include "dex.h"

#define DX_ALLOC(_type,_var)			\
  do {						\
  (_var) = (_type*) malloc(sizeof(_type));	\
  if ((_var) == NULL) alloc_fail();		\
  (_var)->meta.offset = offset;			\
  (_var)->meta.corrupted = 0;			\
  } while(0)

#define DX_ALLOC_LIST(_type,_var,_size)			\
  do {							\
  if (_size != 0) {					\
    (_var) = (_type*) malloc(sizeof(_type)*(_size));	\
    if ((_var) == NULL) alloc_fail();			\
  } else {						\
    (_var) = NULL;					\
  }							\
  } while(0)

#define DXP_FIXED(_name,_type)			      \
  DXPARSE(_name,_type) {			      \
  _type* res;					      \
    						      \
  if (bs == NULL) return NULL;			      \
						      \
  res = (_type*) malloc(sizeof(_type));		      \
						      \
  if (res == NULL) alloc_fail();		      \
						      \
  bsseek(bs,offset);				      \
						      \
  dxread(bs,(uint8_t*)res,sizeof(_type));	      \
						      \
  return res;					      \
}

void alloc_fail() __attribute__ ((noreturn));
void alloc_fail() {
  fprintf(stderr,"ERROR: Unable to allocate memory.\n");
  exit(-1);
}

//Read Aux
int dxread(ByteStream* bs, uint8_t* buf, size_t size) {
  Metadata* meta = (Metadata*) buf;
  uint8_t* ptr = buf += sizeof(Metadata);
  size_t data_size = size - sizeof(Metadata);
  int ret;

  meta->offset = bs->offset;
  ret =  bsread(bs,ptr,data_size);
  meta->corrupted = (ret != data_size);

  return ret;
}

//Parse functions
DXP_FIXED(dx_header,DexHeaderItem)
DXP_FIXED(dx_stringid,DexStringIdItem)
DXP_FIXED(dx_typeid,DexTypeIdItem)
DXP_FIXED(dx_protoid,DexProtoIdItem)
DXP_FIXED(dx_fieldid,DexFieldIdItem)
DXP_FIXED(dx_methodid,DexMethodIdItem)
DXP_FIXED(dx_classdef,DexClassDefItem)

DexStringDataItem* dx_stringdata(ByteStream* bs, uint32_t offset) {
  DexStringDataItem* res;
  int check;

  if (bs == NULL) return NULL;

  DX_ALLOC(DexStringDataItem,res);

  bsseek(bs,offset);

  check = l128read(bs,&(res->size));

  if (check || bs->exhausted) {
    res->meta.corrupted = 1;
    return res;
  }

  res->data = (uint8_t*) malloc(sizeof(uint8_t)*ul128toui(res->size));

  if (res->data == NULL) alloc_fail();

  check = bsread(bs,res->data,ul128toui(res->size));

  res->meta.corrupted = check != ul128toui(res->size);

  return res;
}

DexEncodedFieldItem* dx_encodedfield(ByteStream* bs, uint32_t offset) {
  DexEncodedFieldItem* res;
  int check;

  if (bs == NULL) return NULL;

  DX_ALLOC(DexEncodedFieldItem,res);

  bsseek(bs,offset);

  check  = l128read(bs,&(res->field_idx_diff));
  check |= l128read(bs,&(res->access_flags));

  res->meta.corrupted = check || bs->exhausted;
  
  return res;
}

DexEncodedMethodItem* dx_encodedmethod(ByteStream* bs, uint32_t offset) {
  DexEncodedMethodItem* res;
  int check;

  if (bs == NULL) return NULL;

  DX_ALLOC(DexEncodedMethodItem,res);

  bsseek(bs,offset);

  check  = l128read(bs,&(res->method_idx_diff));
  check |= l128read(bs,&(res->access_flags));
  check |= l128read(bs,&(res->code_off));

  res->meta.corrupted = check || bs->exhausted;
  
  return res;
}

DexClassDataItem* dx_classdata(ByteStream* bs, uint32_t offset) {
  DexClassDataItem* res;
  int check;

  int i;
  uint8_t* ptr;
  uint32_t off;

  if (bs == NULL) return NULL;

  DX_ALLOC(DexClassDataItem,res);

  bsseek(bs,offset);

  check  = l128read(bs,&(res->static_fields_size));
  check |= l128read(bs,&(res->instance_fields_size));
  check |= l128read(bs,&(res->direct_methods_size));
  check |= l128read(bs,&(res->virtual_methods_size));

  res->meta.corrupted = check || bs->exhausted;

  if (res->meta.corrupted) return res;

  DX_ALLOC_LIST(DexEncodedFieldItem*,res->static_fields,
		ul128toui(res->static_fields_size));
  DX_ALLOC_LIST(DexEncodedFieldItem*,res->instance_fields,
		ul128toui(res->instance_fields_size));
  DX_ALLOC_LIST(DexEncodedMethodItem*,res->direct_methods,
		ul128toui(res->direct_methods_size));
  DX_ALLOC_LIST(DexEncodedMethodItem*,res->virtual_methods,
		ul128toui(res->virtual_methods_size));

  for (i=0; i<ul128toui(res->static_fields_size); i++)
    res->static_fields[i] = dx_encodedfield(bs,bs->offset);

  for (i=0; i<ul128toui(res->instance_fields_size); i++)
    res->instance_fields[i] = dx_encodedfield(bs,bs->offset);

  for (i=0; i<ul128toui(res->direct_methods_size); i++)
    res->direct_methods[i] = dx_encodedmethod(bs,bs->offset);

  for (i=0; i<ul128toui(res->virtual_methods_size); i++)
    res->virtual_methods[i] = dx_encodedmethod(bs,bs->offset);

  return res;
}

DXP_FIXED(dx_typeitem,DexTypeItem);

DexTypeList* dx_typelist(ByteStream* bs, uint32_t offset) {
  DexTypeList* tl;
  int ret;
  int i;

  if (bs == NULL) return NULL;

  DX_ALLOC(DexTypeList,tl);

  ret = bsread_offset(bs,(uint8_t*) &(tl->size),sizeof(uint32_t),offset);

  tl->meta.corrupted = (ret != sizeof(uint32_t));
  if (tl->meta.corrupted) return tl;

  DX_ALLOC_LIST(DexTypeItem*,tl->list,tl->size);

  for (i=0; i<tl->size; i++)
    tl->list[i] = dx_typeitem(bs,bs->offset);

  return tl;
}

DXP_FIXED(dx_tryitem,DexTryItem)

DexEncodedTypeAddrPair* dx_encodedtypeaddrpair(ByteStream* bs, uint32_t offset) {
  if (bs == NULL) return NULL;

  //TODO
  return NULL;
}

DexEncodedCatchHandler* dx_encodedcatchhandler(ByteStream* bs, uint32_t offset) {
  if (bs == NULL) return NULL;
  //TODO
  return NULL;
}

DexEncodedCatchHandlerList* dx_encodedcatchhandlerlist(ByteStream* bs, uint32_t offset) {
  if (bs == NULL) return NULL;
  //TODO
  return NULL;
}

DexCodeItem* dx_codeitem(ByteStream* bs, uint32_t offset) {
  if (bs == NULL) return NULL;
  //TODO
  return NULL;
}

DXP_FIXED(dx_mapitem,DexMapItem)

DexMapList* dx_maplist(ByteStream* bs, uint32_t offset) {
  DexMapList* ml;
  int ret;
  int i;

  if (bs == NULL) return NULL;

  DX_ALLOC(DexMapList,ml);

  ret = bsread_offset(bs,(uint8_t*)&(ml->size),sizeof(uint32_t),offset);

  ml->meta.corrupted = (ret != sizeof(uint32_t));
  if (ml->meta.corrupted) return ml;

  DX_ALLOC_LIST(DexMapItem*,ml->list,ml->size);  

  for (i=0; i<ml->size; i++)
    ml->list[i] = dx_mapitem(bs,bs->offset);

  return ml;
}
