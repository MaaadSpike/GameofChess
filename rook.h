#ifndef _ROOK_H_
#define _ROOK_H_
#include "piece.h"

class Rook : public Piece {
public:
  Rook(std::pair<int, int> coord, bool whiteTurn);
  void possibleMoves(const Board &bd, std::vector<Move> &out,
                     bool capturesOnly = false) const override;
  char key() const override { return isWhiteTurn() ? 'R' : 'r'; }
  int value() const override { return 500; }
  std::unique_ptr<Piece> clone() const override;
};

#endif
