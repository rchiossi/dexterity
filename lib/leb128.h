#ifndef __LEB128__
#define __LEB128__

#include <stdint.h>
#include "bytestream.h"

typedef struct _leb128 {
  uint8_t data[5];
  unsigned int size;
} leb128_t;

unsigned int l128size(ByteStream* bs);

int l128read(ByteStream* bs, leb128_t* leb);
int l128read_offset(ByteStream* bs, leb128_t* leb, uint32_t offset);
int l128write(ByteStream* bs, leb128_t* leb);
int l128write_offset(ByteStream* bs, leb128_t* leb, uint32_t offset);

uint32_t ul128toui(leb128_t uleb);
int32_t ul128p1toi(leb128_t ulebp1);
int32_t sl128toi(leb128_t sleb);

void uitoul128(leb128_t* uleb, uint32_t num);
void itoul128p1(leb128_t* leb, int32_t num);
void itosl128(leb128_t* leb, int32_t num);

#endif //__LEB128__
