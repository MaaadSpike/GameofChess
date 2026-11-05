#include "empty.h"

Empty::Empty(std::pair<int, int> coord, bool iswhite) : Piece{coord, iswhite} {}

std::unique_ptr<Piece> Empty::clone() const {
  return std::make_unique<Empty>(getCoord(), isWhiteTurn());
}
