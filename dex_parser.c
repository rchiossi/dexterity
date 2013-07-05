#include <stdint.h>
#include <malloc.h>

#include "bytestream.h"
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

DXPARSE(dx_stringdata,DexStringDataItem) {
  //TODO
  return NULL;
}

DXP_FIXED(dx_typeid,DexTypeIdItem)
DXP_FIXED(dx_protoid,DexProtoIdItem)
DXP_FIXED(dx_fieldid,DexFieldIdItem)
DXP_FIXED(dx_methodid,DexMethodIdItem)
DXP_FIXED(dx_classdef,DexClassDefItem)


DXPARSE(dx_encodedfield,DexEncodedFieldItem) {
  //TODO
  return NULL;
}

DXPARSE(dx_encodedmethod,DexEncodedMethodItem) {
  //TODO
  return NULL;
}

DXPARSE(dx_classdata,DexClassDataItem) {
  //TODO
  return NULL;
}

DXPARSE(dx_typelist,DexTypeList) {
  DexTypeList* tl = (DexTypeList*) malloc(sizeof(DexTypeList));
  uint8_t* ptr;
  int ret;

  if (tl == NULL) return NULL;

  tl->meta.offset = offset;

  ptr = (uint8_t*) &(tl->size);
  ret = bsread_offset(bs,ptr,sizeof(uint32_t),offset);

  tl->meta.corrupted = (ret == sizeof(uint32_t));

  tl->list = (uint16_t*) malloc(sizeof(uint16_t)*tl->size);

  if (tl->list != NULL) {
    ptr = (uint8_t*) tl->list;
    ret = bsread_offset(bs,ptr,sizeof(uint16_t)*tl->size,offset);
  }

  tl->meta.corrupted = ((ret != sizeof(uint32_t)) || tl->meta.corrupted);

  return tl;
}

DXP_FIXED(dx_tryitem,DexTryItem)


//DXP_FIXED(dx_,)
