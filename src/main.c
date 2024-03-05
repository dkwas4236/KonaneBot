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
#include <stdbool.h>
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

  

int main(int argc, char** argv) {
  
  Bool gaming = Bool_True;
  char *boardFilePath = NULL;
  
  FILE *dump = fopen("dump.txt", "w");

  PlayerKind agentPlayer;
  if (argc > 3) {
    fprintf(stderr, "Dude, you got to use this thing properly\n");
    return -1;  
  } else {
    boardFilePath = argv[1];
    if (*argv[2] == 'W') {
      agentPlayer = PlayerKind_White;
    } else if (*argv[2] == 'B') {
      agentPlayer = PlayerKind_Black
    } else {
      fprintf(stderr, "Man, You gotta input black or white");
      return -1;
    }
  }

  Arena *arena = ArenaInit(Gigabyte(4)); // Don't worry this won't actually allocate 4 gigabytes

  BitBoard board = BitBoardFromFile(arena, boardFilePath);

  U64 playerBoard = (agentPlayer == PlayerKind_White) ? board.whole & allWhite : board.whole & allBlack;
  U64 allPlayerBoard = (agentPlayer == PlayerKind_White) ? allWhite : allBlack;
  U8 agentOpponent = (agentPlayer == PlayerKind_White) ? PlayerKind_Black : PlayerKind_White;

  srand(time(NULL));
  // char playerStartingMoves[2][3];
  // if (agentPlayer == PlayerKind_White) {
  //   strcpy(playerStartingMoves[0], "D4");
  //   strcpy(playerStartingMoves[1], "E5");
  // } else {
  //   strcpy(playerStartingMoves[0], "D5");
  //   strcpy(playerStartingMoves[1], "E4");
  // }

  // char randomStart[3];
  // strcpy(randomStart, playerStartingMoves[rand() % 2]);
  
  // // if playerBoard ^ board.allPlayer = 0, start of game. Pick random choice between
  // // center pieces
  // if (!(playerBoard ^ board.whole)) printf("%s\n", randomStart);
  // else {
  //   // do generated moves here
  // }
  // printf("E4\n");
  // printf("E2-E4\n");

  // STRT TEST - John
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


  // input algo:    -> takes *board, player  -> returns none
  //    first move? -> use CoordFromEnemyInput(), continue DONE
  //    else
  //    input move function -> returns * 2 coords DONE
  //    then,
  //    determine if move is horizontal or vertical     
  //    can find this out using Coord1, Coord2          
  //    starting point: 1llu << IndexFromCoord(Coord1)  
  //    if Coord1.y == Coord2.y -> horizontal
  //      shift amount: abs(Coord1.x - Coord2.x)
  //      if Coord1.x - Coord2.x < 0 -> left shift
  //        for loop i (1, shift amount)
  //          left shift 1 from starting point, |= 1 each time
  //      else                       -> right shift
  //        for loop i (1, shift amount)
  //          right shift 1 from starting point, |= 1 each time
  //     
  //    if Coord1.x == Coord2.x -> vertical
  //      shift amount: abs(Coord1.y - Coord2.y)
  //      if Coord1.y - Coord2.y < 0 -> left shift
  //        for loop i (1, shift amount)
  //          left shift 8 from starting point, |= 1 each time
  //      else                       -> right shift
  //        for loop i (1, shift amount)
  //          right shift 8 from starting point, |= 1 each time
  //      
  int turns = 1;
  int depth = 6;
  while (gaming) {
    StateNodePool* stateNodePool = StateNodePoolInit(arena);
    if (turns >= 8) depth = 5;
    if (agentPlayer == PlayerKind_White) {
      agentMove(agentPlayer, &board, stateNodePool, depth);
      // agent
      // input()
      mainInput(&board, agentOpponent);
    }

    else {
      // input()
      // then
      // agent

      // Somehow this fixes drivercheck
      agentMove(agentPlayer, &board, stateNodePool, depth);
      mainInput(&board, agentOpponent);
    }

    // calculate score
    // minimax(pool, child, 2, INT_MIN, INT_MAX, (playerKind == PlayerKind_White) ? true : false);

    // show board
    // printBoardToConsole(&board);
    // StateNodePoolFree(stateNodePool, stateNode);
    turns++;
    ArenaReset(arena); // Please that we need to free memory
  }



  BitBoardFilePrint(dump, board);

  // deinitalization
  ArenaDeinit(arena);

  fclose(dump);
  return 0;
}
