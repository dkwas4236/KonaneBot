/*
  USAGE: 
    The files arena.h and arena.c are for memory management using 
    only arena allocators.
  
  RESOURCES:
    if you are completely new to the concept here are some resources
    https://youtu.be/3IAlJSIjvH0?si=-euCkZToGViOxQg7
    https://www.gingerbill.org/article/2019/02/08/memory-allocation-strategies-002/
  
  COPYRIGHT:
    Copyright 2024 Isaac McCracken - All rights reserved
*/


#ifndef ME_ARENA
#define ME_ARENA

#include "types.h"

#define Kilobyte(x) ((x)<<10)
#define Megabyte(x) ((x)<<20)
#define Gigabyte(x) (((U64)(x))<<30)

#define ARENA_DEFAULT_SIZE  Megabyte(1)


typedef struct Arena Arena;
struct Arena {
  char *buff;
  U64 pos;
  U64 cap;
  U64 align;
  Arena *next;
};

typedef struct TempArena TempArena;
struct TempArena {
  Arena *arena;
  U64 pos;
  U64 align;
};



Arena *ArenaInit(U64 capacity); // Initalize a Arena with a certain an inital buffer
void ArenaDeinit(Arena *arena); // Free all the buffers in a arena
void *ArenaPushNoZero(Arena *arena, U64 size); // Get a memory region with no zero initialization
void *ArenaPush(Arena *arena, U64 size); // get a memory region with zero initalization
void ArenaReset(Arena *arena); // set the 
static inline void ArenaSetAutoAlign(Arena *arena, U64 align) { // We have default alignment of 8
  arena->align = align;
}

TempArena TempArenaInit(Arena *backing_arena); // begin a temporary arena
void TempArenaDeinit(TempArena temp_arena); // end a temporary arena



#endif