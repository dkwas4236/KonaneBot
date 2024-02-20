#include <stdint.h>
#include <stdio.h>
#include "boardio.h"

// get bitboard of input file. make sure index
// is only either 0 (black pieces) or 1 (white pieces). 
uint64_t* getBitBoard(char* fileName) {
  // gonna use malloc here, not sure how to use arena allocator
  uint64_t* bitBoards = (uint64_t*) malloc(sizeof(uint64_t)*2);

  FILE* readFile = fopen(fileName, "r");
  uint64_t b = 0, w = 0;

  char c;
  while ((c = fgetc(readFile)) != EOF) {
    if (c == '\n') continue;

    b <<= 1;
    w <<= 1;

    if (c == 'B') b++;
    else if (c == 'W') w++;
  }

  fclose(readFile);

  bitBoards[0] = b;
  bitBoards[1] = w;

  return bitBoards;
}
