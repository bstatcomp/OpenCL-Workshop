#include <stdio.h>
#include <stdlib.h>

#define MAX_SOURCE_SIZE 16384

void read_kernel(char** source_str, char* filename){
  FILE *fp;
  size_t source_size;

  fp = fopen(filename, "rb");
  
  if (!fp) {
    fprintf(stderr, ":-(#\n");
    exit(1);
  }

  *source_str = (char*)malloc(MAX_SOURCE_SIZE);
  source_size = fread(*source_str, 1, MAX_SOURCE_SIZE, fp);
  (*source_str)[source_size] = '\0';
  fclose( fp );
}
