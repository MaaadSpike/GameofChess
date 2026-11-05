#ifndef _PIECE_H_
#define _PIECE_H_

#include <memory>
#include <utility>
#include <vector>

class Board; // Board and Piece refer to each other
class Move;

// Abstract base of every piece (including the Empty "null piece").
// Each concrete piece knows how it moves: possibleMoves() generates its
// pseudo-legal moves (legal except that the own king may be left in check;
// the Board filters those out).
class Piece {
  std::pair<int, int> coordinate;
  bool whiteTurn; // which side the piece belongs to; true = white
  bool moved = false;

protected:
  bool isEnemy(char k) const; // is the piece key k an opponent piece?
  void addTarget(const Board &bd, int r, int c, std::vector<Move> &out) const;
  // Sliding pieces (rook / bishop / queen)
  void slide(const Board &bd, const int dirs[][2], int ndirs,
             std::vector<Move> &out, bool capturesOnly) const;
  // Jumping pieces (knight / king single steps)
  void jump(const Board &bd, const int offs[][2], int noffs,
            std::vector<Move> &out, bool capturesOnly) const;

public:
  Piece(std::pair<int, int> coord, bool whiteTurn);
  virtual ~Piece() = default;

  std::pair<int, int> getCoord() const { return coordinate; }
  void setCoord(const std::pair<int, int> &c) { coordinate = c; }
  bool isWhiteTurn() const { return whiteTurn; }

  // Append this piece's pseudo-legal moves to out.
  virtual void possibleMoves(const Board &bd, std::vector<Move> &out,
                             bool capturesOnly = false) const = 0;
  // Can this piece go from old_coord to new_coord (ignoring self-check)?
  virtual bool isMove(const std::pair<int, int> &old_coord,
                      const std::pair<int, int> &new_coord,
                      const Board &bd) const;

  virtual bool isEmptyPiece() const { return false; }
  virtual char key() const = 0;   // 'K','q','e',...
  virtual int value() const = 0;  // material value in centipawns
  virtual std::unique_ptr<Piece> clone() const = 0;

  bool hasMoved() const { return moved; }
  void setMoved(bool m = true) { moved = m; }

  // Factory: build a piece from its key ('e' gives an Empty)
  static std::unique_ptr<Piece> create(char key, std::pair<int, int> coord);
};

#endif
