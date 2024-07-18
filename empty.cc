#include "empty.h"
#include "board.h"
#include "piece.h"

// NOTE: Might want to create seperate constructor to decouple whiteTurn from
// Piece
Empty::Empty(std::pair<int, int> coord, bool iswhite) : Piece{coord, iswhite} {}

bool Empty::isMove(const std::pair<int, int> &old_coord,
                   const std::pair<int, int> &new_coord, Board &bd) const {
  return false; // Empty piece is defined to def false
}

bool Empty::isEmptyPiece() const { return true; }

char Empty::key() const { return 'e'; }
