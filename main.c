/* Copyright 2024 Isaac McCracken - All rights reserved
*/

#include <stdio.h>
#include <time.h>

#define THINKING_TIME 10
#define BOARD_WIDTH 8

# include "types.h"


typedef union Board Board;
union Board {
  U64 whole;
  U8 rows[8];
};

enum {
  Turn_black,
  Turn_white,
}; 


enum {
  Agent_player,
  Agent_ai,
};

enum {
  Board_empty,
  Board_black,
  Board_white,
};

/*
  Breadth first search
    Implementation
    - fringe FIFO - new successors go at end
    - when we go to new node we add all 

*/

typedef struct Coord Coord;
struct Coord {
  U8 x;
  U8 y;
};

Coord IndexToCoord(U8 index) {
  return (Coord){.x = index % 8, .y = index / 8};
}

U8 CoordToIndex(U8 index) {

}


typedef struct StateNode StateNode;
struct StateNode {
  U64 white; // bit will be one if there is a square there
  U64 black; // bit will be one if there is a square there
  StateNode *parent;
  U64 childCount;

};




/**
 * @brief This is a debug print for printing the bit board
 * 
 * @param board 
 */
void PrintBitBoard(Board board) {
  for (U8 i = 0; i < 8; i++) {
    for (U8 j = 0; j < 8; j++) {
      char c = (board.rows[i] & (1<<j)) > 0;
      putchar('0' + c); // prints zero or 1
      putchar(' ');
    }
    putchar('\n');
    
  }
  
}



void GenerateBitMoves(Board *bitMoves) {
  U8 boardArray[8][8] = {0};
  for (U8 i = 0; i < 64; i++) {
    U8 x = i % 8;
    U8 y = i / 8;

  }
}


typedef struct AgentMove AgentMove;
struct AgentMove {
  Coord start;
  Coord end;
};

void PrintBoard(int *board, int width) {
  for (int i = 0; i < width * width; i++) {

    if (i % width == 0) {
      putchar('A' + (i / width));
      putchar(' ');
    } 


    char c = 'O';
    if (board[i] == Board_black) c = 'B';
    if (board[i] == Board_white) c = 'W';

    putchar(c);
    putchar(' ');

    if ((i + 1) % width == 0) putchar('\n');
  }

  printf("  ");
  for (int i = 0; i < width; i++) {putchar('1' + i); putchar(' ');}
  
  
  
}



int main(void) {
  int player_types[2] = {Agent_player, Agent_player};
  int board[BOARD_WIDTH * BOARD_WIDTH] = { 0 };


  Board bitMoves[64];
  GenerateBitMoves(bitMoves);

  Board whiteBoard = (Board){.whole = 0xaa55aa55aa55aa55};
  Board blackBoard = (Board){.whole = 0x55aa55aa55aa55aa};
  PrintBitBoard(whiteBoard);
  putchar('\n');

  

  return 0;
}