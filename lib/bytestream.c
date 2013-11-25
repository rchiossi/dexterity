#include <stdint.h>

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "malloc_s.h"
#include "bytestream.h"

ByteStream* bsalloc(unsigned int size) {
  ByteStream* bs = (ByteStream*) malloc_s(sizeof(ByteStream));

  if (bs == NULL) {
    printf("ERROR: Cannot allocate bytestream structure.\n");
    exit(-1);
  }

  bs->filename = NULL;
  bs->size = size;

  bs->exhausted = 0;
  bs->offset = 0;
  
  bs->data = (uint8_t*) mmap(NULL, size, PROT_READ | PROT_WRITE, 
			     MAP_PRIVATE | MAP_ANONYMOUS, -1, 0x0);

  if (bs->data == MAP_FAILED) {
    free(bs);
    printf("ERROR: Unable to map memory.\n");
    exit(-1);
  }

  return bs;
}

ByteStream* bsmap(char* filename) {
  int fd;
  struct stat fdstat;
  ByteStream* bs;

  fd = open(filename,O_RDONLY);

  if (fd == -1) {
    printf("ERROR: File %s doesn't exists or cannot be read.\n",filename);
    exit(-1);
  }

  if (fstat(fd, &fdstat) == -1) {
    close(fd);
    printf("ERROR: Unable to get file size for %s.\n",filename);
    exit(-1);
  }

  bs = (ByteStream*) malloc_s(sizeof(ByteStream));

  if (bs == NULL) {
    close(fd);
    printf("ERROR: Cannot allocate bytestream structure.\n");
    exit(-1);
  }

  bs->filename = filename;
  bs->size = fdstat.st_size;

  bs->exhausted = 0;
  bs->offset = 0;
  
  bs->data = (uint8_t*) mmap(NULL, bs->size, PROT_READ | PROT_WRITE,
			     MAP_PRIVATE,fd,0x0);

  if (bs->data == MAP_FAILED) {
    free(bs);
    close(fd);
    printf("ERROR: Unable to map file.\n");
    exit(-1);
  }

  close(fd); 

  return bs;
}

int bsfree(ByteStream* bs) {
  int ret = 0;

  if (bs->data != NULL) {
    ret = munmap(bs->data,bs->size);
  }

  free(bs);

  return ret;
}

void bsseek(ByteStream* bs, uint32_t offset) {
  bs->offset = offset;
  bs->exhausted = 0;
}

void bsreset(ByteStream* bs) {
  bs->offset = 0x0;
  bs->exhausted = 0; 
}

unsigned int bsread(ByteStream* bs, uint8_t* data, size_t size) {
  unsigned int* in;
  unsigned int* out;
  
  unsigned int asize;

  unsigned int isize;
  unsigned int ioffset;

  unsigned int i;

  if (bs->exhausted) return 0;

  if (bs->offset >= bs->size) return 0;

  if (bs->offset+size > bs->size) {
    size = bs->size - bs->offset;
    bs->exhausted = 1;
  }

  //align read
  asize = sizeof(unsigned int) - (bs->offset % sizeof(unsigned int)); 

  for (i=0; i<asize && i<size; i++) {
    data[i] = bs->data[bs->offset+i];
  }

  bs->offset += i;
  data += i;
  size -= i;
  asize = i;

  //optimized read
  isize = size / sizeof(unsigned int);
  ioffset = bs->offset / sizeof(unsigned int);

  in = (uint32_t*) bs->data;
  out = (uint32_t*) data;

  for (i=0; i<isize; i++) {
    out[i] = in[ioffset+i];
  }

  bs->offset += i * sizeof(unsigned int);
  data += i * sizeof(unsigned int);
  
  //trailing bytes read
  size -= isize * sizeof(unsigned int);

  for (i=0; i<size; i++) {
    data[i] = bs->data[bs->offset+i];
  }

  bs->offset += i;

  return isize * sizeof(unsigned int) + size + asize;
}

unsigned int bsread_offset(ByteStream* bs, uint8_t* buf, size_t size, uint32_t offset) {

  if (offset >= bs->size) {
    bs->exhausted = 1;
    return 0;
  }

  bs->offset = offset;
  bs->exhausted = 0;

  return bsread(bs,buf,size);
}

int bswrite(ByteStream* bs, uint8_t* data, unsigned int size) {
  unsigned int* in;
  unsigned int* out;
  
  unsigned int asize;

  unsigned int isize;
  unsigned int ioffset;

  unsigned int i;

  if (bs->exhausted) return 0;

  if (bs->offset >= bs->size) return 0;

  if (bs->offset+size > bs->size) {
    size = bs->size - bs->offset;
    bs->exhausted = 1;
  }

  //align write
  asize = sizeof(unsigned int) - (bs->offset % sizeof(unsigned int)); 

  for (i=0; i<asize && i<size; i++) {
    bs->data[bs->offset+i] = data[i];
  }

  bs->offset += i;
  data += i;
  size -= i;
  asize = i;

  //optimized write
  isize = size / sizeof(unsigned int);
  ioffset = bs->offset / sizeof(unsigned int);

  in = (uint32_t*) data;
  out = (uint32_t*) bs->data;

  for (i=0; i<isize; i++) {
    out[ioffset+i] = in[i];
  }

  bs->offset += i * sizeof(unsigned int);
  data += i * sizeof(unsigned int);

  //write finishing bytes
  size -= i * sizeof(unsigned int);

  for (i=0; i<size; i++) {
    bs->data[bs->offset+i] = data[i];
  }

  bs->offset += i;

  return isize * sizeof(unsigned int) + size + asize;
}

unsigned int bswrite_offset(ByteStream* bs, uint8_t* buf, size_t size, uint32_t offset) {

  if (offset >= bs->size) {
    bs->exhausted = 1;
    return 0;
  }

  bs->offset = offset;
  bs->exhausted = 0;

  return bswrite(bs,buf,size);
}

int bssave(ByteStream* bs, char* filename) {
  FILE* f = fopen(filename,"w");

  if (bs == NULL || f == NULL) return -1;

  fwrite(bs->data, 1, bs->size, f);

  fflush(f);
  fclose(f);    
  return 0;
}

