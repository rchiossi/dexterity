#include <stdint.h>

#include "bytestream.h"

#include "leb128.h"

unsigned int l128size(ByteStream* bs) {
  int32_t offset = bs->offset;
  unsigned int len = 0;
  uint8_t cbyte = 0x0;

  bsread(bs,&cbyte,1);

  if (cbyte != 0x0 && !bs->exhausted) {
    len = 1;

    while (!bs->exhausted && len < 5 && (cbyte & 0x80) != 0x0) {
      len++;
      bsread(bs,&cbyte,1);
    }      
  }  
  
  bsseek(bs,offset);  

  return len;
}

int l128read(ByteStream* bs, uint32_t offset, leb128_t* leb)  {
  int ret;

  if (bs == NULL) return -1;

  bsseek(bs,offset);

  leb->size = l128size(bs);

  ret = bsread(bs,leb->data,leb->size);

  return 0;
}

unsigned int ul128toui(leb128_t uleb) {
  unsigned int val = 0;
  int i;
  
  for (i=uleb.size-1; i>=0; i--) {
    val = val * 128 + (uleb.data[i] & 0x7f);
  }  

  return val;
}

unsigned int ul128p1toui(leb128_t ulebp1) {  
  return ul128toui(ulebp1)-1;
}

unsigned int sl128toui(leb128_t sleb) {
  unsigned int val = 0;
  
  val = ul128toui(sleb);

  if (sleb.data[sleb.size-1] & 0x40 != 0) {
    val = val | ~((1 << (sleb.size*7))-1);
  }

  return val;
}
