#ifndef _KNIGHT_H_
#define _KNIGHT_H_
#include "piece.h"

class Knight : public Piece {
public:
  Knight(std::pair<int, int> coord, bool whiteTurn);
  void possibleMoves(const Board &bd, std::vector<Move> &out,
                     bool capturesOnly = false) const override;
  char key() const override { return isWhiteTurn() ? 'N' : 'n'; }
  int value() const override { return 320; }
  std::unique_ptr<Piece> clone() const override;
};

#endif
