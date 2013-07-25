#ifndef __LEB128__
#define __LEB128__

#include "bytestream.h"

typedef struct _leb128 {
  uint8_t data[5];
  unsigned int size;
} leb128_t;


unsigned int l128size(ByteStream* bs);

int l128read(ByteStream* bs, uint32_t offset, leb128_t* leb);

unsigned int ul128toui(leb128_t uleb);
unsigned int ul128p1toui(leb128_t ulebp1);
unsigned int sl128toui(leb128_t sleb);

#endif //__LEB128__
