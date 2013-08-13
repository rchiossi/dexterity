#include <stdint.h>
#include <malloc.h>

#include "bytestream.h"
#include "leb128.h"
#include "dex.h"

#define DXP_FIXED(_name,_type)		      \
DXPARSE(_name,_type) {			      \
  _type* res;					      \
    						      \
  if (bs == NULL) return NULL;			      \
						      \
  res = (_type*) malloc(sizeof(_type));		      \
						      \
  if (res == NULL) return NULL;			      \
						      \
  dxread(bs,(uint8_t*)res,sizeof(_type),offset);      \
						      \
  return res;					      \
}

//Read Aux
int dxread(ByteStream* bs, uint8_t* buf, size_t size, uint32_t offset) {
  Metadata* meta = (Metadata*) buf;
  uint8_t* ptr = buf += sizeof(Metadata);
  size_t data_size = size - sizeof(Metadata);
  int ret =  bsread_offset(bs,ptr,data_size,offset);

  meta->corrupted = (ret != data_size);
  meta->offset = offset;

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

  res = (DexStringDataItem*) malloc(sizeof(DexStringDataItem));

  if (res == NULL) return NULL;

  check = l128read(bs,offset,&(res->size));

  if (check || bs->exhausted) {
    res->meta.corrupted = 1;
    return res;
  }

  res->data = (uint8_t*) malloc(sizeof(uint8_t)*ul128toui(res->size));

  if (res->data == NULL) {
    free(res);
    return NULL;
  }

  check = bsread(bs,res->data,ul128toui(res->size));

  res->meta.corrupted = check != ul128toui(res->size);
  res->meta.offset = offset;

  return res;
}

DexEncodedFieldItem* dx_encodedfield(ByteStream* bs, uint32_t offset) {
  //TODO
  return NULL;
}

DexEncodedMethodItem* dx_encodedmethod(ByteStream* bs, uint32_t offset) {
  //TODO
  return NULL;
}

DexClassDataItem* dx_classdata(ByteStream* bs, uint32_t offset) {
  //TODO
  return NULL;
}

DXP_FIXED(dx_typeitem,DexTypeItem);

DexTypeList* dx_typelist(ByteStream* bs, uint32_t offset) {
  DexTypeList* tl = (DexTypeList*) malloc(sizeof(DexTypeList));
  uint8_t* ptr;
  uint32_t off;
  int ret;
  int i;

  if (tl == NULL || bs == NULL) return NULL;

  tl->meta.offset = offset;

  ptr = (uint8_t*) &(tl->size);
  ret = bsread_offset(bs,ptr,sizeof(uint32_t),offset);

  tl->meta.corrupted = (ret != sizeof(uint32_t));

  if (tl->meta.corrupted) return tl;

  tl->list = (DexTypeItem*) malloc(sizeof(DexTypeItem)*tl->size);

  if (tl->list == NULL) {
    free(tl);
    return NULL;
  }

  for (i=0; i<tl->size; i++) {
    ptr = (uint8_t*) &(tl->list[i]);
    
    off = offset + sizeof(uint32_t) + (sizeof(DexTypeItem)-sizeof(Metadata))*i;
      
    dxread(bs,ptr,sizeof(DexTypeItem),off);

    if (tl->list[i].meta.corrupted) {
      tl->meta.corrupted = 1;
      break;
    }
  }

  return tl;
}

DXP_FIXED(dx_tryitem,DexTryItem)

DexEncodedTypeAddrPair* dx_encodedtypeaddrpair(ByteStream* bs, uint32_t offset) {
  //TODO
  return NULL;
}

DexEncodedCatchHandler* dx_encodedcatchhandler(ByteStream* bs, uint32_t offset) {
  //TODO
  return NULL;
}

DexEncodedCatchHandlerList* dx_encodedcatchhandlerlist(ByteStream* bs, uint32_t offset) {
  //TODO
  return NULL;
}

DexCodeItem* dx_codeitem(ByteStream* bs, uint32_t offset) {
  //TODO
  return NULL;
}

DXP_FIXED(dx_mapitem,DexMapItem)

DexMapList* dx_maplist(ByteStream* bs, uint32_t offset) {
  DexMapList* ml = (DexMapList*) malloc(sizeof(DexMapList));
  uint8_t* ptr;
  uint32_t off;
  int ret;
  int i;

  if (ml == NULL || bs == NULL) return NULL;

  ml->meta.offset = offset;

  ptr = (uint8_t*) &(ml->size);
  ret = bsread_offset(bs,ptr,sizeof(uint32_t),offset);

  ml->meta.corrupted = (ret != sizeof(uint32_t));

  if (ml->meta.corrupted) return ml;

  ml->list = (DexMapItem*) malloc(sizeof(DexMapItem)*ml->size);

  if (ml->list == NULL) {
    free(ml);
    return NULL;
  }

  for (i=0; i<ml->size; i++) {
    ptr = (uint8_t*) &(ml->list[i]);
    
    off = offset + sizeof(uint32_t) + (sizeof(DexMapItem)-sizeof(Metadata))*i;

    dxread(bs,ptr,sizeof(DexMapItem),off);

    if (ml->list[i].meta.corrupted) {
      ml->meta.corrupted = 1;
      break;
    }
  }

  return ml;
}
