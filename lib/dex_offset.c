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

void dxo_header(DexHeaderItem* obj, uint32_t base, int32_t delta) {
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

void dxo_stringid(DexStringIdItem* obj, uint32_t base, int32_t delta) {
  UPDATE(obj->meta.offset);

  UPDATE(obj->string_data_off);
}

void dxo_typeid(DexTypeIdItem* obj, uint32_t base, int32_t delta) {
  UPDATE(obj->meta.offset);
}

void dxo_protoid(DexProtoIdItem* obj, uint32_t base, int32_t delta) {
  UPDATE(obj->meta.offset);

  UPDATE(obj->parameters_off);
}

void dxo_fieldid(DexFieldIdItem* obj, uint32_t base, int32_t delta) {
  UPDATE(obj->meta.offset);
}

void dxo_methodid(DexMethodIdItem* obj, uint32_t base, int32_t delta) {
  UPDATE(obj->meta.offset);
}

void dxo_classdef(DexClassDefItem* obj, uint32_t base, int32_t delta) {
  UPDATE(obj->meta.offset);

  UPDATE(obj->interfaces_off);
  UPDATE(obj->annotations_off);
  UPDATE(obj->class_data_off);
  UPDATE(obj->static_values_off);
}

void dxo_stringdata(DexStringDataItem* obj, uint32_t base, int32_t delta) {
  UPDATE(obj->meta.offset);
}

void dxo_encodedfield(DexEncodedFieldItem* obj, uint32_t base, int32_t delta) {
  UPDATE(obj->meta.offset);
}

void dxo_encodedmethod(DexEncodedMethodItem* obj, uint32_t base, int32_t delta) {
  UPDATE(obj->meta.offset);

  UPDATE_ULEB(obj->code_off);
}

void dxo_classdata(DexClassDataItem* obj, uint32_t base, int32_t delta) {
  int i;

  UPDATE(obj->meta.offset);

  for (i=0; i<ul128toui(obj->static_fields_size); i++)
    dxo_encodedfield(obj->static_fields[i],base,delta);

  for (i=0; i<ul128toui(obj->instance_fields_size); i++)
    dxo_encodedfield(obj->instance_fields[i],base,delta);

  for (i=0; i<ul128toui(obj->direct_methods_size); i++)
    dxo_encodedmethod(obj->direct_methods[i],base,delta);

  for (i=0; i<ul128toui(obj->virtual_methods_size); i++)
    dxo_encodedmethod(obj->virtual_methods[i],base,delta);
}

void dxo_typeitem(DexTypeItem* obj, uint32_t base, int32_t delta) {
  UPDATE(obj->meta.offset);
}

void dxo_typelist(DexTypeList* obj, uint32_t base, int32_t delta) {
  int i;

  UPDATE(obj->meta.offset);

  for (i=0; i<obj->size; i++)
    dxo_typeitem(obj->list[i],base,delta);
}

void dxo_tryitem(DexTryItem* obj, uint32_t base, int32_t delta) {
  UPDATE(obj->meta.offset);
}

void dxo_encodedtypeaddrpair(DexEncodedTypeAddrPair* obj, uint32_t base, int32_t delta) {
  UPDATE(obj->meta.offset);
}

void dxo_encodedcatchhandler(DexEncodedCatchHandler* obj, uint32_t base, int32_t delta) {
  int i;

  UPDATE(obj->meta.offset);

  for (i=0; i<abs(sl128toi(obj->size)); i++)
    dxo_encodedtypeaddrpair(obj->handlers[i],base,delta);
}

void dxo_encodedcatchhandlerlist(DexEncodedCatchHandlerList* obj, uint32_t base, int32_t delta) {
  int i;

  UPDATE(obj->meta.offset);

  for (i=0; i<ul128toui(obj->size); i++)
    dxo_encodedcatchhandler(obj->list[i],base,delta);
}

void dxo_codeitem(DexCodeItem* obj, uint32_t base, int32_t delta) {
  int i;

  UPDATE(obj->meta.offset);

  UPDATE(obj->debug_info_off);

  if (obj->tries_size > 0) {
    for (i=0; i<obj->tries_size; i++)
      dxo_tryitem(obj->tries[i],base,delta);

    dxo_encodedcatchhandlerlist(obj->handlers,base,delta);    
  }
}

void dxo_debuginfo(DexDebugInfo* obj, uint32_t base, int32_t delta) {
  UPDATE(obj->meta.offset);
}

void dxo_mapitem(DexMapItem* obj, uint32_t base, int32_t delta) {
  UPDATE(obj->meta.offset);

  UPDATE(obj->offset);
}

void dxo_maplist(DexMapList* obj, uint32_t base, int32_t delta) {
  int i;

  UPDATE(obj->meta.offset);

  for (i=0; i<obj->size; i++)
    dxo_mapitem(obj->list[i],base,delta);
}

void dxo_encodedvalue(DexEncodedValue* obj, uint32_t base, int32_t delta) {
  int i;
  uint8_t value_type;

  UPDATE(obj->meta.offset);

  value_type = (obj->argtype & 0x1f);
	       
  switch (value_type) {
  case 0x1c:
    dxo_encodedarray((DexEncodedArray*) obj->value,base,delta);
    break;
  case 0x1d:
    dxo_encodedannotation((DexEncodedAnnotation*)obj->value,base,delta);
    break;
  default:
    break;
  }
}

void dxo_encodedarray(DexEncodedArray* obj, uint32_t base, int32_t delta) {
  int i;
  
  UPDATE(obj->meta.offset);

  for (i=0; i<ul128toui(obj->size); i++)
    dxo_encodedvalue(obj->values[i],base,delta);
}

void dxo_annotationelement(DexAnnotationElement* obj, uint32_t base, int32_t delta) {
  UPDATE(obj->meta.offset);

  dxo_encodedvalue(obj->value,base,delta);
}

void dxo_encodedannotation(DexEncodedAnnotation* obj, uint32_t base, int32_t delta) {
  int i;

  UPDATE(obj->meta.offset);

  for (i=0; i<ul128toui(obj->size); i++)
    dxo_annotationelement(obj->elements[i],base,delta);
}

void dxo_fieldannotation(DexFieldAnnotation* obj, uint32_t base, int32_t delta) {
  UPDATE(obj->meta.offset);

  UPDATE(obj->annotations_off);
}

void dxo_methodannotation(DexMethodAnnotation* obj, uint32_t base, int32_t delta) {
  UPDATE(obj->meta.offset);

  UPDATE(obj->annotations_off);
}

void dxo_parameterannotation(DexParameterAnnotation* obj, uint32_t base, int32_t delta) {
  UPDATE(obj->meta.offset);

  UPDATE(obj->annotations_off);
}

void dxo_annotationdirectoryitem(DexAnnotationDirectoryItem* obj, uint32_t base, int32_t delta) {
  int i;

  UPDATE(obj->meta.offset);

  UPDATE(obj->class_annotations_off);

  for (i=0; i<obj->fields_size; i++)
    dxo_fieldannotation(obj->field_annotations[i],base,delta);

  for (i=0; i<obj->annotated_methods_size; i++)
    dxo_methodannotation(obj->method_annotations[i],base,delta);
  
  for (i=0; i<obj->annotated_parameters_size; i++)
    dxo_parameterannotation(obj->parameter_annotations[i],base,delta);
}

void dxo_annotationsetrefitem(DexAnnotationSetRefItem* obj, uint32_t base, int32_t delta) {
  UPDATE(obj->meta.offset);

  UPDATE(obj->annotations_off);
}

void dxo_annotationsetreflist(DexAnnotationSetRefList* obj, uint32_t base, int32_t delta) {
  int i;

  UPDATE(obj->meta.offset);

  for (i=0; i<obj->size; i++)
    dxo_annotationsetrefitem(obj->list[i],base,delta);
}

void dxo_annotationoffitem(DexAnnotationOffItem* obj, uint32_t base, int32_t delta) {
  UPDATE(obj->meta.offset);

  UPDATE(obj->annotation_off);
}

void dxo_annotationsetitem(DexAnnotationSetItem* obj, uint32_t base, int32_t delta) {
  int i;

  UPDATE(obj->meta.offset);

  for (i=0; i<obj->size; i++)
    dxo_annotationoffitem(obj->entries[i],base,delta);
}

void dxo_annotationitem(DexAnnotationItem* obj, uint32_t base, int32_t delta) {
  UPDATE(obj->meta.offset);

  dxo_encodedannotation(obj->annotation,base,delta);
}

void dxo_encodedarrayitem(DexEncodedArrayItem* obj, uint32_t base, int32_t delta) {
  UPDATE(obj->meta.offset);

  dxo_encodedarray(obj->value,base,delta);
}
