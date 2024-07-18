#include "knight.h"
#include "board.h"
#include "piece.h"

Knight::Knight(std::pair<int, int> coord, bool whiteTurn)
    : Piece{coord, whiteTurn} {}

bool Knight::isMove(const std::pair<int, int> &old_coord,
                    const std::pair<int, int> &new_coord, Board &bd) const {

  if (new_coord.first >= 8 || new_coord.first < 0 || new_coord.second >= 8 ||
      new_coord.second < 0) {
    return false;
  }
  if (isWhiteTurn() ==
          bd.getPiece(new_coord.first, new_coord.second)->isWhiteTurn() &&
      !bd.getPiece(new_coord.first, new_coord.second)->isEmptyPiece()) {
    return false;
  }

  if (abs(new_coord.first - old_coord.first) == 2 &&
      abs(new_coord.second - old_coord.second) == 1) {
    return true;
  }

  if (abs(new_coord.first - old_coord.first) == 1 &&
      abs(new_coord.second - old_coord.second) == 2) {
    return true;
  }

  return false;
}

bool Knight::isEmptyPiece() const { return false; }

char Knight::key() const {
  if (isWhiteTurn()) {
    return 'N';
  }
  return 'n';
}
