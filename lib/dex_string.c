#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "dex.h"
#include "leb128.h"

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

//TODO convert everything to MUTF8

uint32_t dx_string_find_index(Dex* dx, char* str) {
  unsigned int i;
  int ret;

  if (dx == NULL || str == NULL) return -1;

  for (i=0; i<dx->header->string_ids_size; i++) {
    ret = strncmp(str, (char *)dx->string_data_list[i]->data, strlen((char *)dx->string_data_list[i]->data));

    if (ret == 0) // string already exists
      return -1;
    else if (ret < 0)
      return i;
  }

  return dx->header->string_ids_size;
}

void dx_string_add_stringid(Dex* dx, uint32_t index) {
  DexStringIdItem* sid;
  int32_t delta;
  unsigned int i;

  if (dx == NULL || index >= dx->header->string_ids_size) return; 

  sid = (DexStringIdItem*) malloc_s(sizeof(DexStringIdItem));

  sid->meta.corrupted = false;

  delta = sizeof(DexStringIdItem) - sizeof(Metadata);

  if (index == dx->header->string_ids_size) {
    sid->meta.offset = dx->string_ids[index-1]->meta.offset;
    sid->meta.offset += sizeof(DexStringIdItem) - sizeof(Metadata);
  } else {
    sid->meta.offset = dx->string_ids[index]->meta.offset;
  }

  //Fix references
  dx_shift_offset(dx,sid->meta.offset,delta);
  dx_shift_stringid(dx,index,1);

  if (index == dx->header->string_ids_size) {
    sid->string_data_off  = dx->string_ids[index-1]->string_data_off;
    sid->string_data_off += dx->string_data_list[index-1]->size.size;
    sid->string_data_off += ul128toui(dx->string_data_list[index-1]->size) + 1;
  } else {
    sid->string_data_off = dx->string_ids[index]->string_data_off;
  }  

  //Add new stringid to the string_ids list
  if (dx->header->string_ids_size >= dx->meta.string_ids_alloc)
    DX_REALLOC_LIST(DexStringIdItem*,dx->string_ids,dx->meta.string_ids_alloc);
  
  for (i=dx->header->string_ids_size; i>index; i--)
    dx->string_ids[i] = dx->string_ids[i-1];

  dx->header->string_ids_size++;  

  dx->string_ids[index] = sid;

  //Fix Maps
  for (i=0; i<dx->map_list->size; i++) {
    if (dx->map_list->list[i]->type == 0x0001) {
      dx->map_list->list[i]->size++;
    }    
  }

  //Fix header
  if (index == 0) 
    dx->header->string_ids_off = sid->meta.offset;

  //Fix filesize
  dx->header->file_size += delta;
}

void dx_string_add_stringdata(Dex* dx, uint32_t index, char* str) {
  DexStringDataItem* sdata;
  int32_t delta;
  unsigned int i;

  if (dx == NULL || index >= dx->header->string_ids_size) return; 

  sdata = (DexStringDataItem*) malloc_s(sizeof(DexStringDataItem));

  sdata->meta.corrupted = false;
  sdata->meta.offset = dx->string_ids[index]->string_data_off;

  uitoul128(&(sdata->size),strlen(str));
  DX_ALLOC_LIST(uint8_t,sdata->data,ul128toui(sdata->size)*3+1);

  strncpy((char *)sdata->data,str,strlen(str));
  sdata->data[strlen(str)] = 0x0;

  //Fix references
  delta = sdata->size.size + ul128toui(sdata->size) + 1;
  dx_shift_offset(dx,sdata->meta.offset,delta);

  //Restore offset
  dx->string_ids[index]->string_data_off = sdata->meta.offset;

  //Add new stringdata to the string_data list
  if (dx->meta.string_data_list_size >= dx->meta.string_data_list_alloc)
    DX_REALLOC_LIST(DexStringDataItem*,dx->string_data_list,dx->meta.string_data_list_alloc);
  
  for (i=dx->meta.string_data_list_size; i>index; i--)
    dx->string_data_list[i] = dx->string_data_list[i-1];

  dx->meta.string_data_list_size++;  

  dx->string_data_list[index] = sdata;

  //Fix Maps
  for (i=0; i<dx->map_list->size; i++) {
    if (dx->map_list->list[i]->type == 0x2002) {
      dx->map_list->list[i]->size++;
    }    
  }

  //Fix header
  if (index == 0) 
    dx->header->data_size += delta;

  //Fix filesize
  dx->header->file_size += delta;  

  //Fix datasize
  dx->header->data_size += delta;  
}

void dx_string_add(Dex* dx, char* str) {
  uint32_t index;

  if (dx == NULL || str == NULL) return;

  index = dx_string_find_index(dx,str);

  dx_string_add_stringid(dx,index);
  dx_string_add_stringdata(dx,index,str);
}
