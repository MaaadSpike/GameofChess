#include "king.h"
#include "board.h"

King::King(std::pair<int, int> coord, bool whiteTurn)
    : Piece{coord, whiteTurn}, moved{false} {}

bool King::isMove(const std::pair<int, int> &old_coord,
                  const std::pair<int, int> &new_coord, Board &bd) const {
  // Check single step moves
  if (new_coord.first >= 8 || new_coord.first < 0 ||
  new_coord.second >= 8 || new_coord.second < 0) {
    return false;
  }
  if (isWhiteTurn() ==
          bd.getPiece(new_coord.first, new_coord.second)->isWhiteTurn() &&
      !bd.getPiece(new_coord.first, new_coord.second)->isEmptyPiece()) {
    return false;
  }

  // diagonal movements
  if (abs(new_coord.first - old_coord.first) == 1 &&
      abs(new_coord.second - old_coord.second) == 1) {
    return true;
  }

  if (abs(new_coord.first - old_coord.first) == 1 &&
      abs(new_coord.second - old_coord.second) == 0) {
    return true;
  }

  if (abs(new_coord.first - old_coord.first) == 0 &&
      abs(new_coord.second - old_coord.second) == 1) {
    return true;
  }

  // Check castle
  int x = old_coord.first;
  //int y = old_coord.second;

  // NOTE: No need to consider players differently as initial setup is the same
  if (!hasMoved() && (old_coord.first == new_coord.first)) {
    if (((old_coord.second - new_coord.second) == 2) &&
        tolower(bd.getPiece(x, 0)->key()) == 'r' &&
        !bd.getPiece(x, 0)->hasMoved() &&
        tolower(bd.getPiece(x, 1)->key()) == 'e' &&
        tolower(bd.getPiece(x, 2)->key()) == 'e' &&
        tolower(bd.getPiece(x, 3)->key()) == 'e') {
      return true;
    }
    if (((new_coord.second - old_coord.second) == 2) &&
        tolower(bd.getPiece(x, 7)->key()) == 'r' &&
        !bd.getPiece(x, 7)->hasMoved() &&
        tolower(bd.getPiece(x, 6)->key()) == 'e' &&
        tolower(bd.getPiece(x, 5)->key()) == 'e') {
      return true;
    }
  }

  return false;
}

bool King::isEmptyPiece() const { return false; }

char King::key() const {
  if (isWhiteTurn()) {
    return 'K';
  }
  return 'k';
}

bool King::hasMoved() const { return moved; }

void King::setMoved() { moved = true; }
