#include <stdint.h>

#include "bytestream.h"
#include "dex.h"

void dxparse_header(DexHeaderItem* dh, ByteStream* bs, uint32_t offset) {
  bsread_struct(bs,dh,sizeof(DexHeaderItem),offset);
}


