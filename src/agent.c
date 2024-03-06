#include "agent.h"
#include "types.h"
#include "allocators.h"
#include <stdio.h>
#include <string.h>
#include "boardio.h"
#include <limits.h>
#include <assert.h>

#define ALL_BLACK 0xAA55AA55AA55AA55
#define ALL_WHITE 0x55AA55AA55AA55AA
#define DEPTH 5
#define U64PopCount(x) __builtin_popcountll(x)

Bool shiftValid(U64 jump, U8 shift, Bool max);
void StateNodeGenerateChildren(StateNodePool *pool, StateNode *parent, PlayerKind playerKind);


void StateNodeFillPieceList(U64 piecesModified[4], U64 jump, BitBoard board, PlayerKind player) {

  // Initialize values
  piecesModified[0] &= 0;
  piecesModified[1] &= 0;
  piecesModified[2] &= 0;
  piecesModified[3] &= 0;

  U8 dirs = 0xF;
  U64 playerBoard = (player == PlayerKind_White) ? board.whole & ALL_WHITE : board.whole & ALL_BLACK;
  U64 oppBoard = (player == PlayerKind_White) ? board.whole & ALL_BLACK : board.whole & ALL_WHITE;  
  U64 allOppBoard = (player == PlayerKind_White) ? ALL_BLACK : ALL_WHITE;
  U64 allPlayerBoard = (player == PlayerKind_White) ? ALL_WHITE : ALL_BLACK;

  for (int checkRange=0; checkRange < 7; checkRange++) {
    U8 vShift = 8*(checkRange+1), hShift = (checkRange+1);
    U64 checkJump;    

    // Check up direction
    if ((dirs & 0x8) && shiftValid(jump, vShift, Bool_True)) {
      checkJump = jump << vShift;
      if (checkJump & playerBoard) {
        dirs &= 0x7;
        piecesModified[0] ^= checkJump;
      }
      else if ((checkRange+1)%2 && checkJump & ~oppBoard) dirs &= 0x7;
      else if (checkJump & oppBoard) piecesModified[0] ^= checkJump;
      // // This spot can be double jumped. Disable other direction until
      // // max spot has been found
      // else {
      //   piecesModified[2] = 0;
      //   dirs &= 0xD;
      // }
    } else dirs &= 0x7;

    // Check left direction
    if ((dirs & 0x4) && shiftValid(jump, hShift, Bool_True)) {
      checkJump = jump << hShift;
      // Checks if row is same
      if (!(hShift%2) && (checkJump & allOppBoard) || 
          (hShift%2) && (checkJump & allPlayerBoard)) dirs &= 0xB;
      else if (checkJump & playerBoard) {
        dirs &= 0xB;
        piecesModified[1] ^= checkJump;
      }
      else if (hShift%2 && checkJump & ~oppBoard) dirs &= 0xB;
      else if (checkJump & oppBoard) piecesModified[1] ^= checkJump;
      // else {
      //   piecesModified[3] = 0;
      //   dirs &= 0xE;
      // }
    } else dirs &= 0xB;
    
    // Check down direction
    if ((dirs & 0x2) && shiftValid(jump, vShift, Bool_False)) {
      checkJump = jump >> vShift;
      if (checkJump & playerBoard) {
        dirs &= 0xD;
        piecesModified[2] ^= checkJump;
      }
      else if ((checkRange+1)%2 && checkJump & ~oppBoard) dirs &= 0xD;
      else if (checkJump & oppBoard) piecesModified[2] ^= checkJump;
      // else {
      //   piecesModified[0] = 0;
      //   dirs &= 0x7;
      // }
    } else dirs &= 0xD;

    // Check right direction
    if ((dirs & 0x1) && shiftValid(jump, hShift, Bool_False)) {
      checkJump = jump >> hShift;
      if (!(hShift%2) && (checkJump & allOppBoard) || 
          (hShift%2) && (checkJump & allPlayerBoard)) dirs &= 0xE;
      else if (checkJump & playerBoard) {
        dirs &= 0xE;
        piecesModified[3] ^= checkJump;
      }
      else if (hShift%2 && (checkJump & ~oppBoard)) dirs &= 0xE;
      else if (checkJump & oppBoard) piecesModified[3] ^= checkJump;
      // else {
      //   piecesModified[1] = 0;
      //   dirs &= 0xB;
      // }
    } else dirs &= 0xE;
  }

  if (!(piecesModified[0] & playerBoard)) piecesModified[0] = 0;
  if (!(piecesModified[1] & playerBoard)) piecesModified[1] = 0;
  if (!(piecesModified[2] & playerBoard)) piecesModified[2] = 0;
  if (!(piecesModified[3] & playerBoard)) piecesModified[3] = 0;

} 

Bool isOver(StateNode* node) {
  U64 whitePieces = 0, blackPieces = 0;
  U64 whiteSpots = getPlayerEmptySpace(node->board, PlayerKind_White);
  U8 counter = 0;
  U64 checker = whiteSpots, jumpSpace;
  while (checker) {
    jumpSpace = checker & 1;
    if (jumpSpace) {
      U64 piecesList[4];
      StateNodeFillPieceList(piecesList, jumpSpace << counter, node->board, PlayerKind_White);
      for (int j = 0; j < 4; j++) {
        // If a spot is empty, get if this is reachable by a piece. +1 to score for each
        // piece reachable
        if (piecesList[j]) {
          whitePieces |= (piecesList[j] & ALL_WHITE);
        }
      }
    }
    checker >>= 1;
    counter++;
  }

  // Same thing as white pieces but with black pieces.
  U64 blackSpots = getPlayerEmptySpace(node->board, PlayerKind_Black);
  counter = 0;
  checker = blackSpots;
  while (checker) {
    jumpSpace = checker & 1;
    if (jumpSpace) {
      U64 piecesList[4]; StateNodeFillPieceList(&piecesList[0], jumpSpace << counter, node->board, PlayerKind_White);
      for (int j = 0; j < 4; j++) {
        // If a spot is empty, get if this is reachable by a piece. +1 to score for each
        // piece reachable
        if (piecesList[j]) {
          blackPieces |= (piecesList[j] & ALL_BLACK);
        }
      }
    }
    checker >>= 1;
    counter++;
  }

  return !(whitePieces && blackPieces);
}


void agentMove(U8 agentPlayer, BitBoard* board, StateNodePool *pool, int depth) {
  // printf("Agent move: ");

  U64 allPlayerBoard = (agentPlayer == PlayerKind_White) ? ALL_WHITE : ALL_BLACK;
  

  char notRandomStart[3];
  if (agentPlayer == PlayerKind_Black) {
    strcpy(notRandomStart, "D4");
  } else {
    strcpy(notRandomStart, "E4");
  }
  // First move
  if (!((board->whole & allPlayerBoard) ^ allPlayerBoard)) {
    printf("%s\n", notRandomStart);
    board->whole ^= 1llu<<IndexFromCoord(CoordFromInput(notRandomStart));
    return;
  }

  // Determine best move: Generate children of possible moves
  StateNode* stateNode = StateNodePoolAlloc(pool);
  stateNode->board = *board;
  StateNodeGenerateChildren(pool, stateNode, agentPlayer);

  // Go through all children and set their score as minimax()
  for (StateNode* child = stateNode->firstChild; child; child=child->next) {
    child->score = minimax(pool, child, depth, INT_MIN, INT_MAX, (agentPlayer == PlayerKind_White) ?
      1 : 0);
  }

  // Go through all children and print their score
  I32 bestScore = (agentPlayer == PlayerKind_White) ? INT_MAX : INT_MIN;
  StateNode* newState = stateNode->firstChild;
  for (StateNode* child = stateNode->firstChild; child; child=child->next) {
    if (agentPlayer == PlayerKind_White && child->score > newState->score) newState = child;
    else if (agentPlayer == PlayerKind_Black && child->score < newState->score) newState = child;
    // printf("State score: %d\n", child->score);
  }

  printf("%s\n", newState->move);
  *board = newState->board;
}


// score < 0: black favoured (black has more pieces to move)
// score > 0: white favoured (white has more pieces to move)
// score = 0: equal pieces move
void StateNodeCalcCost(StateNode* node) {
  // These hold the pieces that are able to move
  U64 whitePieces = 0, blackPieces = 0;
  
  // Get white pieces empty spots
  U64 whiteSpots = getPlayerEmptySpace(node->board, PlayerKind_White);

  // This algo is really weird.
  // In short, the algo will go through each square.
  // If the square is empty, call getMovablePieces() to get all the pieces that can
  // move into this square.
  // & each direction with ALL_WHITE to get the piece that can move to the empty square
  // | each piece with whitePieces
  // Now we have all the whitePieces that can move
  U8 counter = 0;
  U64 checker = whiteSpots, jumpSpace;
  while (checker) {
    jumpSpace = checker & 1;
    if (jumpSpace) {
      U64 piecesList[4];
      StateNodeFillPieceList(piecesList, jumpSpace << counter, node->board, PlayerKind_White);
      for (int j = 0; j < 4; j++) {
        // If a spot is empty, get if this is reachable by a piece. +1 to score for each
        // piece reachable
        if (piecesList[j]) {
          whitePieces |= (piecesList[j] & ALL_WHITE);
        }
      }
    }
    checker >>= 1;
    counter++;
  }

  // Same thing as white pieces but with black pieces.
  U64 blackSpots = getPlayerEmptySpace(node->board, PlayerKind_Black);
  counter = 0;
  checker = blackSpots;
  while (checker) {
    jumpSpace = checker & 1;
    if (jumpSpace) {
      U64 piecesList[4]; StateNodeFillPieceList(&piecesList[0], jumpSpace << counter, node->board, PlayerKind_White);
      for (int j = 0; j < 4; j++) {
        // If a spot is empty, get if this is reachable by a piece. +1 to score for each
        // piece reachable
        if (piecesList[j]) {
          blackPieces |= (piecesList[j] & ALL_BLACK);
        }
      }
    }
    checker >>= 1;
    counter++;
  }

  // printBoardToConsole(&node->board);
  // printf("# of white pieces movable: %d\n", U64PopCount(whitePieces));
  // printf("# of black pieces movable: %d\n", U64PopCount(blackPieces));
  // printf("This state's score: %d\n", U64PopCount(whitePieces) - U64PopCount(blackPieces));

  node->score = U64PopCount(whitePieces)-U64PopCount(blackPieces);
}


// void StateNodeGenerateChildren2(StateNodePool *pool, StateNode *parent, PlayerKind playerKind) {
//   if (playerKind) {
//     for (U8 i = 0; i < 32; i++) {
//       U8 index = i << 1; // i * 2

//     }
    
//   }
// }

void StateNodeGenerateChildren(StateNodePool *pool, StateNode *parent, PlayerKind playerKind) {
  // 0b01 if black pieces, 0b10 if white pieces

  U64 currentSpace = (playerKind == PlayerKind_White) ? 0x2 : 0x1;
  U64 playerEmpty = getPlayerEmptySpace(parent->board, playerKind);
  U64 allPlayer = (playerKind == PlayerKind_White) ? ALL_WHITE : ALL_BLACK;

  U8 counter = 0;
  U64 checker = playerEmpty, jumpSpace;
  while (checker) {
    jumpSpace = checker & 1;
    if (jumpSpace) {
      U64 piecesList[4];
      StateNodeFillPieceList(piecesList, jumpSpace << counter, parent->board, playerKind);
      for (int j = 0; j < 4; j++) {
        // Only create a new state if a move exists in the given direction
        if (piecesList[j]) {
          BitBoard board;
          board.whole = (parent->board.whole)^(piecesList[j]|(jumpSpace << counter));
          StateNode* child = StateNodePoolAlloc(pool);
          child->board = board;

          // Change this to minimax
          // StateNodeCalcCost(child);

          char coord[3];
          bitToTextCoord(piecesList[j] & allPlayer, coord);
          strcat(child->move, coord);
          strcat(child->move, "-");

          bitToTextCoord(jumpSpace << counter, coord);
          strcat(child->move, coord);

          // printf("Child with move %s created\n", child->move);

          StateNodePushChild(parent, child);
        }
      }
    }
    checker >>= 1;
    counter++;
  }
  // printf("Children count: %llu\n", StateNodeCountChildren(parent));

}


// For the minimax functions
I32 minimax(StateNodePool *pool, StateNode* node, I32 depth, I32 alpha, I32 beta, I32 maximizingPlayer) {
  // printf("Depth remaining: %d\n", depth);
  if (depth == 0 || isOver(node)) {
    //Run Evaluation Function
    StateNodeCalcCost(node);
    return node->score;
  }

  if (maximizingPlayer) {
    I32 maxEval = INT_MIN;

    // Generate children as white
    StateNodeGenerateChildren(pool, node, PlayerKind_White);
    
    for (StateNode* child = node->firstChild; child != NULL; child = child->next) {
      I32 eval = minimax(pool, child, depth -1, alpha, beta, Bool_False);
      maxEval = max(maxEval, eval);
      alpha = max(alpha, eval);
      if (beta <= alpha) {
        break;
      }
    }
    return maxEval;
  }

  else {
    I32 minEval = INT_MAX;

    // Generate children as black
    StateNodeGenerateChildren(pool, node, PlayerKind_Black);

    for (StateNode* child = node->firstChild; child != NULL; child = child->next) {
      I32 eval = minimax(pool, child, depth -1, alpha, beta, Bool_True);
      minEval = min(minEval, eval);
      beta = min(beta, eval);
      if (beta <= alpha) {
        break;
      }
    }
    return minEval;
  }

}



/*
 * This function simply gets all the empty spots the given player can land on.
 */
U64 getPlayerEmptySpace(BitBoard board, char player) {
  return (player == PlayerKind_White) ? (~board.whole) & ALL_WHITE : (~board.whole) & ALL_BLACK;
}

/*
 * Gets all the movable pieces given the empty spots the player can land on. Should
 * be used with getPlayerEmptySpace() and passing one jump at a time to make move
 * calculation easier. Returns an array of movements in each direction.
 * 0 - up
 * 1 - left
 * 2 - down
 * 3 - right
 */

/*
 * Helper function, returns Bool_True if given shift is within 64 bits
 */
Bool shiftValid(U64 jump, U8 shift, Bool max) {
  if (max) return (jump < (0xFFFFFFFFFFFFFFFF >> shift))? Bool_True : Bool_False;
  return (jump >= (1 << shift))? Bool_True : Bool_False;
}

void StateNodePushChild(StateNode *parent, StateNode *child) {
  if (parent->lastChild) {
    // link parent to new child
    StateNode *oldLast = parent->lastChild;
    parent->lastChild = child;
    // link children to each other
    oldLast->next = child;
    // increase child count (child count could be useless)
  } else {
    parent->firstChild = child;
    parent->lastChild = child;
  }
}


U64 StateNodeCountChildren(StateNode *node) {
  U64 count = 0;
  StateNode *curr = node->firstChild;
  while (curr) {
    curr = curr->next;
    count++;
  }

  return count;
}