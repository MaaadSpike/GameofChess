#include "rook.h"
#include "board.h"
#include "move.h"

namespace {
const int DIRS[4][2] = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}};
}

void Rook::possibleMoves(const Board &bd, std::vector<Move> &out,
                         bool capturesOnly) const {
  slide(bd, DIRS, 4, out, capturesOnly);
}

Rook::Rook(std::pair<int, int> coord, bool whiteTurn)
    : Piece{coord, whiteTurn} {}

std::unique_ptr<Piece> Rook::clone() const {
  auto p = std::make_unique<Rook>(getCoord(), isWhiteTurn());
  p->setMoved(hasMoved());
  return p;
}
