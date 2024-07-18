#ifndef _LEVEL3_H
#define _LEVEL3_H

#include "algorithm.h"
#include <vector>

class Level3 : public Algorithm {
public:

    // Override the calculateBestMove function with Level 3 specific logic
    std::vector<Move> calculateBestMove(Gamecontroller& gameController, bool isWhite) override;

private:
    // Additional Level3 specific helper methods
    bool moveAvoidsCapture(const Move& move, Gamecontroller& gameController);
    bool moveCapturesOpponent(const Move& move, const Board& board);
    bool moveGivesCheck(const Move& move, Gamecontroller& gameController);
};

#endif // LEVEL3_H
