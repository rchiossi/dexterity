#ifndef __DEX__
#define __DEX__

#include <stdint.h>
#include <stddef.h>
#include <malloc.h>

#include "bytestream.h"

// Metadata
typedef struct _Metadata {
  unsigned int corrupted;
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
  uint32_t size;
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
  uint32_t field_idx_diff;
  uint32_t access_flags;
} DexEncodedFieldItem;

typedef struct _DexEncodedMethodItem {
  Metadata meta;
  uint32_t method_idx_diff;
  uint32_t access_flags;
  uint32_t code_off;
} DexEncodedMethodItem;

typedef struct _DexClassDataItem {
  Metadata meta;
  uint32_t static_fields_size;
  uint32_t instance_fields_size;
  uint32_t direct_methods_size;
  uint32_t virtual_methods_size;

  DexEncodedFieldItem* static_fields;
  DexEncodedFieldItem* instance_fields;
  DexEncodedMethodItem* direct_methods;
  DexEncodedMethodItem* virtual_methods;

  void *_data;
} DexClassDataItem;

typedef struct _DexTypeList {
  Metadata meta;
  uint32_t size;
  uint16_t* list;
} DexTypeList;

typedef struct _DexTryItem {
  Metadata meta;
  uint32_t start_addr;
  uint16_t insns_count;
  uint16_t handler_off;
} DexTryItem;

typedef struct _DexEncodedTypeAddrPair {
  Metadata meta;
  uint32_t type_idx;
  uint32_t addr;
} DexEncodedTypeAddrPair;

typedef struct _DexEncodedCatchHandler {
  Metadata meta;
  int32_t size;
  DexEncodedTypeAddrPair *handlers;
  uint32_t catch_all_address;
} DexEncodedCatchHandler;

typedef struct _DexEncodedCatchHandlerList {
  Metadata meta;
  uint32_t size;
  DexEncodedCatchHandler *list;
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
  DexTryItem *tries;
  DexEncodedCatchHandlerList handlers;
  uint32_t raw_code_offset;
} DexCodeItem;

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
  DexMapItem* list;
} DexMapList;

typedef struct _Dex {
  DexHeaderItem* header;
} Dex;

//Parse
int dexread(ByteStream* bs, uint8_t* buf, size_t size, uint32_t offset);

#define DXPARSE(_name,_type) _type* _name (ByteStream* bs, uint32_t offset)

DXPARSE(dx_header,DexHeaderItem);
DXPARSE(dx_stringid,DexStringIdItem);

//Build
int dexwrite(ByteStream* bs, uint8_t* buf, size_t size, uint32_t offset);

#define DXBUILD(_name,_type) void _name (ByteStream* bs, _type* obj)

DXBUILD(dxb_header,DexHeaderItem);
DXBUILD(dxb_stringid,DexStringIdItem);

//General
Dex* dxdex(ByteStream* bs, uint32_t offset);
void dxfree(Dex* dex);

#endif
