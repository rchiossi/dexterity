#include <stdint.h>
#include <stdlib.h>

#include "leb128.h"
#include "dex.h"

#define UPDATE(_off)				\
  do {						\
    if ((_off) >= base)				\
      (_off) += delta;				\
  } while (0)

#define UPDATE_ULEB(_leb)				\
  do {							\
    if (ul128toui((_leb)) >= base)			\
      uitoul128(&(_leb),ul128toui((_leb))+delta);	\
  } while (0)

#define UPDATE_ULEBP1(_leb)				\
  do {							\
    if (ul128p1toi((_leb)) >= base)			\
      itoul128p1(&(_leb),ul128p1toi((_leb))+delta);	\
  } while (0)


void dxsi_typeid(DexTypeIdItem* obj, uint32_t base, int32_t delta) {
  UPDATE(obj->descriptor_idx);
}

void dxsi_protoid(DexProtoIdItem* obj, uint32_t base, int32_t delta) {
  UPDATE(obj->shorty_idx);
}

void dxsi_fieldid(DexFieldIdItem* obj, uint32_t base, int32_t delta) {
  UPDATE(obj->name_idx);
}

void dxsi_methodid(DexMethodIdItem* obj, uint32_t base, int32_t delta) {
  UPDATE(obj->name_idx);
}

void dxsi_classdef(DexClassDefItem* obj, uint32_t base, int32_t delta) {
  UPDATE(obj->source_file_idx);
}

void dxsi_debuginfo(DexDebugInfo* obj, uint32_t base, int32_t delta) {
  int i;

  for (i=0; i<ul128toui(obj->parameters_size); i++) {
    UPDATE_ULEBP1(obj->parameter_names[i]);
  }
}

void dxsi_encodedvalue(DexEncodedValue* obj, uint32_t base, int32_t delta) {
  int i;
  uint8_t value_type;

  UPDATE(obj->meta.offset);

  value_type = (obj->argtype & 0x1f);
	       
  switch (value_type) {
  case 0x17:
    UPDATE(*((uint32_t*)obj->value));
    break;
  case 0x1C:
    dxsi_encodedarray((DexEncodedArray*) obj->value,base,delta);
    break;
  case 0x1d:
    dxsi_encodedannotation((DexEncodedAnnotation*)obj->value,base,delta);
    break;
  default:
    break;
  }
}

void dxsi_encodedarray(DexEncodedArray* obj, uint32_t base, int32_t delta) {
  int i;
  
  for (i=0; i<ul128toui(obj->size); i++)
    dxsi_encodedvalue(obj->values[i],base,delta);
}

void dxsi_annotationelement(DexAnnotationElement* obj, uint32_t base, int32_t delta) {
  UPDATE_ULEB(obj->name_idx);

  dxsi_encodedvalue(obj->value,base,delta);
}

void dxsi_encodedannotation(DexEncodedAnnotation* obj, uint32_t base, int32_t delta) {
  int i;

  for (i=0; i<ul128toui(obj->size); i++)
    dxsi_annotationelement(obj->elements[i],base,delta);
}

void dxsi_annotationitem(DexAnnotationItem* obj, uint32_t base, int32_t delta) {
  dxsi_encodedannotation(obj->annotation,base,delta);
}
