#include "king.h"
#include "board.h"
#include "move.h"

namespace {
const int OFFS[8][2] = {{-1, -1}, {-1, 0}, {-1, 1}, {0, -1},
                        {0, 1},   {1, -1}, {1, 0},  {1, 1}};
}

void King::possibleMoves(const Board &bd, std::vector<Move> &out,
                         bool capturesOnly) const {
  jump(bd, OFFS, 8, out, capturesOnly);
  if (capturesOnly) return;

  // Castling: the king may not be in check, pass through or land on an
  // attacked square, and the squares between king and rook must be empty.
  const bool white = isWhiteTurn();
  const int row = white ? 7 : 0;
  if (getCoord() != std::pair<int, int>{row, 4}) return;
  const int rights = bd.castlingRights();
  const char rook = white ? 'R' : 'r';
  const int kingSide = white ? Board::WHITE_OO : Board::BLACK_OO;
  const int queenSide = white ? Board::WHITE_OOO : Board::BLACK_OOO;

  if ((rights & (kingSide | queenSide)) == 0) return;
  if (bd.isAttacked(row, 4, !white)) return;

  if ((rights & kingSide) && bd.keyAt(row, 7) == rook &&
      bd.keyAt(row, 5) == 'e' && bd.keyAt(row, 6) == 'e' &&
      !bd.isAttacked(row, 5, !white) && !bd.isAttacked(row, 6, !white)) {
    Move m{{row, 4}, {row, 6}};
    m.settype1(key());
    m.settype2(key());
    m.setcastling();
    out.push_back(m);
  }
  if ((rights & queenSide) && bd.keyAt(row, 0) == rook &&
      bd.keyAt(row, 1) == 'e' && bd.keyAt(row, 2) == 'e' &&
      bd.keyAt(row, 3) == 'e' && !bd.isAttacked(row, 3, !white) &&
      !bd.isAttacked(row, 2, !white)) {
    Move m{{row, 4}, {row, 2}};
    m.settype1(key());
    m.settype2(key());
    m.setcastling();
    out.push_back(m);
  }
}

King::King(std::pair<int, int> coord, bool whiteTurn)
    : Piece{coord, whiteTurn} {}

std::unique_ptr<Piece> King::clone() const {
  auto p = std::make_unique<King>(getCoord(), isWhiteTurn());
  p->setMoved(hasMoved());
  return p;
}
