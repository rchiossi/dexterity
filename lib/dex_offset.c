#include <stdint.h>
#include <stdlib.h>

#include "leb128.h"
#include "dex.h"

#define UPDATE(_off)				\
  do {						\
    if ((_off) >= shift->base)				\
      (_off) += shift->delta;				\
  } while (0)

#define UPDATE_ULEB(_leb)				\
  do {							\
    if (ul128toui((_leb)) >= shift->base)			\
      uitoul128(&(_leb),ul128toui((_leb))+shift->delta);	\
  } while (0)

void add_shift(dx_shift* shift, uint32_t base, int32_t delta) {
  dx_shift* next;
  dx_shift* last;

  printf("Shift Added.\n");

  if (shift == NULL) return;

  next = (dx_shift*) malloc_s(sizeof(dx_shift));

  next->base = base;
  next->delta = delta;
  next->next = NULL;
  
  last = shift;
  while (last->next != NULL) last = last->next;
  
  last->next = next;
}

//align 4
void dxo_header(DexHeaderItem* obj, dx_shift* shift) {
  UPDATE(obj->meta.offset);

  UPDATE(obj->link_off);
  UPDATE(obj->map_off);
  UPDATE(obj->string_ids_off);
  UPDATE(obj->type_ids_off);
  UPDATE(obj->proto_ids_off);
  UPDATE(obj->field_ids_off);
  UPDATE(obj->method_ids_off);
  UPDATE(obj->class_defs_off);
  UPDATE(obj->data_off);
}

//align 4
void dxo_stringid(DexStringIdItem* obj, dx_shift* shift) {
  UPDATE(obj->meta.offset);

  UPDATE(obj->string_data_off);
}

//align 4
void dxo_typeid(DexTypeIdItem* obj, dx_shift* shift) {
  UPDATE(obj->meta.offset);
}

//align 4
void dxo_protoid(DexProtoIdItem* obj, dx_shift* shift) {
  UPDATE(obj->meta.offset);

  UPDATE(obj->parameters_off);
}

//align 4
void dxo_fieldid(DexFieldIdItem* obj, dx_shift* shift) {
  UPDATE(obj->meta.offset);
}

//align 4
void dxo_methodid(DexMethodIdItem* obj, dx_shift* shift) {
  UPDATE(obj->meta.offset);
}

//align 4
void dxo_classdef(DexClassDefItem* obj, dx_shift* shift) {
  UPDATE(obj->meta.offset);

  UPDATE(obj->interfaces_off);
  UPDATE(obj->annotations_off);
  UPDATE(obj->class_data_off);
  UPDATE(obj->static_values_off);
}

void dxo_stringdata(DexStringDataItem* obj, dx_shift* shift) {
  UPDATE(obj->meta.offset);
}

void dxo_encodedfield(DexEncodedFieldItem* obj, dx_shift* shift) {
  UPDATE(obj->meta.offset);
}

void dxo_encodedmethod(DexEncodedMethodItem* obj, dx_shift* shift) {
  size_t old_size;

  UPDATE(obj->meta.offset);
  
  old_size = obj->code_off.size;
  
  UPDATE_ULEB(obj->code_off);

  if (obj->code_off.size != old_size)
    add_shift(shift,obj->meta.offset + 1,obj->code_off.size - old_size);
}

void dxo_classdata(DexClassDataItem* obj, dx_shift* shift) {
  unsigned int i;

  UPDATE(obj->meta.offset);

  for (i=0; i<ul128toui(obj->static_fields_size); i++)
    dxo_encodedfield(obj->static_fields[i],shift);

  for (i=0; i<ul128toui(obj->instance_fields_size); i++)
    dxo_encodedfield(obj->instance_fields[i],shift);

  for (i=0; i<ul128toui(obj->direct_methods_size); i++)
    dxo_encodedmethod(obj->direct_methods[i],shift);

  for (i=0; i<ul128toui(obj->virtual_methods_size); i++)
    dxo_encodedmethod(obj->virtual_methods[i],shift);
}

void dxo_typeitem(DexTypeItem* obj, dx_shift* shift) {
  UPDATE(obj->meta.offset);
}

//align 4
void dxo_typelist(DexTypeList* obj, dx_shift* shift) {
  unsigned int i;

  UPDATE(obj->meta.offset);

  for (i=0; i<obj->size; i++)
    dxo_typeitem(obj->list[i],shift);
}

void dxo_tryitem(DexTryItem* obj, dx_shift* shift) {
  UPDATE(obj->meta.offset);
}

void dxo_encodedtypeaddrpair(DexEncodedTypeAddrPair* obj, dx_shift* shift) {
  UPDATE(obj->meta.offset);
}

void dxo_encodedcatchhandler(DexEncodedCatchHandler* obj, dx_shift* shift) {
  int i;

  UPDATE(obj->meta.offset);

  for (i=0; i<abs(sl128toi(obj->size)); i++)
    dxo_encodedtypeaddrpair(obj->handlers[i],shift);
}

void dxo_encodedcatchhandlerlist(DexEncodedCatchHandlerList* obj, dx_shift* shift) {
  unsigned int i;

  UPDATE(obj->meta.offset);

  for (i=0; i<ul128toui(obj->size); i++)
    dxo_encodedcatchhandler(obj->list[i],shift);
}

//align 4
void dxo_codeitem(DexCodeItem* obj, dx_shift* shift) {
  unsigned int i;

  UPDATE(obj->meta.offset);

  UPDATE(obj->debug_info_off);

  if (obj->tries_size > 0) {
    for (i=0; i<obj->tries_size; i++)
      dxo_tryitem(obj->tries[i],shift);

    dxo_encodedcatchhandlerlist(obj->handlers,shift);    
  }
}

void dxo_debuginfo(DexDebugInfo* obj, dx_shift* shift) {
  UPDATE(obj->meta.offset);
}

void dxo_mapitem(DexMapItem* obj, dx_shift* shift) {
  UPDATE(obj->meta.offset);

  UPDATE(obj->offset);
}

//align 4
void dxo_maplist(DexMapList* obj, dx_shift* shift) {
  unsigned int i;

  UPDATE(obj->meta.offset);

  for (i=0; i<obj->size; i++)
    dxo_mapitem(obj->list[i],shift);
}

void dxo_encodedvalue(DexEncodedValue* obj, dx_shift* shift) {
  uint8_t value_type;

  UPDATE(obj->meta.offset);

  value_type = (obj->argtype & 0x1f);
	       
  switch (value_type) {
  case 0x1c:
    dxo_encodedarray((DexEncodedArray*) obj->value,shift);
    break;
  case 0x1d:
    dxo_encodedannotation((DexEncodedAnnotation*)obj->value,shift);
    break;
  default:
    break;
  }
}

void dxo_encodedarray(DexEncodedArray* obj, dx_shift* shift) {
  unsigned int i;
  
  UPDATE(obj->meta.offset);

  for (i=0; i<ul128toui(obj->size); i++)
    dxo_encodedvalue(obj->values[i],shift);
}

void dxo_annotationelement(DexAnnotationElement* obj, dx_shift* shift) {
  UPDATE(obj->meta.offset);

  dxo_encodedvalue(obj->value,shift);
}

void dxo_encodedannotation(DexEncodedAnnotation* obj, dx_shift* shift) {
  unsigned int i;

  UPDATE(obj->meta.offset);

  for (i=0; i<ul128toui(obj->size); i++)
    dxo_annotationelement(obj->elements[i],shift);
}

void dxo_fieldannotation(DexFieldAnnotation* obj, dx_shift* shift) {
  UPDATE(obj->meta.offset);

  UPDATE(obj->annotations_off);
}

void dxo_methodannotation(DexMethodAnnotation* obj, dx_shift* shift) {
  UPDATE(obj->meta.offset);

  UPDATE(obj->annotations_off);
}

void dxo_parameterannotation(DexParameterAnnotation* obj, dx_shift* shift) {
  UPDATE(obj->meta.offset);

  UPDATE(obj->annotations_off);
}

//align 4
void dxo_annotationdirectoryitem(DexAnnotationDirectoryItem* obj, dx_shift* shift) {
  unsigned int i;

  UPDATE(obj->meta.offset);

  UPDATE(obj->class_annotations_off);

  for (i=0; i<obj->fields_size; i++)
    dxo_fieldannotation(obj->field_annotations[i],shift);

  for (i=0; i<obj->annotated_methods_size; i++)
    dxo_methodannotation(obj->method_annotations[i],shift);
  
  for (i=0; i<obj->annotated_parameters_size; i++)
    dxo_parameterannotation(obj->parameter_annotations[i],shift);
}

void dxo_annotationsetrefitem(DexAnnotationSetRefItem* obj, dx_shift* shift) {
  UPDATE(obj->meta.offset);

  UPDATE(obj->annotations_off);
}

//align 4
void dxo_annotationsetreflist(DexAnnotationSetRefList* obj, dx_shift* shift) {
  unsigned int i;

  UPDATE(obj->meta.offset);

  for (i=0; i<obj->size; i++)
    dxo_annotationsetrefitem(obj->list[i],shift);
}

void dxo_annotationoffitem(DexAnnotationOffItem* obj, dx_shift* shift) {
  UPDATE(obj->meta.offset);

  UPDATE(obj->annotation_off);
}

//align 4
void dxo_annotationsetitem(DexAnnotationSetItem* obj, dx_shift* shift) {
  unsigned int i;

  UPDATE(obj->meta.offset);

  for (i=0; i<obj->size; i++)
    dxo_annotationoffitem(obj->entries[i],shift);
}

void dxo_annotationitem(DexAnnotationItem* obj, dx_shift* shift) {
  UPDATE(obj->meta.offset);

  dxo_encodedannotation(obj->annotation,shift);
}

void dxo_encodedarrayitem(DexEncodedArrayItem* obj, dx_shift* shift) {
  UPDATE(obj->meta.offset);

  dxo_encodedarray(obj->value,shift);
}

void dx_shift_offset(Dex* dx, uint32_t base, int32_t delta) {
  unsigned int i;

  dx_shift *shift, *prev;

  if (dx == NULL) return;

  shift = (dx_shift*) malloc_s(sizeof(dx_shift));

  shift->base = base;
  shift->delta = delta;
  shift->next = NULL;
  
  dxo_header(dx->header,shift);
  dxo_maplist(dx->map_list,shift);

  for (i=0; i<dx->header->string_ids_size; i++)
    dxo_stringid(dx->string_ids[i],shift);
  
  for (i=0; i<dx->header->type_ids_size; i++)
    dxo_typeid(dx->type_ids[i],shift);
  
  for (i=0; i<dx->header->proto_ids_size; i++)
    dxo_protoid(dx->proto_ids[i],shift);
  
  for (i=0; i<dx->header->field_ids_size; i++)
    dxo_fieldid(dx->field_ids[i],shift);
  
  for (i=0; i<dx->header->method_ids_size; i++)
    dxo_methodid(dx->method_ids[i],shift);
  
  for (i=0; i<dx->header->class_defs_size; i++)
    dxo_classdef(dx->class_defs[i],shift);
    
  //Data
  for (i=0; i<dx->meta.string_data_list_size; i++)
    dxo_stringdata(dx->string_data_list[i],shift);
  
  for (i=0; i<dx->meta.type_lists_size; i++)
    dxo_typelist(dx->type_lists[i],shift);
  
  for (i=0; i<dx->meta.an_directories_size; i++)
    dxo_annotationdirectoryitem(dx->an_directories[i],shift);
  
  for (i=0; i<dx->meta.class_data_size; i++)
    dxo_classdata(dx->class_data[i],shift);
  
  for (i=0; i<dx->meta.encoded_arrays_size; i++)
    dxo_encodedarray(dx->encoded_arrays[i],shift);
  
  for (i=0; i<dx->meta.code_list_size; i++)
    dxo_codeitem(dx->code_list[i],shift);
  
  for (i=0; i<dx->meta.debug_info_list_size; i++)
    dxo_debuginfo(dx->debug_info_list[i],shift);
  
  for (i=0; i<dx->meta.an_set_size; i++)
    dxo_annotationsetitem(dx->an_set[i],shift);
  
  for (i=0; i<dx->meta.an_set_ref_lists_size; i++)
    dxo_annotationsetreflist(dx->an_set_ref_lists[i],shift);
  
  for (i=0; i<dx->meta.annotations_size; i++)
    dxo_annotationitem(dx->annotations[i],shift);  

  prev = shift;
  shift = shift->next;
  free(prev);

  //ULEB expansion
  while (shift != NULL) {
    printf("delta1 = %d\n",shift->delta);
    dx->header->file_size += shift->delta;

    if (shift->base > dx->header->data_off)
      dx->header->data_size += shift->delta;

    dx_shift_offset(dx,shift->base,shift->delta);

    prev = shift;
    shift = shift->next;
    free(prev);
  }
}
