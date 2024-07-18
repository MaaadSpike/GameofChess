#ifndef _EMPTY_H_
#define _EMPTY_H_
#include "piece.h"

class Empty : public Piece {
public:
  Empty(std::pair<int, int> coord, bool iswhite);
  bool isMove(const std::pair<int, int> &old_coord,
              const std::pair<int, int> &new_coord,
              Board &bd) const
      override; // tell whether the movment is valid; Empty defaults to false

  bool isEmptyPiece() const override; // tell whether the piece is empty piece
  char key() const override;          // return the type of the piece
};

#endif
