/* 
  USAGE:
    The files boardio.h and boardio.c are for formating the board representation
    input and the output to the specifications of the professors program


  COPYRIGHT:
    Copyright 2024 Isaac McCracken - All rights reserved
  
*/

#include "types.h"
#include "arena.h"

BitBoard BitBoardFromFile(Arena *tempArena, const char* fileName);