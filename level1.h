#ifndef LEVEL1_H
#define LEVEL1_H

#include "algorithm.h"
#include "gamecontroller.h"

class Level1 : public Algorithm {
public:
    // Override the calculateBestMove function with Level 1 specific logic
    vector<Move> calculateBestMove(Gamecontroller& gameController, bool isWhite) override;
};

#endif // LEVEL1_H
