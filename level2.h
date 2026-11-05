#ifndef LEVEL2_H
#define LEVEL2_H

#include "algorithm.h"

// Level 2: prefers checkmates, then capturing moves (bigger prey first) and
// checks over other moves; otherwise random.
class Level2 : public Algorithm {
public:
  Move calculateBestMove(const Gamecontroller &gameController) override;
};

#endif // LEVEL2_H
