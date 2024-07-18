#ifndef LEVEL4_H
#define LEVEL4_H

#include "algorithm.h"
#include "gamecontroller.h"
#include "move.h"
#include <vector>

class Level4 : public Algorithm {
public:
    std::vector<Move> calculateBestMove(Gamecontroller& gameController, bool isWhite) override;

private:
    int moveCapturesOpponent(const Move& move, const Board& board);
    bool moveAvoidsCapture(const Move& move, Gamecontroller& gameController);
    bool moveGivesCheck(const Move& move, Gamecontroller& gameController);
    int getPieceValue(char pieceType); // Method to get the value of a piece based on its type
};

#endif // LEVEL4_H
