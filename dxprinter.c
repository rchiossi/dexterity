#include <stdio.h>
#include <string.h>
#include <stdint.h>

#include "dex.h"
#include "dxprinter.h"

// Aux print functions
void print_hex(uint8_t ptr[], unsigned int size) {
  int i;

  for (i=0; i<size; i++) {
    printf("%02x",ptr[i]);
  }
}

char* magic_name(Dex* dex) {
  if (!strncmp(dex->header->magic,"dex\n035",8))
    return "dex\\n035\\0";
  else if (!strncmp(dex->header->magic,"dey\n036",8))
    return "dey\\n036\\0";
  else
    return "UNKNOWN";
}

char* endian_tag(Dex* dex) {
  if (dex->header->endian_tag == 0x78563412)
    return "BIG_ENDIAN";
  else if (dex->header->endian_tag == 0x12345678)
    return "LITTLE_ENDIAN";
  else
    return "UNKNOWN_ENDIAN";
}

// Dex object print
void dxp_header(Dex* dex) {
  printf("DEX Header:\n");
  printf(" magic:           ");
  print_hex(dex->header->magic,8);
  printf(" (%s)\n",magic_name(dex));
  printf(" checksum:        %08x\n", dex->header->checksum);
  printf(" signature:       ");
  print_hex(dex->header->signature,20);
  printf("\n");
  printf(" file_size:       %d\n", dex->header->file_size);
  printf(" header_size:     %d\n", dex->header->header_size);
  printf(" endian_tag:      %08x (%s)\n", dex->header->endian_tag, endian_tag(dex));
  printf(" link_size:       %d\n", dex->header->link_size);
  printf(" link_off:        %08x\n", dex->header->link_off);
  printf(" map_off:         %08x\n", dex->header->map_off);
  printf(" string_ids_size: %d\n", dex->header->string_ids_size);
  printf(" string_ids_off:  %08x\n", dex->header->string_ids_off);
  printf(" type_ids_size:   %d\n", dex->header->type_ids_size);
  printf(" type_ids_off:    %08x\n", dex->header->type_ids_off);
  printf(" proto_ids_size:  %d\n", dex->header->proto_ids_size);
  printf(" proto_ids_off:   %08x\n", dex->header->proto_ids_off);
  printf(" field_ids_size:  %d\n", dex->header->field_ids_size);
  printf(" field_ids_off:   %08x\n", dex->header->field_ids_off);
  printf(" method_ids_size: %d\n", dex->header->method_ids_size);
  printf(" method_ids_off:  %08x\n", dex->header->method_ids_off);
  printf(" class_defs_size: %d\n", dex->header->class_defs_size);
  printf(" class_defs_off:  %08x\n", dex->header->class_defs_off);
  printf(" data_size:       %d\n", dex->header->data_size);
  printf(" data_off:        %08x\n", dex->header->data_off);
    
  if (dex->header->meta.corrupted != 0)
    printf("\nWarning: Header data is corrupted.\n");
}
