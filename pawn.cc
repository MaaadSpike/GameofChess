#include "pawn.h"
#include "board.h"
#include "piece.h"
#include <iostream>

Pawn::Pawn(std::pair<int, int> coord, bool whiteTurn)
    : Piece{coord, whiteTurn}, moved{false} {}

bool Pawn::isMove(const std::pair<int, int> &old_coord,
                  const std::pair<int, int> &new_coord, Board &bd) const {
  // NOTE: En passant can't be handled within piece since the state of the
  // previous board is needed En passant
  //  Adding an firstMove flag doesn't resolve it, as board would have to
  // manually toggle the flag off the following turn, as en passant becomes
  // invalid
  //if (new_coord.first - old_coord.first == -1 &&
  //    abs(new_coord.second - old_coord.second) == 1 &&
  //    !bd.getPiece(new_coord.first, new_coord.second)->isEmptyPiece() &&
  //    old_coord.first == 3 &&
  //    bd.getPiece(3, new_coord.second)->isWhiteTurn() != isWhiteTurn() &&
  //    // Check for 3 explicitly as it is the row where en passant occurs
  //    tolower(bd.getPiece(3, new_coord.second)->key()) == 'p') {
  //  return true;
  //}
//
  //if (new_coord.first - old_coord.first == 1 &&
  //    abs(new_coord.second - old_coord.second) == 1 &&
  //    !bd.getPiece(new_coord.first, new_coord.second)->isEmptyPiece() &&
  //    old_coord.first == 4 &&
  //    bd.getPiece(4, new_coord.second)->isWhiteTurn() != isWhiteTurn() &&
  //    // Check for 4 explicitly as it is the row where en passant occurs
  //    tolower(bd.getPiece(4, new_coord.second)->key()) == 'p') {
  //  return true;
  //}

  // Pawn push
  if (new_coord.first >= 8 || new_coord.first < 0 ||
  new_coord.second >= 8 || new_coord.second < 0) {
    return false;
  }
  std::cout << "pawn.cc"<<"37" << std::endl;
  if (old_coord.second == new_coord.second) {
    std::cout << "pawn.cc "<<"39" << std::endl;
    if (isWhiteTurn() && old_coord.first - new_coord.first == 1 &&
        bd.getPiece(new_coord.first, new_coord.second)->isEmptyPiece()) {
          std::cout << "pawn.cc "<<"42" << std::endl;
      return true;
    }

    if (!isWhiteTurn() && old_coord.first - new_coord.first == -1 &&
        bd.getPiece(new_coord.first, new_coord.second)->isEmptyPiece()) {
          std::cout << "pawn.cc "<<"48" << std::endl;
      return true;
    }
    std::cout << "pawn.cc "<<"51" << std::endl;
    
    if (isWhiteTurn() && new_coord.first - old_coord.first == -2 &&
        !bd.getPiece(old_coord.first, old_coord.second)->hasMoved() &&
        bd.getPiece(old_coord.first - 1, old_coord.second)->isEmptyPiece() &&
        bd.getPiece(new_coord.first, new_coord.second)->isEmptyPiece()) {
      return true;
    }

    if (!isWhiteTurn() && new_coord.first - old_coord.first == 2 &&
        !bd.getPiece(old_coord.first, old_coord.second)->hasMoved() &&
        bd.getPiece(old_coord.first + 1, old_coord.second)->isEmptyPiece() &&
        bd.getPiece(new_coord.first, new_coord.second)->isEmptyPiece()) {
      return true;
    }

    return false;
  }
  std::cout << "pawn.cc "<<"69" << std::endl;
  // Capture
  if (isWhiteTurn() && new_coord.first - old_coord.first == -1 &&
      abs(new_coord.second - old_coord.second) == 1 &&
      !bd.getPiece(new_coord.first, new_coord.second)->isEmptyPiece() &&
      bd.getPiece(new_coord.first, new_coord.second)->isWhiteTurn() !=
          isWhiteTurn()) {
            std::cout << "pawn.cc "<<"76" << std::endl;
    return true;
  }
  std::cout << "pawn.cc "<<"79" << std::endl;

  if (!isWhiteTurn() && new_coord.first - old_coord.first == 1 &&
      abs(new_coord.second - old_coord.second) == 1 &&
      !bd.getPiece(new_coord.first, new_coord.second)->isEmptyPiece() &&
      bd.getPiece(new_coord.first, new_coord.second)->isWhiteTurn() !=
          isWhiteTurn()) {
            std::cout << "pawn.cc "<<"85" << std::endl;
    return true;
  }
  std::cout << "pawn.cc "<<"83" << std::endl;

  return false;
}
// std::cout << "pawn.cc "<<"85" << std::endl;
bool Pawn::isEmptyPiece() const { return false; }

char Pawn::key() const {
  if (isWhiteTurn()) {
    return 'P';
  }
  return 'p';
}

// std::cout << "pawn.cc "<<"94" << std::endl;
bool Pawn::hasMoved() const { return moved; }

void Pawn::setMoved() { moved = true; }
