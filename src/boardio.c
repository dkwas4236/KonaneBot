#include "boardio.h"

#include <stdio.h>

// Read the whole file info a buffer located on the arena
U8 *LoadFileDataArena(Arena *arena, const char *filepath, U32 *bytes_read) {
  FILE *fp = fopen(filepath, "rb");
  
  if (fp) {
    fseek(fp, 0, SEEK_END);
    *bytes_read = ftell(fp);
    rewind(fp);

    U8 *buffer = ArenaPushNoZero(arena, *bytes_read);
    fread(buffer, *bytes_read, 1, fp);

    fclose(fp);

    return buffer;

  } else fprintf(stderr, "couldn't open file \"%s\"\n", filepath);

  return NULL;
}



// get bitboard of input file. make sure index
// is only either 0 (black pieces) or 1 (white pieces). 
BitBoard BitBoardFromFile(Arena *tempArena, const char* fileName) {
  BitBoard board = { 0 };

  // since this data is only temporaly needed
  TempArena temp = TempArenaInit(tempArena);

  U32 bytesRead = 0;
  U8 *buffer = LoadFileDataArena(tempArena, fileName, &bytesRead);

  U32 row = 0;
  U32 col = 0;
  for (U32 index = 0; index < bytesRead; index++) {
    if (buffer[index] == '\n') {
      col = 0;
      row += 1;
      continue;
    } 

    if (buffer[index] != 'O') board.rows[row] |= 1llu<<col;

    
    col += 1;
  }
  

  TempArenaDeinit(temp);
  // After we are done allocations 
  // we can "free" all the memory 
  // that we used during our calculations

  return board;
}
