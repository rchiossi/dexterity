#ifndef __BYTESTREAM__
#define __BYTESTREAM__

#include <stdint.h>
#include <unistd.h>

#define BS_RO 0
#define BS_RW 1

// MAP_ANONYMOUS is MAP_ANON on OSX, so this will let us compile
#ifndef MAP_ANONYMOUS
#define MAP_ANONYMOUS MAP_ANON
#endif

typedef struct _ByteStream {
  char* filename;
  size_t size;

  uint8_t* data;
  uint32_t offset;

  int exhausted;
} ByteStream;

ByteStream* bsalloc(unsigned int size);
ByteStream* bsmap(char* filename);
int bsfree(ByteStream* bstream);

void bsseek(ByteStream* bs, uint32_t offset);
void bsreset(ByteStream* bs);

unsigned int bsread(ByteStream* bs, uint8_t* buf, size_t size);
unsigned int bsread_offset(ByteStream* bs, uint8_t* buf, size_t size,
			   uint32_t offset);

int bswrite(ByteStream* bs, uint8_t* data, unsigned int size);
unsigned int bswrite_offset(ByteStream* bs, uint8_t* buf, size_t size, 
			    uint32_t offset);

int bssave(ByteStream* bs, char* filename);

#endif
