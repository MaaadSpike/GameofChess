#include "pawn.h"
#include "board.h"
#include "move.h"

namespace {
const char PROMOTIONS[4] = {'q', 'n', 'r', 'b'};
}

void Pawn::possibleMoves(const Board &bd, std::vector<Move> &out,
                         bool capturesOnly) const {
  const bool white = isWhiteTurn();
  const int dir = white ? -1 : 1;        // white moves towards row 0
  const int startRow = white ? 6 : 1;
  const int promoRow = white ? 0 : 7;
  const int x = getCoord().first;
  const int y = getCoord().second;
  const int nx = x + dir;
  if (!Board::inBounds(nx, y)) return;

  auto push = [&](int r, int c, char captured, Move::Flag flag,
                  std::pair<int, int> cpo) {
    if (r == promoRow) {
      for (char p : PROMOTIONS) {
        // In capture-only (quiescence) generation, only queen promotions.
        if (capturesOnly && p != 'q') continue;
        Move m{{x, y}, {r, c}};
        m.settype1(key());
        m.settype2(white ? static_cast<char>(p - 'a' + 'A') : p);
        m.settype3(captured);
        m.setpromotion();
        out.push_back(m);
      }
      return;
    }
    Move m{{x, y}, {r, c}};
    m.settype1(key());
    m.settype2(key());
    m.settype3(captured);
    m.setCpo(cpo);
    if (flag == Move::EnPassant) m.setenpassant();
    out.push_back(m);
  };

  // Forward pushes
  if (bd.keyAt(nx, y) == 'e') {
    if (!capturesOnly || nx == promoRow) push(nx, y, 'e', Move::Normal, {nx, y});
    if (!capturesOnly && x == startRow && bd.keyAt(x + 2 * dir, y) == 'e')
      push(x + 2 * dir, y, 'e', Move::Normal, {x + 2 * dir, y});
  }

  // Captures (including en passant)
  for (int dc = -1; dc <= 1; dc += 2) {
    const int c = y + dc;
    if (c < 0 || c > 7) continue;
    char k = bd.keyAt(nx, c);
    if (isEnemy(k)) {
      push(nx, c, k, Move::Normal, {nx, c});
    } else if (k == 'e' && bd.enPassantSquare() == std::pair<int, int>{nx, c}) {
      push(nx, c, bd.keyAt(x, c), Move::EnPassant, {x, c});
    }
  }
}

Pawn::Pawn(std::pair<int, int> coord, bool whiteTurn)
    : Piece{coord, whiteTurn} {}

std::unique_ptr<Piece> Pawn::clone() const {
  auto p = std::make_unique<Pawn>(getCoord(), isWhiteTurn());
  p->setMoved(hasMoved());
  return p;
}
