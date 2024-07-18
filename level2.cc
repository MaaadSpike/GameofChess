#include "level2.h"

#include <cstdlib> 
#include <ctime>   
#include <vector>
#include <algorithm> 
#include "gamecontroller.h"

using namespace std;



vector<Move> Level2::calculateBestMove(Gamecontroller& gameController, bool isWhite) {
    vector<Move> responsiveMoves;
    vector<Move> allMoves;
    vector<Move> captureMoves;
    vector<Move> checkMoves;
    vector<Move> regularMoves;
    vector<unique_ptr<Piece>> alivePieces = std::move(gameController.alivePieces());
    
    Board board{*gameController.bd};

    // First, check if the king is in check
    if (gameController.ischeck()) {
        responsiveMoves = generateCheckResponseMoves(alivePieces, gameController, isWhite);
        if (!responsiveMoves.empty()) {
            return responsiveMoves; // Return moves that respond to the check
        }
    }

    // Generate all legal moves for alive pieces of the current player's color
    for (const auto& piece : alivePieces) {
        if (piece->isWhiteTurn() == isWhite) {
            vector<Move> pieceMoves = gameController.possiblemoves(piece);
            for (const auto& move : pieceMoves) {
                if (moveResultsInCapture(move, board)) {
                    captureMoves.push_back(move);
                } else if (moveResultsInCheck(move, gameController)) {
                    checkMoves.push_back(move);
                } else {
                    regularMoves.push_back(move);
                }
            }
        }
    }

    // Prefer capture moves and check-inducing moves
    if (!captureMoves.empty()) {
        return captureMoves;
    } else if (!checkMoves.empty()) {
        return checkMoves;
    }

    // Return regular moves if there are no capture or check moves
    return regularMoves;
}

bool Level2::moveResultsInCapture(const Move& move, const Board& board) {
    // Check if the target position has an opponent's piece
    unique_ptr<Piece> targetPiece = board.getPiece(move.getNpo().first, move.getNpo().second);
    if (targetPiece && targetPiece->isWhiteTurn() != board.getPiece(move.getOpo().first, move.getOpo().second)->isWhiteTurn()) {
        return true; // The move captures an opponent's piece
    }
    return false; // No capture
}


bool Level2::moveResultsInCheck(const Move& move, Gamecontroller& gameController) {
    // Temporarily make the move on the board
    unique_ptr<Piece> tempPiece = gameController.bd->getPiece(move.getOpo().first, move.getOpo().second);
    Board store{*gameController.bd};
    gameController.bd->movePiece(move.getNpo().first, move.getNpo().second, tempPiece);

    // Check if the opponent's king is in check after the move
    //need to be more aware of this part
    bool isCheck = gameController.ischeck();

    // Undo the temporary move
    gameController.bd = std::make_unique<Board>(store);

    return isCheck;
}

