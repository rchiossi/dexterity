#include <stdio.h>

#include "dex.h"

int main(void) {
  ByteStream* in = bsmap("./tests/classes.dex");
  ByteStream* out = bsalloc(in->size);
  Dex* dex = dxdex(in,0);

  dxb_header(out,dex->header);

  bssave(out,"./tests/out");

  bsfree(in);
  bsfree(out);
}
