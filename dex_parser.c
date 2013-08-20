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
  DX_ALLOC(_type,res);				      \
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
  DexEncodedTypeAddrPair* res;
  int check;

  if (bs == NULL) return NULL;

  DX_ALLOC(DexEncodedTypeAddrPair,res);

  bsseek(bs,offset);

  check  = l128read(bs,&(res->type_idx));
  check |= l128read(bs,&(res->addr));

  res->meta.corrupted = check || bs->exhausted;
  
  return res;
}

DexEncodedCatchHandler* dx_encodedcatchhandler(ByteStream* bs, uint32_t offset) {
  DexEncodedCatchHandler* res;
  int check;
  int i;

  if (bs == NULL) return NULL;

  DX_ALLOC(DexEncodedCatchHandler,res);

  bsseek(bs,offset);

  check  = l128read(bs,&(res->size));

  res->meta.corrupted = check;
  if (res->meta.corrupted) return res;

  DX_ALLOC_LIST(DexEncodedTypeAddrPair*,res->handlers,ul128toui(res->size));

  for (i=0; i<abs(sl128toui(res->size)); i++)
    res->handlers[i] = dx_encodedtypeaddrpair(bs,bs->offset);

  if (sl128toui(res->size) <= 0) 
    check |= l128read(bs,&(res->catch_all_addr));

  res->meta.corrupted = check || bs->exhausted;
  
  return res;
}

DexEncodedCatchHandlerList* dx_encodedcatchhandlerlist(ByteStream* bs, uint32_t offset) {
  DexEncodedCatchHandlerList* res;
  int check;
  int i;

  if (bs == NULL) return NULL;

  DX_ALLOC(DexEncodedCatchHandlerList,res);

  bsseek(bs,offset);

  check  = l128read(bs,&(res->size));

  res->meta.corrupted = check;
  if (res->meta.corrupted) return res;

  DX_ALLOC_LIST(DexEncodedCatchHandler*,res->list,ul128toui(res->size));

  for (i=0; i<ul128toui(res->size); i++)
    res->list[i] = dx_encodedcatchhandler(bs,bs->offset);

  res->meta.corrupted = check || bs->exhausted;
  
  return res;
}

DexCodeItem* dx_codeitem(ByteStream* bs, uint32_t offset) {
  DexCodeItem* res;
  int i;

  DX_ALLOC(DexCodeItem,res);

  bsseek(bs,offset);

  bsread(bs,(uint8_t*) &(res->registers_size),sizeof(uint16_t));
  bsread(bs,(uint8_t*) &(res->ins_size),sizeof(uint16_t));
  bsread(bs,(uint8_t*) &(res->outs_size),sizeof(uint16_t));
  bsread(bs,(uint8_t*) &(res->tries_size),sizeof(uint16_t));
  bsread(bs,(uint8_t*) &(res->debug_info_off),sizeof(uint32_t));
  bsread(bs,(uint8_t*) &(res->insns_size),sizeof(uint32_t));

  res->meta.corrupted = bs->exhausted;
  if (res->meta.corrupted) return res;

  DX_ALLOC_LIST(uint16_t,res->insns,res->insns_size);

  for (i=0; i<res->insns_size; i++)
    bsread(bs,(uint8_t*) &(res->insns[i]),sizeof(uint16_t));

  if (res->tries_size > 0) {
    if (res->insns_size % 2 != 0)
      bsread(bs,(uint8_t*) &(res->padding),sizeof(uint16_t));

    DX_ALLOC_LIST(DexTryItem*,res->tries,res->tries_size); 
   
    for (i=0; i<res->tries_size; i++)
      res->tries[i] = dx_tryitem(bs,bs->offset);

    res->handlers = dx_encodedcatchhandlerlist(bs,bs->offset);    
  }

  return res;
}

DexDebugInfo* dx_debuginfo(ByteStream* bs, uint32_t offset) {
  DexDebugInfo* res;
  int check;
  int i;

  if (bs == NULL) return NULL;

  DX_ALLOC(DexDebugInfo,res);

  bsseek(bs,offset);

  check  = l128read(bs,&(res->line_start));
  check |= l128read(bs,&(res->parameters_size));

  res->meta.corrupted = check || bs->exhausted;
  if (res->meta.corrupted) return res;

  DX_ALLOC_LIST(leb128_t,res->parameter_names,ul128toui(res->parameters_size));

  for (i=0; i<ul128toui(res->parameters_size); i++)
    l128read(bs,&(res->parameter_names[i]));

  return res;
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
