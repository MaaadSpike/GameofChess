#include "queen.h"
#include "board.h"
#include "move.h"

namespace {
const int DIRS[8][2] = {{-1, 0}, {1, 0}, {0, -1}, {0, 1},
                        {-1, -1}, {-1, 1}, {1, -1}, {1, 1}};
}

void Queen::possibleMoves(const Board &bd, std::vector<Move> &out,
                          bool capturesOnly) const {
  slide(bd, DIRS, 8, out, capturesOnly);
}

Queen::Queen(std::pair<int, int> coord, bool whiteTurn)
    : Piece{coord, whiteTurn} {}

std::unique_ptr<Piece> Queen::clone() const {
  auto p = std::make_unique<Queen>(getCoord(), isWhiteTurn());
  p->setMoved(hasMoved());
  return p;
}
