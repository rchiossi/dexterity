#ifndef __LEB128__
#define __LEB128__

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

unsigned int ul128toui(leb128_t uleb);
int ul128p1toui(leb128_t ulebp1);
int sl128toui(leb128_t sleb);

#endif //__LEB128__
