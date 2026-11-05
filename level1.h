#ifndef LEVEL1_H
#define LEVEL1_H

#include "algorithm.h"

// Level 1: plays a random legal move.
class Level1 : public Algorithm {
public:
  Move calculateBestMove(const Gamecontroller &gameController) override;
};

#endif // LEVEL1_H
