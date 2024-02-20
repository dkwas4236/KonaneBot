#include <stdint.h>
#include <stdio.h>
#include "boardio.h"

// function will help get bitboard of input file
uint64_t pow(int x, int y)
{
  uint64_t result = 1;
  int power = y;
  if (power == 0) return 1;
  while (power > 0) {
    result *= x;
    power--;
  }
}

// get bitboard of input file. make sure index
// is only either 0 (black pieces) or 1 (white pieces). 
uint64_t* getBitBoard(char* fileName) {
  // gonna use malloc here, not sure how to use arena allocator
  uint64_t* bitBoards = (uint64_t*) malloc(sizeof(uint64_t)*2);

  FILE* readFile = fopen(fileName, "r");
  uint64_t b = 0, w = 0;
  int currBit = 0;

  char c;
  while ((c = fgetc(readFile)) != EOF) {
    if (c == '\n') continue;
    else if (c == 'B') b += pow(2, currBit);
    else if (c == 'W') w += pow(2, currBit);
    currBit++;
  }

  fclose(readFile);

  bitBoards[0] = b;
  bitBoards[1] = w;

  return bitBoards;
}
