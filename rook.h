#ifndef _ROOK_H_
#define _ROOK_H_
#include "piece.h"

class Rook : public Piece {
  bool moved; // tell whether the piece has moved from its original position
public:
  Rook(std::pair<int, int> coord, bool whiteTurn);
  bool isMove(const std::pair<int, int> &old_coord,
              const std::pair<int, int> &new_coord,
              Board &bd) const override; // tell whether the movment is valid
  bool isEmptyPiece() const override; // tell whether the piece is empty piece
  char key() const override;          // return the type of the piece
  bool hasMoved() const override;     // tell whether the piece has moved or not
  void setMoved() override;           // set the move flag to true
};

#endif
