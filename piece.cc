#include "piece.h"
#include "bishop.h"
#include "board.h"
#include "empty.h"
#include "king.h"
#include "knight.h"
#include "move.h"
#include "pawn.h"
#include "queen.h"
#include "rook.h"
#include <cctype>

Piece::Piece(std::pair<int, int> coord, bool whiteTurn)
    : coordinate{coord}, whiteTurn{whiteTurn} {}

bool Piece::isEnemy(char k) const {
  return k != 'e' && (std::isupper(static_cast<unsigned char>(k)) != 0) != whiteTurn;
}

void Piece::addTarget(const Board &bd, int r, int c, std::vector<Move> &out) const {
  Move m{coordinate, {r, c}};
  m.settype1(key());
  m.settype2(key());
  m.settype3(bd.keyAt(r, c));
  out.push_back(m);
}

void Piece::slide(const Board &bd, const int dirs[][2], int ndirs,
                  std::vector<Move> &out, bool capturesOnly) const {
  for (int d = 0; d < ndirs; ++d) {
    int r = coordinate.first + dirs[d][0];
    int c = coordinate.second + dirs[d][1];
    while (Board::inBounds(r, c)) {
      char k = bd.keyAt(r, c);
      if (k == 'e') {
        if (!capturesOnly) addTarget(bd, r, c, out);
      } else {
        if (isEnemy(k)) addTarget(bd, r, c, out);
        break;
      }
      r += dirs[d][0];
      c += dirs[d][1];
    }
  }
}

void Piece::jump(const Board &bd, const int offs[][2], int noffs,
                 std::vector<Move> &out, bool capturesOnly) const {
  for (int i = 0; i < noffs; ++i) {
    int r = coordinate.first + offs[i][0];
    int c = coordinate.second + offs[i][1];
    if (!Board::inBounds(r, c)) continue;
    char k = bd.keyAt(r, c);
    if (k == 'e') {
      if (!capturesOnly) addTarget(bd, r, c, out);
    } else if (isEnemy(k)) {
      addTarget(bd, r, c, out);
    }
  }
}

bool Piece::isMove(const std::pair<int, int> &old_coord,
                   const std::pair<int, int> &new_coord, const Board &bd) const {
  if (old_coord != coordinate) return false;
  std::vector<Move> moves;
  possibleMoves(bd, moves);
  for (const auto &m : moves)
    if (m.getNpo() == new_coord) return true;
  return false;
}

std::unique_ptr<Piece> Piece::create(char key, std::pair<int, int> coord) {
  bool white = std::isupper(static_cast<unsigned char>(key)) != 0;
  switch (std::tolower(static_cast<unsigned char>(key))) {
  case 'k': return std::make_unique<King>(coord, white);
  case 'q': return std::make_unique<Queen>(coord, white);
  case 'r': return std::make_unique<Rook>(coord, white);
  case 'b': return std::make_unique<Bishop>(coord, white);
  case 'n': return std::make_unique<Knight>(coord, white);
  case 'p': return std::make_unique<Pawn>(coord, white);
  default: return std::make_unique<Empty>(coord, false);
  }
}
