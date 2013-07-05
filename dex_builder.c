
#include "bytestream.h"
#include "dex.h"

#define DXBUILD(_name,_type) void _name (ByteStream* bs, _type* obj)

#define DXB_FIXED(_name,_type)			\
DXBUILD(_name,_type) {				\
  uint32_t offset = bs->offset;				\
							\
  if (bs == NULL) return;				\
							\
  dxwrite(bs,(uint8_t*)obj,sizeof(_type),offset);	\
}							\

//write aux
void dxwrite(ByteStream* bs, uint8_t* buf, size_t size, uint32_t offset) {
  Metadata* meta = (Metadata*) buf;
  uint8_t* ptr = buf += sizeof(Metadata);
  size_t data_size = size - sizeof(Metadata);

  bswrite_offset(bs,ptr,data_size,offset);
}

//Build Functions
DXB_FIXED(dxb_header,DexHeaderItem)
DXB_FIXED(dxb_stringid,DexStringIdItem)
