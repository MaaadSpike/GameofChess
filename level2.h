#ifndef LEVEL2_H
#define LEVEL2_H

#include "algorithm.h"

class Level2 : public Algorithm {

    private:
        bool moveResultsInCapture(const Move& move, const Board& board);
        bool moveResultsInCheck(const Move& move, Gamecontroller& gameController);


    public:
        // Override the calculateBestMove function with Level 1 specific logic
        vector<Move> calculateBestMove(Gamecontroller& gameController, bool isWhite) override;

};

#endif // LEVEL2_H
