#include <stdint.h>
#include <malloc.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "bytestream.h"

ByteStream* bsalloc(char* filename) {
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

  bs = (ByteStream*) malloc(sizeof(ByteStream));

  if (bs == NULL) {
    close(fd);
    printf("ERROR: Cannot allocate bytestream structure.\n");
    exit(-1);
  }

  bs->filename = filename;
  bs->size = fdstat.st_size;

  bs->exhausted = 0;
  bs->offset = 0;
  
  bs->data = (uint8_t*) mmap(NULL,bs->size,PROT_READ,MAP_PRIVATE,fd,0x0);

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

int bsread(ByteStream* bs, uint8_t* buf, size_t size) {
  unsigned int* in = (uint32_t*) bs->data;
  unsigned int* out = (uint32_t*) buf;
  
  unsigned int isize;
  unsigned int ioffset;

  unsigned int i,j;

  if (bs->exhausted) return 0;

  if (bs->offset >= bs->size) return 0;

  if (bs->offset+size > bs->size) {
    size = bs->size - bs->offset;
    bs->exhausted = 1;
  }

  isize = size / sizeof(unsigned int);
  ioffset = bs->offset / sizeof(unsigned int);
  for (i=0; i<isize; i++) {
    out[i] = in[ioffset+i];
  }

  bs->offset += i * sizeof(unsigned int);
  
  size -= isize*sizeof(unsigned int);

  for (j=0; j<size; j++) {
    buf[j] = bs->data[bs->offset+i];
  }

  bs->offset += j;

  return size;
}

int bsread_offset(ByteStream* bs, uint8_t* buf, size_t size, uint32_t offset) {

  if (offset >= bs->size) {
    bs->exhausted = 1;
    return 0;
  }

  bs->offset = offset;

  return bsread(bs,buf,size);
}

int bsread_struct(ByteStream* bs, uint8_t* buf, size_t size, uint32_t offset) {
  unsigned int* corrupted = (unsigned int*) buf;
  uint8_t* ptr = buf += sizeof(unsigned int);
  size_t data_size = size - sizeof(unsigned int);
  int ret =  bsread_offset(bs,ptr,data_size,offset);

  *corrupted = (ret == data_size);

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
