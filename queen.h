#ifndef _QUEEN_H_
#define _QUEEN_H_
#include "piece.h"

class Queen : public Piece {
public:
  Queen(std::pair<int, int> coord, bool whiteTurn);
  void possibleMoves(const Board &bd, std::vector<Move> &out,
                     bool capturesOnly = false) const override;
  char key() const override { return isWhiteTurn() ? 'Q' : 'q'; }
  int value() const override { return 900; }
  std::unique_ptr<Piece> clone() const override;
};

#endif
