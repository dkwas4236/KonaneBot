#ifndef TYPES_H
#define TYPES_H

typedef unsigned char       U8;
typedef unsigned short      U16;
typedef unsigned int        U32;
typedef unsigned long long  U64;

typedef signed char       I8;
typedef signed char       I8;
typedef signed short      I16;
typedef signed int        I32;
typedef signed long long  I64;

#define ROOT2 1.41421356237f // for monte carlo tree search

#define MyAssert(expr) if (!expr) *((U32*)0) = 0xDEAD

enum {
  Direction_right,
  Direction_left,
  Direction_up,
  Direction_down,
};


typedef union BitBoard BitBoard;
union BitBoard {
  U8 rows[8];
  U64 whole;
};



// 64 byte per node maybe we can cut down on size
typedef struct StateNode StateNode;
struct StateNode {
  BitBoard   board;
  StateNode *next;
  StateNode *prev;
  StateNode *firstChild;
  StateNode *lastChild;
  U64 childCount;
};



typedef struct Coord Coord;
struct Coord {
  U8 x;
  U8 y;
};


static inline Coord IndexToCoord(U8 index) {
  return (Coord){.x = index % 8, .y = index / 8};
}

static inline U8 CoordToIndex(Coord coord) {
  return coord.x + coord.y * 8;
}



#endif