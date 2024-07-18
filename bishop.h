#ifndef _BISHOP_H_
#define _BISHOP_H_
#include "piece.h"

class Bishop : public Piece {
public:
  Bishop(std::pair<int, int> coord, bool whiteTurn);

  bool isMove(const std::pair<int, int> &old_coord,
              const std::pair<int, int> &new_coord,
              Board &bd) const override; // tell whether the movment is valid

  bool isEmptyPiece() const override; // tell whether the piece is empty piece
  char key() const override;          // return the type of the piece
};

#endif
