#ifndef _EMPTY_H_
#define _EMPTY_H_
#include "piece.h"

// Null-object piece that fills every empty square of the board.
class Empty : public Piece {
public:
  Empty(std::pair<int, int> coord, bool iswhite = false);
  void possibleMoves(const Board &, std::vector<Move> &, bool = false) const override {}
  bool isEmptyPiece() const override { return true; }
  char key() const override { return 'e'; }
  int value() const override { return 0; }
  std::unique_ptr<Piece> clone() const override;
};

#endif
