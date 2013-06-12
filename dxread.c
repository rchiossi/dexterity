#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void usage(char* name) {
  printf("Usage: %s [option] target\n",name);
  exit(0);
}

int main(int argc, char* argv[]) {  
  char* target;
  char option;
  
  if (argc == 3) {
    target = argv[2];
  } 
  else if (argc == 4) {
    target = argv[3];
  }
  else usage(argv[0]);
  
  if (argv[1][0] == '-') {
    if (strlen(argv[1] < 2))
      usage(argv[0]);    
    
    option = argv[1][1];
  } 
  else {
    option = argv[1][0];
  }

  //TODO: Open file and parse Dex
       
  switch (option) {
  case 'H':
    p_header(target);      
  }
  

  target = argv[2];  
}
