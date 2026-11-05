#ifndef _LEVEL3_H
#define _LEVEL3_H

#include "algorithm.h"

// Level 3: prefers avoiding capture, capturing moves, and checks.
// Implemented as a 2-ply search (its move + the opponent's best reply)
// followed by a capture-only "quiescence" search, so it sees hanging pieces
// and simple exchanges, plus a little randomness for variety.
class Level3 : public Algorithm {
  int quiesce(Board &bd, int alpha, int beta, int ply);
  int reply(Board &bd, int alpha, int beta);

public:
  Move calculateBestMove(const Gamecontroller &gameController) override;
};

#endif // _LEVEL3_H
