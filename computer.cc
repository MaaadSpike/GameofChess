#include "computer.h"
#include "gamecontroller.h"
#include "level1.h"
#include "level2.h"
#include "level3.h"
#include "level4.h"

Computer::Computer(int id, bool isWhite, const std::string &difficultyLevel)
    : Player{id, isWhite} {
  setDifficulty(difficultyLevel);
}

Computer::~Computer() = default;

void Computer::setDifficulty(const std::string &level) {
  difficultyLevel = level;
  if (level == "level1") {
    algorithm = std::make_unique<Level1>();
    id = 1;
  } else if (level == "level2") {
    algorithm = std::make_unique<Level2>();
    id = 2;
  } else if (level == "level3") {
    algorithm = std::make_unique<Level3>();
    id = 3;
  } else {
    algorithm = std::make_unique<Level4>();
    id = 4;
  }
}

Move Computer::decideMove(const Gamecontroller &gameController) {
  return algorithm->calculateBestMove(gameController);
}
