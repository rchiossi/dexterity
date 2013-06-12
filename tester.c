#include "bytestream.h"

int main(void) {
  ByteStream* bs;

  bs = bsalloc("tests/classes.dex");

  bsfree(bs);
}
