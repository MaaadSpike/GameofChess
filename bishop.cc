#include "bishop.h"
#include "board.h"
#include "move.h"

namespace {
const int DIRS[4][2] = {{-1, -1}, {-1, 1}, {1, -1}, {1, 1}};
}

void Bishop::possibleMoves(const Board &bd, std::vector<Move> &out,
                           bool capturesOnly) const {
  slide(bd, DIRS, 4, out, capturesOnly);
}

Bishop::Bishop(std::pair<int, int> coord, bool whiteTurn)
    : Piece{coord, whiteTurn} {}

std::unique_ptr<Piece> Bishop::clone() const {
  auto p = std::make_unique<Bishop>(getCoord(), isWhiteTurn());
  p->setMoved(hasMoved());
  return p;
}
