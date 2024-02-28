/*
  USAGE: 
    The files agent.h and agent.c are for implementing the agent itself
  
  COPYRIGHT:
    Copyright 2024 Isaac McCracken - All rights reserved
*/
#ifndef AGENT_H
#define AGENT_H

#include "types.h"


U64 getPlayerEmptySpace(char player, BitBoard board);
U64* getMovablePieces(U64 jump, BitBoard board, char player); 


#endif
