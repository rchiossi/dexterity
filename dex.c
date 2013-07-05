#include "bytestream.h"
#include "dex.h"

Dex* dxdex(ByteStream* bs, uint32_t offset) {
  Dex* dex = (Dex*) malloc(sizeof(Dex));

  if (dex == NULL) return NULL;

  dex->header = dx_header(bs,offset+0);  

  return dex;
}

void dxfree(Dex* dex) {
  free(dex->header);

  free(dex);
}
