/*
  USAGE: 
    The files agent.h and agent.c are for implementing the agent itself
  
  COPYRIGHT:
    Copyright 2024 Isaac McCracken - All rights reserved
*/
#ifndef AGENT_H
#define AGENT_H

#include "types.h"
#include "allocators.h"

U64 getPlayerEmptySpace(BitBoard board, char player);
U64* getMovablePieces(U64 jump, BitBoard board, char player); 
StateNode* StateNodeGenerateChildren(StateNodePool *pool, StateNode *parent, char playerKind);
U64 StateNodeCountChildren(StateNode *node);
void StateNodePushChild(StateNode *parent, StateNode *child);

#endif
