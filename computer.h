#ifndef COMPUTER_H
#define COMPUTER_H

#include "algorithm.h"
#include "player.h"
#include <memory>
#include <string>

class Gamecontroller;

// A computer player delegates move choice to an Algorithm (strategy pattern):
// level1 random, level2 captures/checks, level3 short search, level4 engine.
class Computer : public Player {
  std::string difficultyLevel;
  std::unique_ptr<Algorithm> algorithm;

public:
  Computer(int id, bool isWhite, const std::string &difficultyLevel);
  ~Computer() override;

  bool isHuman() const override { return false; }
  void setDifficulty(const std::string &level) override;
  std::string describe() const override { return "computer" + std::to_string(id); }

  // Decide (but do not play) the move for the current position.
  Move decideMove(const Gamecontroller &gameController);
  const std::string &lastInfo() const { return algorithm->lastInfo(); }
};

#endif // COMPUTER_H
