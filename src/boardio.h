/* 
  USAGE:
    The files boardio.h and boardio.c are for formating the board representation
    input and the output to the specifications of the professors program


  COPYRIGHT:
    Copyright 2024 Isaac McCracken - All rights reserved
  
*/

#ifndef BOARDIO_H
#define BOARDIO_H

#include <stdio.h>
#include "types.h"
#include "allocators.h"


BitBoard BitBoardFromFile(Arena *tempArena, const char* fileName);
void BitBoardFilePrint(FILE *fp, BitBoard board);
Coord CoordFromEnemyInput(void);
Coord CoordFromInput(char* coord);
Coord* multipleCoordsInput();
void CoordOutputMove(Coord coord);
void mainInput(BitBoard* board, char player);
void printBoardToConsole(BitBoard* board);
void bitToTextCoord(U64 bit, char* textCoord);

#endif
