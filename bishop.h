#ifndef _BISHOP_H_
#define _BISHOP_H_
#include "piece.h"

class Bishop : public Piece {
public:
  Bishop(std::pair<int, int> coord, bool whiteTurn);
  void possibleMoves(const Board &bd, std::vector<Move> &out,
                     bool capturesOnly = false) const override;
  char key() const override { return isWhiteTurn() ? 'B' : 'b'; }
  int value() const override { return 330; }
  std::unique_ptr<Piece> clone() const override;
};

#endif
