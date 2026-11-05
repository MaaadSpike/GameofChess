#ifndef _MOVE_H_
#define _MOVE_H_
#include <string>
#include <utility>

// A Move stores:
//   Opo / type1 : original position and the type of the moving piece
//   Npo / type2 : new position and the type that ends up there (differs on promotion)
//   Cpo / type3 : position and type of the captured piece ('e' if nothing is captured)
//   flag        : normal / castling / promotion / en passant
//
// Coordinates are (row, col) with row 0 = rank 8 and col 0 = file a.
class Move {
public:
  enum Flag : unsigned char { Normal = 0, Castling, Promotion, EnPassant };

private:
  std::pair<int, int> Npo{-1, -1}; // new position
  std::pair<int, int> Opo{-1, -1}; // original position
  std::pair<int, int> Cpo{-1, -1}; // captured position
  char type1 = 'e';                // original type
  char type2 = 'e';                // new type
  char type3 = 'e';                // captured type
  Flag flag = Normal;

public:
  Move() = default;
  Move(std::pair<int, int> from, std::pair<int, int> to);

  bool iscasteld() const { return flag == Castling; }
  bool ispromoted() const { return flag == Promotion; }
  bool isenpassant() const { return flag == EnPassant; }
  bool iscapture() const { return type3 != 'e'; }
  bool isNull() const { return Opo.first < 0; }

  std::pair<int, int> getNpo() const { return Npo; }
  std::pair<int, int> getOpo() const { return Opo; }
  std::pair<int, int> getCpo() const { return Cpo; }
  char gettype1() const { return type1; }
  char gettype2() const { return type2; }
  char gettype3() const { return type3; }
  Flag getFlagValue() const { return flag; }
  std::string getflag() const;

  void setNpo(const std::pair<int, int> &p) { Npo = p; }
  void setOpo(const std::pair<int, int> &p) { Opo = p; }
  void setCpo(const std::pair<int, int> &p) { Cpo = p; }
  void settype1(char c) { type1 = c; }
  void settype2(char c) { type2 = c; }
  void settype3(char c) { type3 = c; }
  void setcastling() { flag = Castling; }
  void setpromotion() { flag = Promotion; }
  void setenpassant() { flag = EnPassant; }

  // "e2e4", "e7e8q"
  std::string toString() const;

  // Same from/to squares and (for promotions) same promoted piece.
  bool operator==(const Move &o) const;
  bool operator!=(const Move &o) const { return !(*this == o); }

  static std::string squareName(const std::pair<int, int> &sq);
  static bool parseSquare(const std::string &s, std::pair<int, int> &out);
};

#endif
