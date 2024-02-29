/* Copyright 2024 Isaac McCracken - All rights reserved
 ......................................................
 ......................................................

 ......................................................
 ......................................................

 ......................................................
 ......................................................
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#define THINKING_TIME 10
#define BOARD_WIDTH 8


#include "types.h"
#define BITMOVES_IMPLEMENTATION
#include "bitmoves.h"
#include "boardio.h"
#include "allocators.h"
#include "agent.h"

#include <string.h>

/**
 * @brief This is a debug print for printing the bit board
 * 
 * @param board 
 */
void PrintBitBoard(BitBoard board) {
  for (U8 i = 0; i < 8; i++) {
    for (U8 j = 0; j < 8; j++) {
      char c = (board.rows[i] & (1<<j)) > 0;
      putchar('0' + c); // prints zero or 1
      putchar(' ');
    }
    putchar('\n');
  }
}

void PrintBoard(BitBoard board) {
  for (U8 i = 0; i < 8; i++) {
    for (U8 j = 0; j < 8; j++) {
      U8 color = (i + j) % 2 ;
      char c = (board.rows[i] & (1<<j)) > 0;
      if (c) c = (color)? 'W': 'B';
      else c = 'O';
      putchar(c);
    }
    putchar('\n');
  }
}

// void StateNodePushChild(StateNode *parent, StateNode *child) {
//   if (parent->lastChild) {
//     MyAssert(parent->firstChild);
//     // link parent to new child
//     StateNode *oldLast = parent->lastChild;
//     parent->lastChild = child;
//     // link children to each other
//     child->prev = oldLast;
//     oldLast->next = child;
//     // increase child count (child count could be useless)
//     parent->childCount++;
//   } else {
//     parent->firstChild = child;
//     parent->lastChild = child;
//     parent->childCount = 1;
//   }
// }

void GenerateAllMoves(Arena *arena, StateNode *parent, U64 turn) {
  const BitBoard board = parent->board;
  if (turn == 0) {
    MyAssert(board.whole == allPieces);
    StateNode *node = ArenaPush(arena, sizeof(StateNode));
    node->board = board;
    node->board.whole &= ~1llu;
    StateNodePushChild(parent, node);
    return;
  }

  // for each of our pieces
  for (U8 index = 0; index < 32; index++) {
    Coord coord = CoordFromIndex(index);
    U64 pieceMask = 1llu << index;
    U64 moveMask = 0llu;
    U64 killMask = 0llu;

    // shift one over if its whites turn or multiplu by 2... 8=====D~~~~~**
    if (turn % 2) pieceMask << 1;  

    // skip if we do not have a piece there
    if (!(pieceMask & board.whole)) continue;

    // for each of the four directions we can move for that piece
    for (U8 dir; dir < 4; dir++) {

    }

  }
  
}

int main(int argc, char** argv) {
  
  Bool gaming = Bool_True;
  char *boardFilePath = NULL;
  
  FILE *dump = fopen("dump.txt", "w");

  if (argc > 3) {
    printf("Dude, you got to use this thing properly\n");
    return -1;  
  } else {
    boardFilePath = argv[1];
    
  }

  Arena *arena = ArenaInit(Gigabyte(4)); // Don't worry this won't actually allocate 4 gigabytes

  BitBoard board = BitBoardFromFile(arena, boardFilePath);

  U8 player = (*argv[2] == 'W') ?  PlayerKind_White : PlayerKind_Black;
  U64 playerBoard = (player == PlayerKind_White) ? board.whole & allWhite : board.whole & allBlack;
  U64 allPlayerBoard = (player == PlayerKind_White) ? allWhite : allBlack;

  srand(time(NULL));
  char playerStartingMoves[2][3];
  if (player == PlayerKind_White) {
    strcpy(playerStartingMoves[0], "D4");
    strcpy(playerStartingMoves[1], "E5");
  } else {
    strcpy(playerStartingMoves[0], "D5");
    strcpy(playerStartingMoves[1], "E4");
  }

  char randomStart[3];
  strcpy(randomStart, playerStartingMoves[rand() % 2]);
  
  // // if playerBoard ^ board.allPlayer = 0, start of game. Pick random choice between
  // // center pieces
  // if (!(playerBoard ^ board.whole)) printf("%s\n", randomStart);
  // else {
  //   // do generated moves here
  // }
  // printf("E4\n");
  // printf("E2-E4\n");

  // STRT TEST - John
  // StateNodePool* stateNodePool = StateNodePoolInit(arena);
  // StateNode* stateNode = StateNodePoolAlloc(stateNodePool);
  // stateNode->board = board;
  // U64 playerCanGoTo = getPlayerEmptySpace(stateNode->board, player);
  // StateNodeGenerateChildren(stateNodePool, stateNode, player);
  // STOP TEST - John
  
  // if playerBoard ^ board.allPlayer = 0, start of game. Pick random choice between
  // if (!((board.whole & allPlayerBoard) ^ allPlayerBoard)) {
  //   printf("%s\n", randomStart);
  //   board.whole ^= 1llu<<IndexFromCoord(CoordFromInput(randomStart));
  // }
  
  // Coord e4 = (Coord){4, 4};
  // fprintf(dump, "Player: (%d, %d)\n", e4.x, e4.y);
  // board.whole ^= (1llu<<IndexFromCoord(e4));

  // Coord enemyStone = CoordFromEnemyInput();
  // fprintf(dump, "ENEMY: (%d, %d)\n", enemyStone.x, enemyStone.y);
  // board.whole ^= (1llu<<IndexFromCoord(enemyStone));


  // input algo:
  //    first move? -> use CoordFromEnemyInput(), continue
  //    determine if move is horizontal or vertical
  //    can find this out using Coord1, Coord2
  //    starting point: 1llu << IndexFromCoord(Coord1)
  //    if Coord1.y == Coord2.y -> horizontal
  //      shift amount: abs(Coord1.x - Coord2.x)
  //      if Coord1.x - Coord2.x < 0 -> left shift
  //        for loop i (1, shift amount)
  //        left shift (i) from starting point, |= 1 each time
  //      else                       -> right shift
  //        for loop i (1, shift amount)
  //        right shift (i) from starting point, |= 1 each time
  //     
  //    if Coord1.x == Coord2.x -> vertical
  //      shift amount: abs(Coord1.y - Coord2.y)
  //      if Coord1.y - Coord2.y < 0 -> left shift
  //        for loop i (1, shift amount)
  //          left shift (i*8) from starting point, |= 1 each time
  //      else                       -> right shift
  //        for loop i (1, shift amount)
  //          right shift (i*8) from starting point, |= 1 each time
  //      
  bool blackIsPlayer = (player == PlayerKind_White) ? false : true;
  while (gaming) {
    if (blackIsPlayer) {
      // input()
      // agent
    }

    else {
      // agent
      // input()
    }
  }



  BitBoardFilePrint(dump, board);

  // deinitalization
  ArenaDeinit(arena);

  fclose(dump);
  return 0;
}
