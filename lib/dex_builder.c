#include <stdint.h>
#include <stdlib.h>

#include "bytestream.h"
#include "dex.h"

#define DXB_FIXED(_name,_type)				\
  DXBUILD(_name,_type) {				\
    if (bs == NULL || obj == NULL) return;		\
							\
    bsseek(bs,obj->meta.offset);			\
							\
    dxwrite(bs,(uint8_t*)obj,sizeof(_type));		\
  }							\

//write aux
void dxwrite(ByteStream* bs, uint8_t* buf, size_t size) {
  uint8_t* ptr = buf += sizeof(Metadata);
  size_t data_size = size - sizeof(Metadata);

  bswrite(bs,ptr,data_size);
}

//Build Functions
DXB_FIXED(dxb_header,DexHeaderItem)
DXB_FIXED(dxb_stringid,DexStringIdItem)
DXB_FIXED(dxb_typeid,DexTypeIdItem)
DXB_FIXED(dxb_protoid,DexProtoIdItem)
DXB_FIXED(dxb_fieldid,DexFieldIdItem)
DXB_FIXED(dxb_methodid,DexMethodIdItem)
DXB_FIXED(dxb_classdef,DexClassDefItem)

void dxb_stringdata(ByteStream* bs, DexStringDataItem* obj) {
  uint8_t* tape;
  char end[] = "\x00";

  if (bs == NULL || obj == NULL) return;

  bsseek(bs,obj->meta.offset);

  l128write(bs,&(obj->size));

  tape = (uint8_t*) obj->data;

  if (ul128toui(obj->size) != 0x0) {
    while(*tape) {
      bswrite(bs,tape,sizeof(uint8_t));

      if ((*tape & 0xe0) == 0xc0 ) {
	tape += 1;
	bswrite(bs,tape,sizeof(uint8_t));
      }

      else if ((*tape & 0xf0) == 0xe0) {
	tape += 1;
	bswrite(bs,tape,sizeof(uint8_t)*2);
	tape += 1;
      }

      tape++;
    }
  }

  bswrite(bs,(uint8_t*)end,sizeof(uint8_t));
}

void dxb_encodedfield(ByteStream* bs, DexEncodedFieldItem* obj) {
  if (bs == NULL || obj == NULL) return;

  bsseek(bs,obj->meta.offset);

  l128write(bs,&(obj->field_idx_diff));
  l128write(bs,&(obj->access_flags));
}

void dxb_encodedmethod(ByteStream* bs, DexEncodedMethodItem* obj) {
  if (bs == NULL || obj == NULL) return;

  bsseek(bs,obj->meta.offset);
  l128write(bs,&(obj->method_idx_diff));
  l128write(bs,&(obj->access_flags));
  l128write(bs,&(obj->code_off));
}

void dxb_classdata(ByteStream* bs, DexClassDataItem* obj) {
  unsigned int i;

  if (bs == NULL || obj == NULL) return;

  bsseek(bs,obj->meta.offset);

  l128write(bs,&(obj->static_fields_size));
  l128write(bs,&(obj->instance_fields_size));
  l128write(bs,&(obj->direct_methods_size));
  l128write(bs,&(obj->virtual_methods_size));

  for (i=0; i<ul128toui(obj->static_fields_size); i++)
    dxb_encodedfield(bs,obj->static_fields[i]);

  for (i=0; i<ul128toui(obj->instance_fields_size); i++)
    dxb_encodedfield(bs,obj->instance_fields[i]);

  for (i=0; i<ul128toui(obj->direct_methods_size); i++)
    dxb_encodedmethod(bs,obj->direct_methods[i]);

  for (i=0; i<ul128toui(obj->virtual_methods_size); i++)
    dxb_encodedmethod(bs,obj->virtual_methods[i]);
}

DXB_FIXED(dxb_typeitem,DexTypeItem)

void dxb_typelist(ByteStream* bs, DexTypeList* obj) {
  unsigned int i;

  if (bs == NULL || obj == NULL) return;

  bsseek(bs,obj->meta.offset);

  bswrite(bs,(uint8_t*) &(obj->size),sizeof(uint32_t));

  for (i=0; i<obj->size; i++)
    dxb_typeitem(bs,obj->list[i]);
}

DXB_FIXED(dxb_tryitem,DexTryItem)

void dxb_encodedtypeaddrpair(ByteStream* bs, DexEncodedTypeAddrPair* obj) {
  if (bs == NULL || obj == NULL) return;

  bsseek(bs,obj->meta.offset);

  l128write(bs,&(obj->type_idx));
  l128write(bs,&(obj->addr));
}

void dxb_encodedcatchhandler(ByteStream* bs, DexEncodedCatchHandler* obj) {
  int i;

  if (bs == NULL || obj == NULL) return;

  bsseek(bs,obj->meta.offset);

  l128write(bs,&(obj->size));

  for (i=0; i<abs(sl128toi(obj->size)); i++)
    dxb_encodedtypeaddrpair(bs,obj->handlers[i]);

  if (sl128toi(obj->size) <= 0) 
    l128write(bs,&(obj->catch_all_addr));
}

void dxb_encodedcatchhandlerlist(ByteStream* bs, DexEncodedCatchHandlerList* obj) {
  unsigned int i;

  if (bs == NULL || obj == NULL) return;

  bsseek(bs,obj->meta.offset);

  l128write(bs,&(obj->size));

  for (i=0; i<ul128toui(obj->size); i++)
    dxb_encodedcatchhandler(bs,obj->list[i]);
}

void dxb_codeitem(ByteStream* bs, DexCodeItem* obj) {
  unsigned int i;

  if (bs == NULL || obj == NULL) return;

  bsseek(bs,obj->meta.offset);

  bswrite(bs,(uint8_t*) &(obj->registers_size),sizeof(uint16_t));
  bswrite(bs,(uint8_t*) &(obj->ins_size),sizeof(uint16_t));
  bswrite(bs,(uint8_t*) &(obj->outs_size),sizeof(uint16_t));
  bswrite(bs,(uint8_t*) &(obj->tries_size),sizeof(uint16_t));
  bswrite(bs,(uint8_t*) &(obj->debug_info_off),sizeof(uint32_t));
  bswrite(bs,(uint8_t*) &(obj->insns_size),sizeof(uint32_t));

  for (i=0; i<obj->insns_size; i++)
    bswrite(bs,(uint8_t*) &(obj->insns[i]),sizeof(uint16_t));

  if (obj->tries_size > 0) {
    if (obj->insns_size % 2 != 0)
      bswrite(bs,(uint8_t*) &(obj->padding),sizeof(uint16_t));

    for (i=0; i<obj->tries_size; i++)
     dxb_tryitem(bs,obj->tries[i]);

    dxb_encodedcatchhandlerlist(bs,obj->handlers);    
  }
}

void dxb_debuginfo(ByteStream* bs, DexDebugInfo* obj) {
  unsigned int i;

  if (bs == NULL || obj == NULL) return;

  bsseek(bs,obj->meta.offset);

  l128write(bs,&(obj->line_start));
  l128write(bs,&(obj->parameters_size));

  for (i=0; i<ul128toui(obj->parameters_size); i++)
    l128write(bs,&(obj->parameter_names[i]));

  dxb_debug_state_machine(bs,obj->state_machine);
}

DXB_FIXED(dxb_mapitem,DexMapItem)

void dxb_maplist(ByteStream* bs, DexMapList* obj) {
  unsigned int i;

  if (bs == NULL || obj == NULL) return;

  bsseek(bs,obj->meta.offset);

  bswrite(bs,(uint8_t*)&(obj->size),sizeof(uint32_t));

  for (i=0; i<obj->size; i++)
    dxb_mapitem(bs,obj->list[i]);
}

void dxb_encodedvalue(ByteStream* bs, DexEncodedValue* obj) {
  int i;

  uint8_t value_type;
  uint8_t value_arg;

  if (bs == NULL || obj == NULL) return;

  bsseek(bs,obj->meta.offset);

  bswrite(bs,(uint8_t*)&(obj->argtype),sizeof(uint8_t));

  value_type = (obj->argtype & 0x1f);
  value_arg  = ((obj->argtype >> 5) & 0x7);
	       
  switch (value_type) {
  case 0x00:
    bswrite(bs,(uint8_t*) obj->value,sizeof(uint8_t));
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
    for (i=0; i<value_arg+1; i++)
      bswrite(bs,(uint8_t*) &(obj->value[i]),sizeof(uint8_t));   

    break;
  case 0x1c:
    dxb_encodedarray(bs,(DexEncodedArray*) obj->value);
    break;
  case 0x1d:
    dxb_encodedannotation(bs,(DexEncodedAnnotation*)obj->value);
    break;
  case 0x1e:
  case 0x1f:
    break;
  default:
    break;
  }
}

void dxb_encodedarray(ByteStream* bs, DexEncodedArray* obj) {
  unsigned int i;

  if (bs == NULL || obj == NULL) return;

  bsseek(bs,obj->meta.offset);

  l128write(bs,&(obj->size));

  for (i=0; i<ul128toui(obj->size); i++)
    dxb_encodedvalue(bs,obj->values[i]);
}

void dxb_annotationelement(ByteStream* bs, DexAnnotationElement* obj) {
  if (bs == NULL || obj == NULL) return;

  bsseek(bs,obj->meta.offset);

  l128write(bs,&(obj->name_idx));

  dxb_encodedvalue(bs,obj->value);
}

void dxb_encodedannotation(ByteStream* bs, DexEncodedAnnotation* obj) {
  unsigned int i;

  if (bs == NULL || obj == NULL) return;

  bsseek(bs,obj->meta.offset);

  l128write(bs,&(obj->type_idx));
  l128write(bs,&(obj->size));

  for (i=0; i<ul128toui(obj->size); i++)
    dxb_annotationelement(bs,obj->elements[i]);
}

void dxb_debug_state_machine(ByteStream* bs, uint8_t* obj) {
  uint32_t size;

  if (bs == NULL || obj == NULL) return;

  size = ((uint32_t*) obj)[0];

  bswrite(bs,(obj+sizeof(uint32_t)),size);
}

DXB_FIXED(dxb_fieldannotation,DexFieldAnnotation)
DXB_FIXED(dxb_methodannotation,DexMethodAnnotation)
DXB_FIXED(dxb_parameterannotation,DexParameterAnnotation)

void dxb_annotationdirectoryitem(ByteStream* bs, DexAnnotationDirectoryItem* obj) {
  unsigned int i;

  if (bs == NULL || obj == NULL) return;

  bsseek(bs,obj->meta.offset);

  bswrite(bs,(uint8_t*) &(obj->class_annotations_off),sizeof(uint32_t));
  bswrite(bs,(uint8_t*) &(obj->fields_size),sizeof(uint32_t));
  bswrite(bs,(uint8_t*) &(obj->annotated_methods_size),sizeof(uint32_t));
  bswrite(bs,(uint8_t*) &(obj->annotated_parameters_size),sizeof(uint32_t));

  for (i=0; i<obj->fields_size; i++)
    dxb_fieldannotation(bs,obj->field_annotations[i]);

  for (i=0; i<obj->annotated_methods_size; i++)
    dxb_methodannotation(bs,obj->method_annotations[i]);
  
  for (i=0; i<obj->annotated_parameters_size; i++)
    dxb_parameterannotation(bs,obj->parameter_annotations[i]);
}

DXB_FIXED(dxb_annotationsetrefitem,DexAnnotationSetRefItem)

void dxb_annotationsetreflist(ByteStream* bs, DexAnnotationSetRefList* obj) {
  unsigned int i;

  if (bs == NULL || obj == NULL) return;

  bsseek(bs,obj->meta.offset);

  bswrite(bs,(uint8_t*) &(obj->size),sizeof(uint32_t));

  for (i=0; i<obj->size; i++)
    dxb_annotationsetrefitem(bs,obj->list[i]);
}

DXB_FIXED(dxb_annotationoffitem,DexAnnotationOffItem)

void dxb_annotationsetitem(ByteStream* bs, DexAnnotationSetItem* obj) {
  unsigned int i;

  if (bs == NULL || obj == NULL) return;

  bsseek(bs,obj->meta.offset);

  bswrite(bs,(uint8_t*) &(obj->size),sizeof(uint32_t));

  for (i=0; i<obj->size; i++)
    dxb_annotationoffitem(bs,obj->entries[i]);
}

void dxb_annotationitem(ByteStream* bs, DexAnnotationItem* obj) {
  if (bs == NULL || obj == NULL) return;

  bsseek(bs,obj->meta.offset);

  bswrite(bs,(uint8_t*) &(obj->visibility),sizeof(uint8_t));

  dxb_encodedannotation(bs,obj->annotation);
}

void dxb_encodedarrayitem(ByteStream* bs, DexEncodedArrayItem* obj) {
  if (bs == NULL || obj == NULL) return;

  bsseek(bs,obj->meta.offset);

  dxb_encodedarray(bs,obj->value);
}

void dx_build(Dex* dx, char* filename) {
  ByteStream* bs;
  unsigned int i;

  if (dx == NULL) return;

  bs = bsalloc(dx->header->file_size);

  if (bs == NULL) return;

  
  dxb_header(bs,dx->header);
  dxb_maplist(bs,dx->map_list);

  for (i=0; i<dx->header->string_ids_size; i++)
    dxb_stringid(bs,dx->string_ids[i]);

  for (i=0; i<dx->header->type_ids_size; i++)
    dxb_typeid(bs,dx->type_ids[i]);

  for (i=0; i<dx->header->proto_ids_size; i++)
    dxb_protoid(bs,dx->proto_ids[i]);

  for (i=0; i<dx->header->field_ids_size; i++)
    dxb_fieldid(bs,dx->field_ids[i]);

  for (i=0; i<dx->header->method_ids_size; i++)
    dxb_methodid(bs,dx->method_ids[i]);

  for (i=0; i<dx->header->class_defs_size; i++)
    dxb_classdef(bs,dx->class_defs[i]);

  //Data
  for (i=0; i<dx->meta.string_data_list_size; i++)
    dxb_stringdata(bs,dx->string_data_list[i]);
  
  for (i=0; i<dx->meta.type_lists_size; i++)
    dxb_typelist(bs,dx->type_lists[i]);

  for (i=0; i<dx->meta.an_directories_size; i++)
    dxb_annotationdirectoryitem(bs,dx->an_directories[i]);

  for (i=0; i<dx->meta.class_data_size; i++)
    dxb_classdata(bs,dx->class_data[i]);

  for (i=0; i<dx->meta.encoded_arrays_size; i++)
    dxb_encodedarray(bs,dx->encoded_arrays[i]);

  for (i=0; i<dx->meta.code_list_size; i++)
    dxb_codeitem(bs,dx->code_list[i]);

  for (i=0; i<dx->meta.debug_info_list_size; i++)
    dxb_debuginfo(bs,dx->debug_info_list[i]);

  for (i=0; i<dx->meta.an_set_size; i++)
    dxb_annotationsetitem(bs,dx->an_set[i]);

  for (i=0; i<dx->meta.an_set_ref_lists_size; i++)
    dxb_annotationsetreflist(bs,dx->an_set_ref_lists[i]);

  for (i=0; i<dx->meta.annotations_size; i++)
    dxb_annotationitem(bs,dx->annotations[i]);

  bssave(bs,filename);

  bsfree(bs);
}
