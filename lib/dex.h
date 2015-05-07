#ifndef __DEX__
#define __DEX__

#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "malloc_s.h"


#include "bytestream.h"
#include "leb128.h"

// Metadata
typedef struct _Metadata {
  bool corrupted;
  uint32_t offset;
} Metadata;

//Dex Structures
typedef struct _DexHeaderItem {
  Metadata meta;
  uint8_t magic[8];
  uint32_t checksum;
  uint8_t signature[20];
  uint32_t file_size;
  uint32_t header_size;
  uint32_t endian_tag;
  uint32_t link_size;
  uint32_t link_off;
  uint32_t map_off;
  uint32_t string_ids_size;
  uint32_t string_ids_off;
  uint32_t type_ids_size;
  uint32_t type_ids_off;
  uint32_t proto_ids_size;
  uint32_t proto_ids_off;
  uint32_t field_ids_size;
  uint32_t field_ids_off;
  uint32_t method_ids_size;
  uint32_t method_ids_off;
  uint32_t class_defs_size;
  uint32_t class_defs_off;
  uint32_t data_size;
  uint32_t data_off;
} DexHeaderItem;

typedef struct _DexStringIdItem {
  Metadata meta;
  uint32_t string_data_off;
} DexStringIdItem;

typedef struct _DexStringDataItem {
  Metadata meta;
  leb128_t size;
  uint8_t* data;
} DexStringDataItem;

typedef struct _DexTypeIdItem {
  Metadata meta;
  uint32_t descriptor_idx;
} DexTypeIdItem;

typedef struct _DexProtoIdItem {
  Metadata meta;
  uint32_t shorty_idx;
  uint32_t return_type_idx;
  uint32_t parameters_off;
} DexProtoIdItem;

typedef struct _DexFieldIdItem {
  Metadata meta;
  uint16_t class_idx;
  uint16_t type_idx;
  uint32_t name_idx;
} DexFieldIdItem;

typedef struct _DexMethodIdItem {
  Metadata meta;
  uint16_t class_idx;
  uint16_t proto_idx;
  uint32_t name_idx;
} DexMethodIdItem;

typedef struct _DexClassDefItem {
  Metadata meta;
  uint32_t class_idx;
  uint32_t access_flags;
  uint32_t superclass_idx;
  uint32_t interfaces_off;
  uint32_t source_file_idx;
  uint32_t annotations_off;
  uint32_t class_data_off;
  uint32_t static_values_off;
} DexClassDefItem;

typedef struct _DexEncodedFieldItem {
  Metadata meta;
  leb128_t field_idx_diff;
  leb128_t access_flags;
} DexEncodedFieldItem;

typedef struct _DexEncodedMethodItem {
  Metadata meta;
  leb128_t method_idx_diff;
  leb128_t access_flags;
  leb128_t code_off;
} DexEncodedMethodItem;

typedef struct _DexClassDataItem {
  Metadata meta;
  leb128_t static_fields_size;
  leb128_t instance_fields_size;
  leb128_t direct_methods_size;
  leb128_t virtual_methods_size;

  DexEncodedFieldItem** static_fields;
  DexEncodedFieldItem** instance_fields;
  DexEncodedMethodItem** direct_methods;
  DexEncodedMethodItem** virtual_methods;
} DexClassDataItem;

typedef struct __attribute__((packed)) _DexTypeItem {
  Metadata meta;
  uint16_t type_idx;
} DexTypeItem;

typedef struct _DexTypeList {
  Metadata meta;
  uint32_t size;
  DexTypeItem** list;
} DexTypeList;

typedef struct _DexTryItem {
  Metadata meta;
  uint32_t start_addr;
  uint16_t insns_count;
  uint16_t handler_off;
} DexTryItem;

typedef struct _DexEncodedTypeAddrPair {
  Metadata meta;
  leb128_t type_idx;
  leb128_t addr;
} DexEncodedTypeAddrPair;

typedef struct _DexEncodedCatchHandler {
  Metadata meta;
  leb128_t size;
  DexEncodedTypeAddrPair** handlers;
  leb128_t catch_all_addr;
} DexEncodedCatchHandler;

typedef struct _DexEncodedCatchHandlerList {
  Metadata meta;
  leb128_t size;
  DexEncodedCatchHandler** list;
} DexEncodedCatchHandlerList;

typedef struct _DexCodeItem {
  Metadata meta;
  uint16_t registers_size;
  uint16_t ins_size;
  uint16_t outs_size;
  uint16_t tries_size;
  uint32_t debug_info_off;
  uint32_t insns_size;
  uint16_t* insns;
  uint16_t padding;
  DexTryItem** tries;
  DexEncodedCatchHandlerList* handlers;
} DexCodeItem;

typedef struct _DexDebugInfo {
  Metadata meta;
  leb128_t line_start;
  leb128_t parameters_size;
  leb128_t* parameter_names;
  uint8_t* state_machine;
} DexDebugInfo;

typedef struct _DexMapItem {
  Metadata meta;
  uint16_t type;
  uint16_t unused;
  uint32_t size;
  uint32_t offset;
} DexMapItem;

typedef struct _DexMapList {
  Metadata meta;
  uint32_t size;
  DexMapItem** list;
} DexMapList;

typedef struct _DexEncodedValue {
  Metadata meta;
  uint8_t argtype;
  uint8_t* value;
} DexEncodedValue;

typedef struct _DexEncodedArray {
  Metadata meta;
  leb128_t size;
  DexEncodedValue** values;  
} DexEncodedArray;

typedef struct _DexAnnotationElement {
  Metadata meta;
  leb128_t name_idx;
  DexEncodedValue* value;
} DexAnnotationElement;

typedef struct _DexEncodedAnnotation {
  Metadata meta;
  leb128_t type_idx;
  leb128_t size;
  DexAnnotationElement** elements;
} DexEncodedAnnotation;

typedef struct _DexFieldAnnotation {
  Metadata meta;
  uint32_t field_idx;
  uint32_t annotations_off;
} DexFieldAnnotation;

typedef struct _DexMethodAnnotation {
  Metadata meta;
  uint32_t method_idx;
  uint32_t annotations_off;
} DexMethodAnnotation;

typedef struct _DexParameterAnnotation {
  Metadata meta;
  uint32_t method_idx;
  uint32_t annotations_off;
} DexParameterAnnotation;

typedef struct _DexAnnotationDirectoryItem {
  Metadata meta;
  uint32_t class_annotations_off;
  uint32_t fields_size;
  uint32_t annotated_methods_size;
  uint32_t annotated_parameters_size;
  DexFieldAnnotation** field_annotations;
  DexMethodAnnotation** method_annotations;
  DexParameterAnnotation** parameter_annotations;  
} DexAnnotationDirectoryItem;

typedef struct _DexAnnotationSetRefItem {
  Metadata meta;
  uint32_t annotations_off;
} DexAnnotationSetRefItem;

typedef struct _DexAnnotationSetRefList {
  Metadata meta;
  uint32_t size;
  DexAnnotationSetRefItem** list;
} DexAnnotationSetRefList;

typedef struct _DexAnnotationOffItem {
  Metadata meta;
  uint32_t annotation_off;
} DexAnnotationOffItem;

typedef struct _DexAnnotationSetItem {
  Metadata meta;
  uint32_t size;
  DexAnnotationOffItem** entries;
} DexAnnotationSetItem;

typedef struct _DexAnnotationItem {
  Metadata meta;
  uint8_t visibility;
  DexEncodedAnnotation* annotation;
} DexAnnotationItem;

typedef struct _DexEncodedArrayItem {
  Metadata meta;
  DexEncodedArray* value;
} DexEncodedArrayItem;

//TODO
typedef struct _Dex {
  DexHeaderItem* header;
  DexMapList* map_list;
  DexStringIdItem** string_ids;
  DexTypeIdItem** type_ids;
  DexProtoIdItem** proto_ids;
  DexFieldIdItem** field_ids;
  DexMethodIdItem** method_ids;
  DexClassDefItem** class_defs;
  DexStringDataItem** string_data_list;
  DexTypeList** type_lists;
  DexAnnotationDirectoryItem** an_directories;
  DexClassDataItem** class_data;
  DexEncodedArray** encoded_arrays;
  DexCodeItem** code_list;
  DexDebugInfo** debug_info_list;
  DexAnnotationSetItem** an_set;
  DexAnnotationSetRefList** an_set_ref_lists;
  DexAnnotationItem** annotations;
  uint8_t* link_data;

  //Allocation metadata
  struct {
    size_t string_ids_alloc;
    size_t type_ids_alloc;
    size_t proto_ids_alloc;
    size_t field_ids_alloc;
    size_t method_ids_alloc;
    size_t class_defs_alloc;
    size_t string_data_list_size;
    size_t string_data_list_alloc;
    size_t type_lists_size; 
    size_t type_lists_alloc; 
    size_t an_directories_size; 
    size_t an_directories_alloc; 
    size_t class_data_size;
    size_t class_data_alloc;
    size_t encoded_arrays_size;
    size_t encoded_arrays_alloc;
    size_t code_list_size;
    size_t code_list_alloc;
    size_t debug_info_list_size;
    size_t debug_info_list_alloc;
    size_t an_set_size;
    size_t an_set_alloc;
    size_t an_set_ref_lists_size;
    size_t an_set_ref_lists_alloc;
    size_t annotations_size;
    size_t annotations_alloc;
  } meta;
} Dex;

//Parse
Dex* dx_parse(ByteStream* bs);

#define DXPARSE(_name,_type) _type* _name (ByteStream* bs, uint32_t offset)

DXPARSE(dx_header,DexHeaderItem);
DXPARSE(dx_stringid,DexStringIdItem);
DXPARSE(dx_typeid,DexTypeIdItem);
DXPARSE(dx_protoid,DexProtoIdItem);
DXPARSE(dx_fieldid,DexFieldIdItem);
DXPARSE(dx_methodid,DexMethodIdItem);
DXPARSE(dx_classdef,DexClassDefItem);
DXPARSE(dx_stringdata,DexStringDataItem);
DXPARSE(dx_encodedfield,DexEncodedFieldItem);
DXPARSE(dx_encodedmethod,DexEncodedMethodItem);
DXPARSE(dx_classdata,DexClassDataItem);
DXPARSE(dx_typeitem,DexTypeItem);
DXPARSE(dx_typelist,DexTypeList);
DXPARSE(dx_tryitem,DexTryItem);
DXPARSE(dx_encodedtypeaddrpair,DexEncodedTypeAddrPair);
DXPARSE(dx_encodedcatchhandler,DexEncodedCatchHandler);
DXPARSE(dx_encodedcatchhandlerlist,DexEncodedCatchHandlerList);
DXPARSE(dx_codeitem,DexCodeItem);
DXPARSE(dx_debuginfo,DexDebugInfo);
DXPARSE(dx_mapitem,DexMapItem);
DXPARSE(dx_maplist,DexMapList);

DXPARSE(dx_encodedvalue,DexEncodedValue);
DXPARSE(dx_encodedarray,DexEncodedArray);
DXPARSE(dx_annotationelement,DexAnnotationElement);
DXPARSE(dx_encodedannotation,DexEncodedAnnotation);

uint8_t* dx_debug_state_machine(ByteStream* bs, uint32_t offset);

DXPARSE(dx_fieldannotation,DexFieldAnnotation);
DXPARSE(dx_methodannotation,DexMethodAnnotation);
DXPARSE(dx_parameterannotation,DexParameterAnnotation);
DXPARSE(dx_annotationdirectoryitem,DexAnnotationDirectoryItem);
DXPARSE(dx_annotationsetrefitem,DexAnnotationSetRefItem);
DXPARSE(dx_annotationsetreflist,DexAnnotationSetRefList);
DXPARSE(dx_annotationoffitem,DexAnnotationOffItem);
DXPARSE(dx_annotationsetitem,DexAnnotationSetItem);
DXPARSE(dx_annotationitem,DexAnnotationItem);
DXPARSE(dx_encodedarrayitem,DexEncodedArrayItem);

//Build
void dx_build(Dex* dx, char* filename);

#define DXBUILD(_name,_type) void _name (ByteStream* bs, _type* obj)

DXBUILD(dxb_header,DexHeaderItem);
DXBUILD(dxb_stringid,DexStringIdItem);
DXBUILD(dxb_typeid,DexTypeIdItem);
DXBUILD(dxb_protoid,DexProtoIdItem);
DXBUILD(dxb_fieldid,DexFieldIdItem);
DXBUILD(dxb_methodid,DexMethodIdItem);
DXBUILD(dxb_classdef,DexClassDefItem);
DXBUILD(dxb_stringdata,DexStringDataItem);
DXBUILD(dxb_encodedfield,DexEncodedFieldItem);
DXBUILD(dxb_encodedmethod,DexEncodedMethodItem);
DXBUILD(dxb_classdata,DexClassDataItem);
DXBUILD(dxb_typeitem,DexTypeItem);
DXBUILD(dxb_typelist,DexTypeList);
DXBUILD(dxb_tryitem,DexTryItem);
DXBUILD(dxb_encodedtypeaddrpair,DexEncodedTypeAddrPair);
DXBUILD(dxb_encodedcatchhandler,DexEncodedCatchHandler);
DXBUILD(dxb_encodedcatchhandlerlist,DexEncodedCatchHandlerList);
DXBUILD(dxb_codeitem,DexCodeItem);
DXBUILD(dxb_debuginfo,DexDebugInfo);
DXBUILD(dxb_mapitem,DexMapItem);
DXBUILD(dxb_maplist,DexMapList);

DXBUILD(dxb_encodedvalue,DexEncodedValue);
DXBUILD(dxb_encodedarray,DexEncodedArray);
DXBUILD(dxb_annotationelement,DexAnnotationElement);
DXBUILD(dxb_encodedannotation,DexEncodedAnnotation);

void dxb_debug_state_machine(ByteStream* bs, uint8_t* obj);

DXBUILD(dxb_fieldannotation,DexFieldAnnotation);
DXBUILD(dxb_methodannotation,DexMethodAnnotation);
DXBUILD(dxb_parameterannotation,DexParameterAnnotation);
DXBUILD(dxb_annotationdirectoryitem,DexAnnotationDirectoryItem);
DXBUILD(dxb_annotationsetrefitem,DexAnnotationSetRefItem);
DXBUILD(dxb_annotationsetreflist,DexAnnotationSetRefList);
DXBUILD(dxb_annotationoffitem,DexAnnotationOffItem);
DXBUILD(dxb_annotationsetitem,DexAnnotationSetItem);
DXBUILD(dxb_annotationitem,DexAnnotationItem);
DXBUILD(dxb_encodedarrayitem,DexEncodedArrayItem);

//Offset shift
typedef struct _dx_shift {
  uint32_t base;
  int32_t delta;

  struct _dx_shift* next;
} dx_shift;

void dx_shift_offset(Dex* dx, uint32_t base, int32_t delta);
void add_shift(dx_shift* shift, uint32_t base, int32_t delta);

#define DXOFFSET(_name,_type) void _name (_type* obj, dx_shift* shift)

DXOFFSET(dxo_header,DexHeaderItem);
DXOFFSET(dxo_stringid,DexStringIdItem);
DXOFFSET(dxo_typeid,DexTypeIdItem);
DXOFFSET(dxo_protoid,DexProtoIdItem);
DXOFFSET(dxo_fieldid,DexFieldIdItem);
DXOFFSET(dxo_methodid,DexMethodIdItem);
DXOFFSET(dxo_classdef,DexClassDefItem);
DXOFFSET(dxo_stringdata,DexStringDataItem);
DXOFFSET(dxo_encodedfield,DexEncodedFieldItem);
DXOFFSET(dxo_encodedmethod,DexEncodedMethodItem);
DXOFFSET(dxo_classdata,DexClassDataItem);
DXOFFSET(dxo_typeitem,DexTypeItem);
DXOFFSET(dxo_typelist,DexTypeList);
DXOFFSET(dxo_tryitem,DexTryItem);
DXOFFSET(dxo_encodedtypeaddrpair,DexEncodedTypeAddrPair);
DXOFFSET(dxo_encodedcatchhandler,DexEncodedCatchHandler);
DXOFFSET(dxo_encodedcatchhandlerlist,DexEncodedCatchHandlerList);
DXOFFSET(dxo_codeitem,DexCodeItem);
DXOFFSET(dxo_debuginfo,DexDebugInfo);
DXOFFSET(dxo_mapitem,DexMapItem);
DXOFFSET(dxo_maplist,DexMapList);

DXOFFSET(dxo_encodedvalue,DexEncodedValue);
DXOFFSET(dxo_encodedarray,DexEncodedArray);
DXOFFSET(dxo_annotationelement,DexAnnotationElement);
DXOFFSET(dxo_encodedannotation,DexEncodedAnnotation);

DXOFFSET(dxo_fieldannotation,DexFieldAnnotation);
DXOFFSET(dxo_methodannotation,DexMethodAnnotation);
DXOFFSET(dxo_parameterannotation,DexParameterAnnotation);
DXOFFSET(dxo_annotationdirectoryitem,DexAnnotationDirectoryItem);
DXOFFSET(dxo_annotationsetrefitem,DexAnnotationSetRefItem);
DXOFFSET(dxo_annotationsetreflist,DexAnnotationSetRefList);
DXOFFSET(dxo_annotationoffitem,DexAnnotationOffItem);
DXOFFSET(dxo_annotationsetitem,DexAnnotationSetItem);
DXOFFSET(dxo_annotationitem,DexAnnotationItem);
DXOFFSET(dxo_encodedarrayitem,DexEncodedArrayItem);

//string_id shift
void dx_shift_stringid(Dex* dx, uint32_t base, int32_t delta);

#define DXSTRINGID(_name,_type) void _name (_type* obj, dx_shift* shift)

DXSTRINGID(dxsi_typeid,DexTypeIdItem);
DXSTRINGID(dxsi_protoid,DexProtoIdItem);
DXSTRINGID(dxsi_fieldid,DexFieldIdItem);
DXSTRINGID(dxsi_methodid,DexMethodIdItem);
DXSTRINGID(dxsi_classdef,DexClassDefItem);
DXSTRINGID(dxsi_debuginfo,DexDebugInfo);
DXSTRINGID(dxsi_encodedvalue,DexEncodedValue);
DXSTRINGID(dxsi_encodedarray,DexEncodedArray);
DXSTRINGID(dxsi_annotationelement,DexAnnotationElement);
DXSTRINGID(dxsi_encodedannotation,DexEncodedAnnotation);
DXSTRINGID(dxsi_annotationitem,DexAnnotationItem);

#endif
