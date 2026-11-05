#ifndef _PAWN_H_
#define _PAWN_H_
#include "piece.h"

class Pawn : public Piece {
public:
  Pawn(std::pair<int, int> coord, bool whiteTurn);
  void possibleMoves(const Board &bd, std::vector<Move> &out,
                     bool capturesOnly = false) const override;
  char key() const override { return isWhiteTurn() ? 'P' : 'p'; }
  int value() const override { return 100; }
  std::unique_ptr<Piece> clone() const override;
};

#endif
