#include "bytestream.h"
#include "dex.h"

#define CHECK(_OBJ,_RET) if ((_OBJ)->meta.corrupted) return (_RET)
#define ALLOC_LIST(_OBJ,_SIZE,_RET)		\
  do {						\
  (_OBJ) = malloc (sizeof(void*)*(_SIZE));	\
  if ((_OBJ) == NULL) return (_RET);		\
  } while(0)

//#define READ_LIST(_BS,_OFF,_SIZE,_LIST,_RFUNC)	\
//  do{							\
//  bsseek((_BS),(_OFF));				\
//  for (int i=0; i<(_SIZE),i++) {			\
//    (_LIST)[i] = _RFUNC((_BS),(_BS)->offset);		\
//  }							\
//} while(0)

Dex* dxdex_off(ByteStream* bs, uint32_t offset) {
  Dex* dex = (Dex*) malloc(sizeof(Dex));

  if (dex == NULL) return NULL;

  dex->header = dx_header(bs,offset+0);  
  CHECK(dex->header,dex);

  ALLOC_LIST(dex->string_ids,dex->header->string_ids_size,dex);
  //  READ_LIST(bs,dex->header->string_ids_off,
  //	    dex->header->string_ids_size,
  //	    dex->string_ids,dx_stringid)
  

  return dex;
}

void dxfree(Dex* dex) {
  free(dex->header);

  free(dex);
}
