#include "knight.h"
#include "board.h"
#include "move.h"

namespace {
const int OFFS[8][2] = {{-2, -1}, {-2, 1}, {-1, -2}, {-1, 2},
                        {1, -2},  {1, 2},  {2, -1},  {2, 1}};
}

void Knight::possibleMoves(const Board &bd, std::vector<Move> &out,
                           bool capturesOnly) const {
  jump(bd, OFFS, 8, out, capturesOnly);
}

Knight::Knight(std::pair<int, int> coord, bool whiteTurn)
    : Piece{coord, whiteTurn} {}

std::unique_ptr<Piece> Knight::clone() const {
  auto p = std::make_unique<Knight>(getCoord(), isWhiteTurn());
  p->setMoved(hasMoved());
  return p;
}
