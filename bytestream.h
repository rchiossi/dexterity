#ifndef __BYTESTREAM__
#define __BYTESTREAM__

#include <stdint.h>
#include <unistd.h>

typedef struct _ByteStream {
  char* filename;
  uint32_t size;

  uint8_t* data;
  uint32_t offset;

  int exhausted;
} ByteStream;

ByteStream* bsalloc(char* filename);
int bsfree(ByteStream* bstream);

int bsread(ByteStream* bs, uint8_t* buf, size_t size);
int bsread_offset(ByteStream* bs, uint8_t* buf, size_t size,
		  uint32_t offset);

void bsseek(ByteStream* bs, uint32_t offset);
void bsreset(ByteStream* bs);

#endif
