#include <stdint.h>

#include "bytestream.h"
#include "leb128.h"

unsigned int l128size(ByteStream* bs) {
  int32_t offset;
  unsigned int len = 0;
  uint8_t cbyte = 0x0;

  if (bs == NULL) return 0;

  offset = bs->offset;

  bsread(bs,&cbyte,1);

  if (!bs->exhausted) {
    len = 1;

    while (!bs->exhausted && len < 5 && (cbyte & 0x80) != 0x0) {
      len++;
      bsread(bs,&cbyte,1);
    }      
  }  
  
  bsseek(bs,offset);  

  return len;
}

int l128read(ByteStream* bs, leb128_t* leb) {
  unsigned int ret;

  if (bs == NULL || leb == NULL) return -1;

  leb->size = l128size(bs);

  ret = bsread(bs,leb->data,leb->size);

  return ret != leb->size;
}

int l128read_offset(ByteStream* bs, leb128_t* leb, uint32_t offset) {
  if (bs == NULL || leb == NULL) return -1;

  bsseek(bs,offset);

  return l128read(bs,leb);
}

int l128write(ByteStream* bs, leb128_t* leb) {
  if (bs == NULL || leb == NULL) return 0;

  return bswrite(bs,leb->data,leb->size);
}

int l128write_offset(ByteStream* bs, leb128_t* leb, uint32_t offset) {
  if (bs == NULL || leb == NULL) return 0;

  bsseek(bs,offset);
  
  return l128write(bs,leb);
}

uint32_t ul128toui(leb128_t uleb) {
  uint32_t val = 0;
  int i;
  
  for (i=uleb.size-1; i>=0; i--) {
    val = val * 128 + (uleb.data[i] & 0x7f);
  }  

  return val;
}

int32_t ul128p1toi(leb128_t ulebp1) {  
  return ul128toui(ulebp1)-1;
}

int32_t sl128toi(leb128_t sleb) {
  int32_t val = 0;
  
  val = ul128toui(sleb);

  if ((sleb.size != 5) && (sleb.data[sleb.size-1] & 0x40) != 0) {
    val = val | ~((1 << (sleb.size*7))-1);
  }

  return val;
}

void uitoul128(leb128_t* leb, uint32_t num) {
  unsigned int i;

  for (i=0; i<5; i++) {
    if (i == 4)
      leb->data[i] = num & 0x4f;
    else
      leb->data[i] = num & 0x7f;

    num /= 128;
    
    if (num == 0) break;

    leb->data[i] |= 0x80;
  }

  leb->size = i+1;
}

void itoul128p1(leb128_t* leb, int32_t num) {
  uitoul128(leb,(uint32_t) num+1);
}

void itosl128(leb128_t* leb, int32_t num) {
  unsigned int i;

  leb->data[0] = ((num >> 00) & 0x7f) | 0x80;
  leb->data[1] = ((num >> 07) & 0x7f) | 0x80;
  leb->data[2] = ((num >> 14) & 0x7f) | 0x80;
  leb->data[3] = ((num >> 21) & 0x7f) | 0x80;
  leb->data[4] = ((num >> 28) & 0x4f) | 0x00;

  leb->size = 5;

  if (leb->data[4] == 0x00 || leb->data[4] == 0x4f) {
    leb->size--;

    for (i=3; i>0; i--) {
      if (leb->data[i] == 0x80 || leb->data[i] == 0xff)
	leb->size--;
      else 
	break;
    }
  }

  if (num > 0 && (leb->data[leb->size-1] & 0x40) != 0x0) {
    leb->size++;
  } 

  if (num < 0 && (leb->data[leb->size-1] & 0x40) == 0x0) {
    leb->size++;
  } 

  leb->data[leb->size-1] &= 0x7f;
}
