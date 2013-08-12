#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "bytestream.h"
#include "dex.h"
#include "dxprinter.h"

int run_option(char option, char* filter, ByteStream* bs) {
  Dex* dex = dxdex(bs);

  if (dex == NULL) {
    printf("Error: Unable to allocate Dex structure.\n");
    exit(-1);
  }
       
  switch (option) {
  case 'H':
    dxp_header(dex);
  }

  dxfree(dex);

}

void usage(char* name) {
  printf("Usage: %s [option] target\n",name);
  exit(0);
}

int main(int argc, char* argv[]) {  
  char* target;
  char option;
  char* filter;
  
  ByteStream* bs;

  if (argc == 3) {
    target = argv[2];
    filter = NULL;
  } 
  else if (argc == 4) {
    target = argv[3];
    filter = argv[2];
  }
  else usage(argv[0]);
  
  if (argv[1][0] == '-') {
    if (strlen(argv[1]) < 2)
      usage(argv[0]);    
    
    option = argv[1][1];
  } 
  else {
    option = argv[1][0];
  }

  bs = bsmap(target);

  if (bs == NULL) {
    printf("Error: Unable to map target (%s)\n",target);
    exit(-1);
  }

  run_option(option,filter,bs);

  bsfree(bs);

  return 0;
}
