#ifndef HUMAN_H
#define HUMAN_H

#include "player.h"
#include <string>

// A human player: moves come from the command line ("move e2 e4").
class Human : public Player {
public:
  Human(int id, bool isWhite) : Player{id, isWhite} {}
  bool isHuman() const override { return true; }
  void setDifficulty(const std::string &) override {}
  std::string describe() const override { return "human"; }
};

#endif // HUMAN_H
