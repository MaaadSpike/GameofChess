#ifndef _ALGORITHM_H
#define _ALGORITHM_H

//Forward declarations
class Computer;
class Move;
class Piece;
class Gamecontroller;
class Move;
class board;

//#include "computer.h"
#include "board.h"
#include "move.h"

using namespace std;

class Algorithm {
public:
    virtual vector<Move> calculateBestMove(Gamecontroller& gameController, bool isWhite) = 0;
    // Other common methods or attributes for chess algorithms
    Move selectRandomMove(const vector<Move>& moves);
    vector<Move> generateCheckResponseMoves(const vector<unique_ptr<Piece>>& pieces, const Gamecontroller& gc, bool isWhite);
    pair<int, int> findKing(const Board& bd, bool isWhite);
    vector<pair<int, int>> getThreateningPieces(const Board& bd, bool isWhite);
    bool isThreateningPiece(const Board& bd, int x, int y, std::pair<int, int> kingPos, bool isWhite);
    bool canReachKing(const Board& bd, int x, int y, std::pair<int, int> kingPos, bool checkDiagonal, bool checkStraight);
    bool canReachKingStraight(const Board& bd, int x, int y, std::pair<int, int> kingPos);
    bool canReachKingDiagonal(const Board& bd, int x, int y, std::pair<int, int> kingPos);
    vector<Move> canPieceBeCaptured(const vector<unique_ptr<Piece>>& pieces, Piece* threateningPiece, const Gamecontroller& gameController, bool isWhite);
    vector<Move> captureMove(const unique_ptr<Piece>& capturingPiece, const Piece* targetPiece, const Gamecontroller& gameController);
};

#endif // ALGORITHM_H
