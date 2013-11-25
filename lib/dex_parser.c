#include <stdint.h>
#include <stdlib.h>

#include "bytestream.h"
#include "leb128.h"
#include "dex.h"

#define DX_ALLOC(_type,_var)			\
  do {						\
    (_var) = (_type*) malloc_s(sizeof(_type));	\
    (_var)->meta.offset = offset;		\
    (_var)->meta.corrupted = false;			\
  } while(0)

#define DX_ALLOC_LIST(_type,_var,_size)			\
  do {							\
    if (_size != 0) {					\
      (_var) = (_type*) malloc_s(sizeof(_type)*(_size));	\
    } else {						\
      (_var) = NULL;					\
    }							\
  } while(0)

#define DX_REALLOC_LIST(_type,_var,_cur_size)				\
  do {									\
    (_var) = (_type*) realloc((_var),sizeof(_type)*(_cur_size)*2);	\
    _cur_size *= 2;							\
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

//Read Aux
int dxread(ByteStream* bs, uint8_t* buf, size_t size) {
  Metadata* meta = (Metadata*) buf;
  uint8_t* ptr = buf += sizeof(Metadata);
  size_t data_size = size - sizeof(Metadata);
  unsigned int ret;

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
  uint8_t* tape;

  if (bs == NULL) return NULL;

  DX_ALLOC(DexStringDataItem,res);

  bsseek(bs,offset);

  check = l128read(bs,&(res->size));

  if (check || bs->exhausted) {
    res->meta.corrupted = true;
    return res;
  }

  DX_ALLOC_LIST(uint8_t,res->data,ul128toui(res->size)*3+1);

  tape = (uint8_t*) res->data;

  bsread(bs,tape,sizeof(uint8_t));
  
  while(*tape) {
    if ((*tape & 0xe0) == 0xc0 ) {
      tape++;
      bsread(bs,tape,sizeof(uint8_t));
    }

    else if ((*tape & 0xf0) == 0xe0) {
      tape++;
      bsread(bs,tape,sizeof(uint8_t)*2);      
      tape++;
    }
        
    tape++;
    bsread(bs,tape,sizeof(uint8_t));
  }
  
  res->meta.corrupted |= bs->exhausted;

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

  unsigned int i;

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

DXP_FIXED(dx_typeitem,DexTypeItem)

DexTypeList* dx_typelist(ByteStream* bs, uint32_t offset) {
  DexTypeList* res;
  int check;
  unsigned int i;

  if (bs == NULL) return NULL;

  DX_ALLOC(DexTypeList,res);

  bsseek(bs,offset);

  check = bsread(bs,(uint8_t*) &(res->size),sizeof(uint32_t));

  res->meta.corrupted = (check != sizeof(uint32_t));
  if (res->meta.corrupted) return res;

  DX_ALLOC_LIST(DexTypeItem*,res->list,res->size);

  for (i=0; i<res->size; i++)
    res->list[i] = dx_typeitem(bs,bs->offset);

  return res;
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

  for (i=0; i<abs(sl128toi(res->size)); i++)
    res->handlers[i] = dx_encodedtypeaddrpair(bs,bs->offset);

  if (sl128toi(res->size) <= 0) 
    check |= l128read(bs,&(res->catch_all_addr));

  res->meta.corrupted = check || bs->exhausted;
  
  return res;
}

DexEncodedCatchHandlerList* dx_encodedcatchhandlerlist(ByteStream* bs, uint32_t offset) {
  DexEncodedCatchHandlerList* res;
  int check;
  unsigned int i;

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
  unsigned int i;

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
  unsigned int i;

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

  res->state_machine = dx_debug_state_machine(bs,bs->offset);

  return res;
}

DXP_FIXED(dx_mapitem,DexMapItem)

DexMapList* dx_maplist(ByteStream* bs, uint32_t offset) {
  DexMapList* res;
  int check;
  unsigned int i;

  if (bs == NULL) return NULL;

  DX_ALLOC(DexMapList,res);

  bsseek(bs,offset);

  check = bsread(bs,(uint8_t*)&(res->size),sizeof(uint32_t));

  res->meta.corrupted = (check != sizeof(uint32_t));
  if (res->meta.corrupted) return res;

  DX_ALLOC_LIST(DexMapItem*,res->list,res->size);  

  for (i=0; i<res->size; i++)
    res->list[i] = dx_mapitem(bs,bs->offset);

  return res;
}

DexEncodedValue* dx_encodedvalue(ByteStream* bs, uint32_t offset) {
  DexEncodedValue* res;
  int check;
  int i;

  uint8_t value_type;
  uint8_t value_arg;

  if (bs == NULL) return NULL;

  bsseek(bs,offset);

  DX_ALLOC(DexEncodedValue,res);

  check = bsread(bs,(uint8_t*)&(res->argtype),sizeof(uint8_t));

  res->meta.corrupted = (check != sizeof(uint8_t));
  if (res->meta.corrupted) return res;

  value_type = (res->argtype & 0x1f);
  value_arg  = ((res->argtype >> 5) & 0x7);
	       
  switch (value_type) {
  case 0x00:
    DX_ALLOC_LIST(uint8_t,res->value,1);
    bsread(bs,(uint8_t*) res->value,sizeof(uint8_t));
    break;
  case 0x02:
  case 0x03:
  case 0x04:
  case 0x06:
  case 0x10:
  case 0x11:
  case 0x17:
  case 0x18:
  case 0x19:
  case 0x1a:
  case 0x1b:
    DX_ALLOC_LIST(uint8_t,res->value,value_arg+1);

    for (i=0; i<value_arg+1; i++)
      bsread(bs,(uint8_t*) &(res->value[i]),sizeof(uint8_t));   

    break;
  case 0x1c:
    res->value = (uint8_t*) dx_encodedarray(bs,bs->offset);
    break;
  case 0x1d:
    res->value = (uint8_t*) dx_encodedannotation(bs,bs->offset);
    break;
  case 0x1e:
  case 0x1f:
    res->value = NULL;
  default:
    res->meta.corrupted = true;
    res->value = (uint8_t*) -1;
  }

  return res;
}

DexEncodedArray* dx_encodedarray(ByteStream* bs, uint32_t offset) {
  DexEncodedArray* res;
  int check;
  unsigned int i;

  if (bs == NULL) return NULL;

  bsseek(bs,offset);

  DX_ALLOC(DexEncodedArray,res);

  check = l128read(bs,&(res->size));

  res->meta.corrupted = (check || bs->exhausted);
  if (res->meta.corrupted) return res;

  DX_ALLOC_LIST(DexEncodedValue*,res->values,ul128toui(res->size));

  for (i=0; i<ul128toui(res->size); i++)
    res->values[i] = dx_encodedvalue(bs,bs->offset);

  return res;  
}

DexAnnotationElement* dx_annotationelement(ByteStream* bs, uint32_t offset) {
  DexAnnotationElement* res;
  int check;

  if (bs == NULL) return NULL;

  bsseek(bs,offset);

  DX_ALLOC(DexAnnotationElement,res);

  check = l128read(bs,&(res->name_idx));

  res->value = dx_encodedvalue(bs,bs->offset);

  res->meta.corrupted = (check || bs->exhausted);

  return res;
}

DexEncodedAnnotation* dx_encodedannotation(ByteStream* bs, uint32_t offset) {
  DexEncodedAnnotation* res;
  int check;
  unsigned int i;

  if (bs == NULL) return NULL;

  bsseek(bs,offset);

  DX_ALLOC(DexEncodedAnnotation,res);

  check  = l128read(bs,&(res->type_idx));
  check |= l128read(bs,&(res->size));

  res->meta.corrupted = (check || bs->exhausted);
  if (res->meta.corrupted) return res;

  DX_ALLOC_LIST(DexAnnotationElement*,res->elements,ul128toui(res->size));

  for (i=0; i<ul128toui(res->size); i++)
    res->elements[i] = dx_annotationelement(bs,bs->offset);

  return res;  
}

uint8_t* dx_debug_state_machine(ByteStream* bs, uint32_t offset) {
  uint32_t size = 1;
  uint8_t opcode;
  leb128_t leb;
  uint8_t* res;

  bsseek(bs,offset);
  bsread(bs,&opcode,1);

  while (opcode != 0x0) {
    switch (opcode) {
    case 0x1:
    case 0x2:
    case 0x5:
    case 0x6:
    case 0x9:
      l128read(bs,&leb);
      break;
    case 0x3:
      l128read(bs,&leb);
      l128read(bs,&leb);
      l128read(bs,&leb);
      break;
    case 0x4:
      l128read(bs,&leb);
      l128read(bs,&leb);
      l128read(bs,&leb);
      l128read(bs,&leb);
    default:
      break;
    }

    bsread(bs,&opcode,1);
  }

  size = bs->offset - offset;

  DX_ALLOC_LIST(uint8_t,res,size + sizeof(uint32_t));  

  bsseek(bs,offset);
  bsread(bs,(res+sizeof(uint32_t)),size);

  ((uint32_t*) res)[0] = size;

  return res;    
}

DXP_FIXED(dx_fieldannotation,DexFieldAnnotation)
DXP_FIXED(dx_methodannotation,DexMethodAnnotation)
DXP_FIXED(dx_parameterannotation,DexParameterAnnotation)

DexAnnotationDirectoryItem* dx_annotationdirectoryitem(ByteStream* bs, uint32_t offset) {
  DexAnnotationDirectoryItem* res;
  int check;
  unsigned int i;

  if (bs == NULL) return NULL;

  DX_ALLOC(DexAnnotationDirectoryItem,res);

  bsseek(bs,offset);

  check  = bsread(bs,(uint8_t*) &(res->class_annotations_off),sizeof(uint32_t));
  check += bsread(bs,(uint8_t*) &(res->fields_size),sizeof(uint32_t));
  check += bsread(bs,(uint8_t*) &(res->annotated_methods_size),sizeof(uint32_t));
  check += bsread(bs,(uint8_t*) &(res->annotated_parameters_size),sizeof(uint32_t));

  res->meta.corrupted = (check != sizeof(uint32_t)*4 || bs->exhausted);
  if (res->meta.corrupted) return res;

  DX_ALLOC_LIST(DexFieldAnnotation*,res->field_annotations,
		res->fields_size);
  DX_ALLOC_LIST(DexMethodAnnotation*,res->method_annotations,
		res->annotated_methods_size);
  DX_ALLOC_LIST(DexParameterAnnotation*,res->parameter_annotations,
		res->annotated_parameters_size);

  for (i=0; i<res->fields_size; i++)
    res->field_annotations[i] = dx_fieldannotation(bs,bs->offset);

  for (i=0; i<res->annotated_methods_size; i++)
    res->method_annotations[i] = dx_methodannotation(bs,bs->offset);
  
  for (i=0; i<res->annotated_parameters_size; i++)
    res->parameter_annotations[i] = dx_parameterannotation(bs,bs->offset);

  return res;
}

DXP_FIXED(dx_annotationsetrefitem,DexAnnotationSetRefItem)

DexAnnotationSetRefList* dx_annotationsetreflist(ByteStream* bs, uint32_t offset) {
  DexAnnotationSetRefList* res;
  int check;
  unsigned int i;

  if (bs == NULL) return NULL;

  DX_ALLOC(DexAnnotationSetRefList,res);

  bsseek(bs,offset);

  check  = bsread(bs,(uint8_t*) &(res->size),sizeof(uint32_t));

  res->meta.corrupted = (check != sizeof(uint32_t) || bs->exhausted);
  if (res->meta.corrupted) return res;

  DX_ALLOC_LIST(DexAnnotationSetRefItem*,res->list,res->size);

  for (i=0; i<res->size; i++)
    res->list[i] = dx_annotationsetrefitem(bs,bs->offset);

  return res;
}

DXP_FIXED(dx_annotationoffitem,DexAnnotationOffItem)

DexAnnotationSetItem* dx_annotationsetitem(ByteStream* bs, uint32_t offset) {
  DexAnnotationSetItem* res;
  int check;
  unsigned int i;

  if (bs == NULL) return NULL;

  DX_ALLOC(DexAnnotationSetItem,res);

  bsseek(bs,offset);

  check  = bsread(bs,(uint8_t*) &(res->size),sizeof(uint32_t));

  res->meta.corrupted = (check != sizeof(uint32_t) || bs->exhausted);
  if (res->meta.corrupted) return res;

  DX_ALLOC_LIST(DexAnnotationOffItem*,res->entries,res->size);

  for (i=0; i<res->size; i++)
    res->entries[i] = dx_annotationoffitem(bs,bs->offset);

  return res;
}

DexAnnotationItem* dx_annotationitem(ByteStream* bs, uint32_t offset) {
  DexAnnotationItem* res;
  int check;

  if (bs == NULL) return NULL;

  DX_ALLOC(DexAnnotationItem,res);

  bsseek(bs,offset);

  check  = bsread(bs,(uint8_t*) &(res->visibility),sizeof(uint8_t));

  res->annotation = dx_encodedannotation(bs,bs->offset);

  res->meta.corrupted = (check != sizeof(uint8_t) || bs->exhausted);
  
  return res;
}

DexEncodedArrayItem* dx_encodedarrayitem(ByteStream* bs, uint32_t offset) {
  DexEncodedArrayItem* res;

  if (bs == NULL) return NULL;

  DX_ALLOC(DexEncodedArrayItem,res);

  bsseek(bs,offset);

  res->value = dx_encodedarray(bs,bs->offset);

  res->meta.corrupted = bs->exhausted;
  
  return res;
}

#define DX_READ_TABLE(_TYPE,_FUNC,_NUM,_OFFVAR,_VAR,_TSIZE,_TALLOC) \
  do {									\
    for (i=0; i< (_NUM); i++) {						\
      if (_OFFVAR == 0) continue;					\
      									\
      if ((_TSIZE) >= (_TALLOC)) {					\
      	DX_REALLOC_LIST(_TYPE, (_VAR),(_TALLOC));			\
      }									\
      									\
      (_VAR)[(_TSIZE)] = _FUNC(bs,_OFFVAR);				\
      (_TSIZE)++;							\
    }									\
  } while(0)

Dex* dx_parse(ByteStream* bs) {
  Dex* dx;
  unsigned int i,j;

  if (bs == NULL) return NULL;

  dx = (Dex*) malloc_s(sizeof(Dex));

  dx->header = dx_header(bs,bs->offset);
  dx->map_list = dx_maplist(bs,dx->header->map_off);

  //header data
  DX_ALLOC_LIST(DexStringIdItem*,dx->string_ids,dx->header->string_ids_size);
  DX_ALLOC_LIST(DexTypeIdItem*,dx->type_ids,dx->header->type_ids_size);
  DX_ALLOC_LIST(DexProtoIdItem*,dx->proto_ids,dx->header->proto_ids_size);
  DX_ALLOC_LIST(DexFieldIdItem*,dx->field_ids,dx->header->field_ids_size);
  DX_ALLOC_LIST(DexMethodIdItem*,dx->method_ids,dx->header->method_ids_size);
  DX_ALLOC_LIST(DexClassDefItem*,dx->class_defs,dx->header->class_defs_size);

  dx->meta.string_ids_alloc = dx->header->string_ids_size;
  dx->meta.type_ids_alloc = dx->header->type_ids_size;
  dx->meta.proto_ids_alloc = dx->header->proto_ids_size;
  dx->meta.field_ids_alloc = dx->header->field_ids_size;
  dx->meta.method_ids_alloc = dx->header->method_ids_size;
  dx->meta.class_defs_alloc = dx->header->class_defs_size;
    
  bsseek(bs,dx->header->string_ids_off);
  for (i=0; i<dx->header->string_ids_size; i++)
    dx->string_ids[i] = dx_stringid(bs,bs->offset);

  bsseek(bs,dx->header->type_ids_off);
  for (i=0; i<dx->header->type_ids_size; i++)
    dx->type_ids[i] = dx_typeid(bs,bs->offset);

  bsseek(bs,dx->header->proto_ids_off);
  for (i=0; i<dx->header->proto_ids_size; i++)
    dx->proto_ids[i] = dx_protoid(bs,bs->offset);

  bsseek(bs,dx->header->field_ids_off);
  for (i=0; i<dx->header->field_ids_size; i++)
    dx->field_ids[i] = dx_fieldid(bs,bs->offset);

  bsseek(bs,dx->header->method_ids_off);
  for (i=0; i<dx->header->method_ids_size; i++)
    dx->method_ids[i] = dx_methodid(bs,bs->offset);

  bsseek(bs,dx->header->class_defs_off);
  for (i=0; i<dx->header->class_defs_size; i++)
    dx->class_defs[i] = dx_classdef(bs,bs->offset);

  //Data from string id
  dx->meta.string_data_list_size = dx->header->string_ids_size;
  dx->meta.string_data_list_alloc = dx->header->string_ids_size;

  DX_ALLOC_LIST(DexStringDataItem*,dx->string_data_list,dx->header->string_ids_size);

  for (i=0; i<dx->meta.string_data_list_size; i++)
    dx->string_data_list[i] = dx_stringdata(bs,dx->string_ids[i]->string_data_off);
  
  //Data from proto id
  dx->meta.type_lists_size = 0;
  dx->meta.type_lists_alloc = dx->header->proto_ids_size;

  DX_ALLOC_LIST(DexTypeList*,dx->type_lists,dx->meta.type_lists_alloc);

  DX_READ_TABLE(DexTypeList*,
		dx_typelist,
		dx->header->proto_ids_size,
		dx->proto_ids[i]->parameters_off,
		dx->type_lists,
		dx->meta.type_lists_size,
		dx->meta.type_lists_alloc);

  //Data from class def
  DX_READ_TABLE(DexTypeList*,
		dx_typelist,
		dx->header->class_defs_size,
		dx->class_defs[i]->interfaces_off,
		dx->type_lists,
		dx->meta.type_lists_size,
		dx->meta.type_lists_alloc);

  dx->meta.an_directories_size = 0;
  dx->meta.an_directories_alloc = dx->header->class_defs_size;

  DX_ALLOC_LIST(DexAnnotationDirectoryItem*,
		dx->an_directories,dx->meta.an_directories_alloc);

  DX_READ_TABLE(DexAnnotationDirectoryItem*,
		dx_annotationdirectoryitem,
		dx->header->class_defs_size,
		dx->class_defs[i]->annotations_off,
		dx->an_directories,
		dx->meta.an_directories_size,
		dx->meta.an_directories_alloc);

  dx->meta.class_data_size = 0;
  dx->meta.class_data_alloc = dx->header->class_defs_size;

  DX_ALLOC_LIST(DexClassDataItem*,
		dx->class_data,dx->meta.class_data_alloc);

  DX_READ_TABLE(DexClassDataItem*,
		dx_classdata,
		dx->header->class_defs_size,
		dx->class_defs[i]->class_data_off,
		dx->class_data,
		dx->meta.class_data_size,
		dx->meta.class_data_alloc);

  dx->meta.encoded_arrays_size = 0;
  dx->meta.encoded_arrays_alloc = dx->header->class_defs_size;

  DX_ALLOC_LIST(DexEncodedArray*,
		dx->encoded_arrays,dx->meta.encoded_arrays_alloc);

  DX_READ_TABLE(DexEncodedArray*,
		dx_encodedarray,
		dx->header->class_defs_size,
		dx->class_defs[i]->static_values_off,
		dx->encoded_arrays,
		dx->meta.encoded_arrays_size,
		dx->meta.encoded_arrays_alloc);

  //Data from class data
  dx->meta.code_list_size = 0;
  dx->meta.code_list_alloc = dx->meta.class_data_size;

  DX_ALLOC_LIST(DexCodeItem*,
		dx->code_list,dx->meta.code_list_alloc);

  for (j=0; j< dx->meta.class_data_size; j++) {
    if (dx->class_data[j]->meta.corrupted) continue;

    DX_READ_TABLE(DexCodeItem*,
		  dx_codeitem,
		  ul128toui(dx->class_data[j]->direct_methods_size),
		  ul128toui(dx->class_data[j]->direct_methods[i]->code_off),
		  dx->code_list,
		  dx->meta.code_list_size,
		  dx->meta.code_list_alloc);   

    DX_READ_TABLE(DexCodeItem*,
		  dx_codeitem,
		  ul128toui(dx->class_data[j]->virtual_methods_size),
		  ul128toui(dx->class_data[j]->virtual_methods[i]->code_off),
		  dx->code_list,
		  dx->meta.code_list_size,
		  dx->meta.code_list_alloc);   
  }

  //data from code item
  dx->meta.debug_info_list_size = 0;
  dx->meta.debug_info_list_alloc = dx->meta.code_list_size;

  DX_ALLOC_LIST(DexDebugInfo*,
		dx->debug_info_list,dx->meta.debug_info_list_alloc);

  DX_READ_TABLE(DexDebugInfo*,
		dx_debuginfo,
		dx->meta.code_list_size,
		dx->code_list[i]->debug_info_off,
		dx->debug_info_list,
		dx->meta.debug_info_list_size,
		dx->meta.debug_info_list_alloc);
 
  //data from annotations directory
  dx->meta.an_set_size = 0;
  dx->meta.an_set_alloc = dx->meta.an_directories_size;

  DX_ALLOC_LIST(DexAnnotationSetItem*,
		dx->an_set,dx->meta.an_set_alloc);

  DX_READ_TABLE(DexAnnotationSetItem*,
		dx_annotationsetitem,
		dx->meta.an_directories_size,
		dx->an_directories[i]->class_annotations_off,
		dx->an_set,
		dx->meta.an_set_size,
		dx->meta.an_set_alloc);

  dx->meta.an_set_ref_lists_size = 0;
  dx->meta.an_set_ref_lists_alloc = dx->meta.an_directories_size;

  DX_ALLOC_LIST(DexAnnotationSetRefList*,
		dx->an_set_ref_lists,dx->meta.an_set_ref_lists_alloc);

  for (j=0; j<dx->meta.an_directories_size; j++) {
    if (dx->an_directories[j]->meta.corrupted) continue;

    DX_READ_TABLE(DexAnnotationSetItem*,
		  dx_annotationsetitem,
		  dx->an_directories[j]->fields_size,
		  dx->an_directories[j]->field_annotations[i]->annotations_off,
		  dx->an_set,
		  dx->meta.an_set_size,
		  dx->meta.an_set_alloc);

    DX_READ_TABLE(DexAnnotationSetItem*,
		  dx_annotationsetitem,
		  dx->an_directories[j]->annotated_methods_size,
		  dx->an_directories[j]->method_annotations[i]->annotations_off,
		  dx->an_set,
		  dx->meta.an_set_size,
		  dx->meta.an_set_alloc);

    DX_READ_TABLE(DexAnnotationSetRefList*,
		  dx_annotationsetreflist,
		  dx->an_directories[j]->annotated_parameters_size,
		  dx->an_directories[j]->parameter_annotations[i]->annotations_off,
		  dx->an_set_ref_lists,
		  dx->meta.an_set_ref_lists_size,
		  dx->meta.an_set_ref_lists_alloc);
  }

  //data from annotation set ref lists
  for (j=0; j<dx->meta.an_set_ref_lists_size; j++) {
    if (dx->an_set_ref_lists[j]->meta.corrupted) continue;
    
    DX_READ_TABLE(DexAnnotationSetItem*,
		  dx_annotationsetitem,
		  dx->an_set_ref_lists[j]->size,
		  dx->an_set_ref_lists[j]->list[i]->annotations_off,
		  dx->an_set,
		  dx->meta.an_set_size,
		  dx->meta.an_set_alloc);
  }

  //data from annotation set items
  dx->meta.annotations_size = 0;
  dx->meta.annotations_alloc = dx->meta.an_set_size;

  DX_ALLOC_LIST(DexAnnotationItem*,
		dx->annotations,dx->meta.annotations_alloc);


  for (j=0; j<dx->meta.an_set_size; j++) {
    if (dx->an_set[j]->meta.corrupted) continue;
    
    DX_READ_TABLE(DexAnnotationItem*,
		  dx_annotationitem,
		  dx->an_set[j]->size,
		  dx->an_set[j]->entries[i]->annotation_off,
		  dx->annotations,
		  dx->meta.annotations_size,
		  dx->meta.annotations_alloc);
  }

  return dx;
}
