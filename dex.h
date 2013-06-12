#ifndef __DEX__
#define __DEX__

#include <stdint.h>
#include <stddef.h>

typedef struct _DexHeaderItem {
  unsigned int corrupted;
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
  unsigned int corrupted;
  uint32_t string_data_off;
} DexStringIdItem;

typedef struct _DexStringDataItem {
  unsigned int corrupted;
  uint32_t length;
} DexStringDataItem;

typedef struct _DexTypeIdItem {
  unsigned int corrupted;
  uint32_t descriptor_idx;
} DexTypeIdItem;

typedef struct _DexProtoIdItem {
  unsigned int corrupted;
  uint32_t shorty_idx;
  uint32_t return_type_idx;
  uint32_t parameters_off;
} DexProtoIdItem;

typedef struct _DexFieldIdItem {
  unsigned int corrupted;
  uint16_t class_idx;
  uint16_t type_idx;
  uint32_t name_idx;
} DexFieldIdItem;

typedef struct _DexMethodIdItem {
  unsigned int corrupted;
  uint16_t class_idx;
  uint16_t proto_idx;
  uint32_t name_idx;
} DexMethodIdItem;

typedef struct _DexClassDefItem {
  unsigned int corrupted;
  uint32_t class_idx;
  uint32_t access_flags;
  uint32_t superclass_idx;
  uint32_t interfaces_off;
  uint32_t source_file_idx;
  uint32_t annotations_off;
  uint32_t class_data_off;
  uint32_t static_values_off;
} DexClassDefItem;

typedef struct _DexEncodedField {
  unsigned int corrupted;
  uint32_t field_idx_diff;
  uint32_t access_flags;
} DexEncodedField;

typedef struct _DexEncodedMethod {
  unsigned int corrupted;
  uint32_t method_idx_diff;
  uint32_t access_flags;
  uint32_t code_off;
} DexEncodedMethod;

typedef struct _DexClassDataItem {
  unsigned int corrupted;
  uint32_t static_fields_size;
  uint32_t instance_fields_size;
  uint32_t direct_methods_size;
  uint32_t virtual_methods_size;

  DexEncodedField* static_fields;
  DexEncodedField* instance_fields;
  DexEncodedMethod* direct_methods;
  DexEncodedMethod* virtual_methods;

  void *_data;
} DexClassDataItem;

typedef struct _DexTypeList {
  unsigned int corrupted;
  uint32_t size;
  uint16_t* list;
} DexTypeList;

typedef struct _DexTryItem {
  unsigned int corrupted;
  uint32_t start_addr;
  uint16_t insns_count;
  uint16_t handler_off;
} DexTryItem;

typedef struct _DexEncodedTypeAddrPair {
  unsigned int corrupted;
  uint32_t type_idx;
  uint32_t addr;
} DexEncodedTypeAddrPair;

typedef struct _DexEncodedCatchHandler {
  unsigned int corrupted;
  int32_t size;
  DexEncodedTypeAddrPair *handlers;
  uint32_t catch_all_address;
} DexEncodedCatchHandler;

typedef struct _DexEncodedCatchHandlerList {
  unsigned int corrupted;
  uint32_t size;
  DexEncodedCatchHandler *list;
} DexEncodedCatchHandlerList;

typedef struct _DexCodeItem {
  unsigned int corrupted;
  uint16_t registers_size;
  uint16_t ins_size;
  uint16_t outs_size;
  uint16_t tries_size;
  uint32_t debug_info_off;
  uint32_t insns_size;
  uint16_t* insns;
  DexTryItem *tries;
  DexEncodedCatchHandlerList handlers;
  uint32_t raw_code_offset;
} DexCodeItem;

typedef struct _DexMapItem {
  unsigned int corrupted;
  uint16_t type;
  uint16_t unused;
  uint32_t size;
  uint32_t offset;
} DexMapItem;

typedef struct _DexMapList {
  unsigned int corrupted;
  uint32_t size;
  DexMapItem* list;
} DexMapList;

#endif
