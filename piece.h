#ifndef _PIECE_H_
#define _PIECE_H_

#include <memory>
#include <utility>

class Board; // Forward Declaration of Board class -- two classes refer
             // eachother

class Piece {
  std::pair<int, int> coordinate;
  bool whiteTurn; // tell which side the piece belongs to;true means
                  // white;flase, black

public:
  Piece(std::pair<int, int> coord, bool whiteTurn);

  std::pair<int, int> getCoord() const; // return the position of the piece
  void setCoord(const std::pair<int, int>
                    &new_coord); // set the position of piece to new position

  bool isWhiteTurn() const;

  // FIX: Board should be const, refer to knight.cc
  virtual bool isMove(const std::pair<int, int> &old_coord,
                      const std::pair<int, int> &new_coord,
                      Board &bd) const = 0; // tell whether the movment is valid

  // TODO
  // Note : emptyPiece isnt necessary
  //        better solution could be using key and checking if key = 'e'
  virtual bool
  isEmptyPiece() const = 0;     // tell whether the piece is empty piece
  virtual char key() const = 0; // return the unique key assigned to the piece

  // TODO
  // Note: non virt?
  virtual bool hasMoved() const; // tell whether the piece has moved or not
  virtual void setMoved();       // set the move flag to true
};

#endif
