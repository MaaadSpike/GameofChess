#ifndef _KING_H_
#define _KING_H_
#include "piece.h"

class King : public Piece {
public:
  King(std::pair<int, int> coord, bool whiteTurn);
  void possibleMoves(const Board &bd, std::vector<Move> &out,
                     bool capturesOnly = false) const override;
  char key() const override { return isWhiteTurn() ? 'K' : 'k'; }
  int value() const override { return 0; }
  std::unique_ptr<Piece> clone() const override;
};

#endif
