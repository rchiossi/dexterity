#ifndef __BYTESTREAM__
#define __BYTESTREAM__

#include <stdint.h>
#include <unistd.h>

#define BS_RO 0
#define BS_RW 1

typedef struct _ByteStream {
  unsigned int mode;

  char* filename;
  uint32_t size;

  uint8_t* data;
  uint32_t offset;

  int exhausted;
} ByteStream;

ByteStream* bsalloc(unsigned int size);
ByteStream* bsmap(char* filename);
int bsfree(ByteStream* bstream);

unsigned int bsread(ByteStream* bs, uint8_t* buf, size_t size);
unsigned int bsread_offset(ByteStream* bs, uint8_t* buf, size_t size,
			   uint32_t offset);

void bsseek(ByteStream* bs, uint32_t offset);
void bsreset(ByteStream* bs);

#endif
