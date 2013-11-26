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

#define UPDATE_ULEBP1(_leb)				\
  do {							\
    if (ul128p1toi((_leb)) >= (int) (shift->base))			\
      itoul128p1(&(_leb),ul128p1toi((_leb))+shift->delta);	\
  } while (0)


void dxsi_typeid(DexTypeIdItem* obj, dx_shift* shift) {
  UPDATE(obj->descriptor_idx);
}

void dxsi_protoid(DexProtoIdItem* obj, dx_shift* shift) {
  UPDATE(obj->shorty_idx);
}

void dxsi_fieldid(DexFieldIdItem* obj, dx_shift* shift) {
  UPDATE(obj->name_idx);
}

void dxsi_methodid(DexMethodIdItem* obj, dx_shift* shift) {
  UPDATE(obj->name_idx);
}

void dxsi_classdef(DexClassDefItem* obj, dx_shift* shift) {
  UPDATE(obj->source_file_idx);
}

void dxsi_debuginfo(DexDebugInfo* obj, dx_shift* shift) {
  unsigned int i;
  size_t old_size;

  for (i=0; i<ul128toui(obj->parameters_size); i++) {
    old_size = obj->parameter_names[i].size;

    UPDATE_ULEBP1(obj->parameter_names[i]);
    
    if (obj->parameter_names[i].size != old_size)
      add_shift(shift,obj->meta.offset + 1,obj->parameter_names[i].size - old_size);
  }
}

void dxsi_encodedvalue(DexEncodedValue* obj, dx_shift* shift) {
  uint8_t value_type;

  value_type = (obj->argtype & 0x1f);
	       
  switch (value_type) {
  case 0x17:
    UPDATE(*((uint32_t*)obj->value));
    break;
  case 0x1C:
    dxsi_encodedarray((DexEncodedArray*) obj->value,shift);
    break;
  case 0x1d:
    dxsi_encodedannotation((DexEncodedAnnotation*)obj->value,shift);
    break;
  default:
    break;
  }
}

void dxsi_encodedarray(DexEncodedArray* obj, dx_shift* shift) {
  unsigned int i;
  
  for (i=0; i<ul128toui(obj->size); i++)
    dxsi_encodedvalue(obj->values[i],shift);
}

void dxsi_annotationelement(DexAnnotationElement* obj, dx_shift* shift) {
  size_t old_size;
 
  old_size = obj->name_idx.size;

  UPDATE_ULEB(obj->name_idx);
  
  if (obj->name_idx.size != old_size)
    add_shift(shift,obj->meta.offset + 1,obj->name_idx.size - old_size);

  dxsi_encodedvalue(obj->value,shift);
}

void dxsi_encodedannotation(DexEncodedAnnotation* obj, dx_shift* shift) {
  unsigned int i;

  for (i=0; i<ul128toui(obj->size); i++)
    dxsi_annotationelement(obj->elements[i],shift);
}

void dxsi_annotationitem(DexAnnotationItem* obj, dx_shift* shift) {
  dxsi_encodedannotation(obj->annotation,shift);
}

void dx_shift_stringid(Dex* dx, uint32_t base, int32_t delta) {
  unsigned int i;

  dx_shift *shift, *prev;

  if (dx == NULL) return;

  shift = (dx_shift*) malloc_s(sizeof(dx_shift));

  shift->base = base;
  shift->delta = delta;
  shift->next = NULL;

  for (i=0; i<dx->header->type_ids_size; i++)
    dxsi_typeid(dx->type_ids[i],shift);
  
  for (i=0; i<dx->header->proto_ids_size; i++)
    dxsi_protoid(dx->proto_ids[i],shift);
  
  for (i=0; i<dx->header->field_ids_size; i++)
    dxsi_fieldid(dx->field_ids[i],shift);
  
  for (i=0; i<dx->header->method_ids_size; i++)
    dxsi_methodid(dx->method_ids[i],shift);
  
  for (i=0; i<dx->header->class_defs_size; i++)
    dxsi_classdef(dx->class_defs[i],shift);
  
  for (i=0; i<dx->meta.encoded_arrays_size; i++)
    dxsi_encodedarray(dx->encoded_arrays[i],shift);
  
  for (i=0; i<dx->meta.debug_info_list_size; i++)
    dxsi_debuginfo(dx->debug_info_list[i],shift);
  
  for (i=0; i<dx->meta.annotations_size; i++)
    dxsi_annotationitem(dx->annotations[i],shift);

  prev = shift;
  shift = shift->next;
  free(prev);

  //ULEB expansion  
  while (shift != NULL) {
    printf("delta2 = %d\n",shift->delta);
    dx->header->file_size += shift->delta;

    if (shift->base > dx->header->data_off)
      dx->header->data_size += shift->delta;

    dx_shift_offset(dx,shift->base,shift->delta);

    prev = shift;
    shift = shift->next;
    free(prev);
  }
  
}
