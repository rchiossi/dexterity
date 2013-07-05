#include <stdio.h>
#include <string.h>

#include "bytestream.h"

int main(void) {
  ByteStream* bs = bsalloc(20); //bsmap("./tests/classes.dex");
  char buf[2048];
  int val;

  memset(buf,0x0,2048);
  
  printf("\nWRITE:\n");

  val = bswrite(bs,"AAA",3);
  
  printf("%s\n",(char*)bs->data);
  printf("ex: %d\n",bs->exhausted);
  printf("v = %d\n",val);

  val = bswrite(bs,"ABBBBCCCC",9);

  printf("%s\n",(char*)bs->data);
  printf("ex: %d\n",bs->exhausted);
  printf("v = %d\n",val);

  val = bswrite(bs,"DDDDEEEEFFFFGGGG",11);

  printf("%s\n",(char*)bs->data);
  printf("ex: %d\n",bs->exhausted);
  printf("v = %d\n",val);

  val = bswrite(bs,"IIIIIIII",8);

  printf("%s\n",(char*)bs->data);
  printf("ex: %d\n",bs->exhausted);
  printf("v = %d\n",val);

  bsreset(bs);

  printf("\nREAD:\n");

  val = bsread(bs,buf,3);
  printf("%s\n",(char*)buf);
  printf("ex: %d\n",bs->exhausted);
  printf("v = %d\n",val);

  val = bsread(bs,buf,8);
  printf("%s\n",(char*)buf);
  printf("ex: %d\n",bs->exhausted);
  printf("v = %d\n",val);
 
  val = bsread(bs,buf,15);
  printf("%s\n",(char*)buf);
  printf("ex: %d\n",bs->exhausted);
  printf("v = %d\n",val);


  bsfree(bs);

  return 0;
}
