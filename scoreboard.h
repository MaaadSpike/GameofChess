#ifndef _SCOREBOARD_H_
#define _SCOREBOARD_H_
#include <string>

// Running score across games: win = 1, draw = 0.5 each.
class Scoreboard {
  double whitescores = 0;
  double blackscores = 0;

public:
  void addscores(const std::string &result); // "Whitewin", "Blackwin" or "draw"
  double getwhites() const { return whitescores; }
  double getblacks() const { return blackscores; }
  std::string summary() const; // "White 1 - Black 0.5"
  void printscoreboard() const;
};
#endif
