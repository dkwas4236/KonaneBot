#include "agent.h"
#include "types.h"
#include <stdbool.h>
#include <stdlib.h>
#include "allocators.h"
#include <stdio.h>

#define ALL_BLACK 0xAA55AA55AA55AA55
#define ALL_WHITE 0x55AA55AA55AA55AA

bool shiftValid(U64 jump, U8 shift, bool max);



void StateNodeCalcCost(StateNode* node, char player) {
  // get # of pieces we have
  // get # of pieces enemy has
}


/*
TODO: Create function that goes through all squares.
If getPlayerEmptySpace & currentSpace -> getMovablePieces

for each direction  -> create new board with applied moves
                    -> eval the board
                    -> create state node and connect as child to current node
*/
StateNode* StateNodeGenerateChildren(StateNodePool *pool, StateNode *parent, char playerKind) {
  // 0b01 if black pieces, 0b10 if white pieces
  U64 currentSpace = (playerKind == PlayerKind_White) ? 0x2 : 0x1;
  U64 playerEmpty = getPlayerEmptySpace(parent->board, playerKind);
  // printf("Creating nodes for %d\n", playerKind);
  // printf("Starting space is %llu\n", currentSpace);
  // printf("playerEmpty is: %llu\n", playerEmpty);
  // Go through all squares

  U8 counter = 0;
  U64 checker = playerEmpty, jumpSpace;
  while (checker) {
    jumpSpace = checker & 1;
    if (jumpSpace) {
      U64* piecesList = getMovablePieces(jumpSpace << counter, parent->board, playerKind);
      for (int j = 0; j < 4; j++) {
        // Only create a new state if a move exists in the given direction
        if (piecesList[j]) {
          BitBoard board;
          board.whole = (parent->board.whole)^(piecesList[j]|(jumpSpace << counter));
          StateNode* child = StateNodePoolAlloc(pool);
          child->board = board;
          StateNodePushChild(parent, child);
          // printf("Created Node\t%llu\n", piecesList[j]);
        }
      }
    }
    checker >>= 1;
    counter++;
  }
  // printf("Children count: %llu\n", StateNodeCountChildren(parent));

}

U64 getUpMove(BitBoard board, char player) {
  U64 playerEmpty = getPlayerEmptySpace(board, player);
  U64 allplayer = (player == PlayerKind_White) ? ALL_WHITE : ALL_BLACK;
  U8 counter = 0;
  U64 checker = playerEmpty, jumpSpace;
  while (checker) {
    jumpSpace = checker & 1;
    if (jumpSpace) {
      U64* piecesList = getMovablePieces(jumpSpace << counter, board, player);
      if (piecesList[0]) {
        return ((piecesList[0]|(jumpSpace << counter))&allplayer);
      }
    }
    checker >>= 1;
    counter++;
  }
  return 0;
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
U64* getMovablePieces(U64 jump, BitBoard board, char player) {
  // Not using arena since this is not a state
  U64* piecesModified = malloc(4*sizeof(U64));

  // Initialize values
  piecesModified[0] &= 0;
  piecesModified[1] &= 0;
  piecesModified[2] &= 0;
  piecesModified[3] &= 0;

  U8 dirs = 0xF;
  U64 playerBoard = (player == PlayerKind_White) ? board.whole & ALL_WHITE : board.whole & ALL_BLACK;
  U64 oppBoard = (player == PlayerKind_White) ? board.whole & ALL_BLACK : board.whole & ALL_WHITE;  
  U64 allOppBoard = (player == PlayerKind_White) ? ALL_BLACK : ALL_WHITE;

  for (int checkRange=0; checkRange < 7; checkRange++) {
    U8 vShift = 8*(checkRange+1), hShift = (checkRange+1);
    U64 checkJump;    

    // Check up direction
    if ((dirs & 0x8) && shiftValid(jump, vShift, true)) {
      checkJump = jump << vShift;
      if (checkJump & playerBoard) {
        dirs &= 0x7;
        piecesModified[0] ^= checkJump;
      }
      else if ((checkRange+1)%2 && checkJump & ~oppBoard) dirs &= 0x7;
      else if (checkJump & oppBoard) piecesModified[0] ^= checkJump;
    } else dirs &= 0x7;

    // Check left direction
    if ((dirs & 0x4) && shiftValid(jump, hShift, true)) {
      checkJump = jump << hShift;
      if (!(hShift%2) && (checkJump & allOppBoard)) dirs &= 0xB;
      else if (checkJump & playerBoard) {
        dirs &= 0xB;
        piecesModified[1] ^= checkJump;
      }
      else if (hShift%2 && checkJump & ~oppBoard) dirs &= 0xB;
      else if (checkJump & oppBoard) piecesModified[1] ^= checkJump;
    } else dirs &= 0xB;
    
    // Check down direction
    if ((dirs & 0x2) && shiftValid(jump, vShift, false)) {
      checkJump = jump >> vShift;
      if (checkJump & playerBoard) {
        dirs &= 0xD;
        piecesModified[2] ^= checkJump;
      }
      else if ((checkRange+1)%2 && checkJump & ~oppBoard) dirs &= 0xD;
      else if (checkJump & oppBoard) piecesModified[2] ^= checkJump;
    } else dirs &= 0xD;

    // Check right direction
    if ((dirs & 0x1) && shiftValid(jump, hShift, false)) {
      checkJump = jump >> hShift;
      if (!(hShift%2) && (checkJump & allOppBoard)) dirs &= 0xE;
      else if (checkJump & playerBoard) {
        dirs &= 0xE;
        piecesModified[3] ^= checkJump;
      }
      else if (hShift%2 && (checkJump & ~oppBoard)) dirs &= 0xE;
      else if (checkJump & oppBoard) piecesModified[3] ^= checkJump;

    } else dirs &= 0xE;
  }

  if (!(piecesModified[0] & playerBoard)) piecesModified[0] = 0;
  if (!(piecesModified[1] & playerBoard)) piecesModified[1] = 0;
  if (!(piecesModified[2] & playerBoard)) piecesModified[2] = 0;
  if (!(piecesModified[3] & playerBoard)) piecesModified[3] = 0;

  return piecesModified;
} 

/*
 * Helper function, returns true if given shift is within 64 bits
 */
bool shiftValid(U64 jump, U8 shift, bool max) {
  if (max) return (jump < (0xFFFFFFFFFFFFFFFF >> shift))? true : false;
  return (jump > (1 << shift))? true : false;
}

void StateNodePushChild(StateNode *parent, StateNode *child) {
  if (parent->lastChild) {
    MyAssert(parent->firstChild);
    // link parent to new child
    StateNode *oldLast = parent->lastChild;
    parent->lastChild = child;
    // link children to each other
    child->prev = oldLast;
    oldLast->next = child;
    // increase child count (child count could be useless)
    parent->childCount++;
  } else {
    parent->firstChild = child;
    parent->lastChild = child;
    parent->childCount = 1;
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