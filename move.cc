#include "move.h"
#include <cctype>

Move::Move(std::pair<int, int> from, std::pair<int, int> to)
    : Npo{to}, Opo{from}, Cpo{to} {}

std::string Move::getflag() const {
  switch (flag) {
  case Castling: return "castling";
  case Promotion: return "promotion";
  case EnPassant: return "enpassant";
  default: return "normal";
  }
}

std::string Move::toString() const {
  if (isNull()) return "(none)";
  std::string s = squareName(Opo) + squareName(Npo);
  if (flag == Promotion) s += static_cast<char>(std::tolower(type2));
  return s;
}

bool Move::operator==(const Move &o) const {
  if (Opo != o.Opo || Npo != o.Npo) return false;
  if (flag == Promotion || o.flag == Promotion)
    return std::tolower(type2) == std::tolower(o.type2);
  return true;
}

std::string Move::squareName(const std::pair<int, int> &sq) {
  std::string s;
  s += static_cast<char>('a' + sq.second);
  s += static_cast<char>('8' - sq.first);
  return s;
}

bool Move::parseSquare(const std::string &s, std::pair<int, int> &out) {
  if (s.size() != 2) return false;
  char f = static_cast<char>(std::tolower(s[0]));
  char r = s[1];
  if (f < 'a' || f > 'h' || r < '1' || r > '8') return false;
  out = {'8' - r, f - 'a'};
  return true;
}
