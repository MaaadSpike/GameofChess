#include "level3.h"
#include "move.h"
#include "gamecontroller.h"
#include <algorithm> // For std::sort
#include <iostream>
#include <ctime>

std::vector<Move> Level3::calculateBestMove(Gamecontroller& gameController, bool isWhite) {
    std::vector<Move> allMoves;
    std::vector<Move> responsiveMoves;
    std::vector<std::pair<Move, int>> scoredMoves; // Pair of Move and its score

    std::vector<std::unique_ptr<Piece>> alivePieces = gameController.alivePieces();
    Board board{*gameController.bd};
    std::cout << "1" << std::endl;
    // First, check if the king is in check
    if (gameController.ischeck()) {
        std::cout << "2" << std::endl;
        responsiveMoves = generateCheckResponseMoves(alivePieces, gameController, isWhite);
        if (!responsiveMoves.empty()) {
            return responsiveMoves; // Return moves that respond to the check
        }
    }

    std::cout << "3" << std::endl;
    // Generate all possible moves
    for (auto& piece : alivePieces) {
        std::cout << "4" << std::endl;
        if (piece->isWhiteTurn() == isWhite) {
            std::cout << "type: " << piece->key() << std::endl;
            vector<Move> pieceMoves = gameController.possiblemoves(piece);
            allMoves.insert(allMoves.end(), pieceMoves.begin(), pieceMoves.end());
            std::cout << "5" << std::endl;
            // Evaluate and prioritize moves
            for (auto& move : pieceMoves) {
                std::cout << "6" << std::endl;
                int moveScore = 0;
                if (moveAvoidsCapture(move, gameController)) {
                    moveScore += 10; // Arbitrary score for avoiding capture
                }
                if (moveCapturesOpponent(move, board)) {
                    moveScore += 20; // Arbitrary score for capturing an opponent
                }
                if (moveGivesCheck(move, gameController)) {
                    moveScore += 30; // Arbitrary score for giving check
                }
                if (moveScore > 0) {
                    scoredMoves.push_back(std::make_pair(move, moveScore));
                }
            }
        }
    }
    
    std::cout << "7" << std::endl;
    // Sort moves based on their scores
    std::sort(scoredMoves.begin(), scoredMoves.end(), 
        [](const std::pair<Move, int>& a, const std::pair<Move, int>& b) {
            return a.second > b.second; // Sort in descending order of score
        }
    );

    // Extract the moves from scoredMoves, now sorted by their priority
    std::vector<Move> prioritizedMoves;
    for (const auto& scoredMove : scoredMoves) {
        prioritizedMoves.push_back(scoredMove.first);
    }

    // If there are prioritized moves, return them
    if (!prioritizedMoves.empty()) {
        return prioritizedMoves;
    }

    // If no prioritized moves are available, return all legal moves
    std::srand(std::time(nullptr)); // Seed for random generator
    std::random_shuffle(allMoves.begin(), allMoves.end());
    std::cout<<"shuffled" <<std::endl;
    return allMoves;
}


bool Level3::moveAvoidsCapture(const Move& move, Gamecontroller& gameController) {
    // Get the position of the piece after making the move
    Board store{*gameController.bd};
    //std::pair<int, int> newPosition = move.getNpo();
    
    // Get all possible moves of the opponent
    //vector<Move> opponentMoves;
    gameController.bd->movePiece(move.getNpo().first, move.getNpo().second, 
    gameController.bd->getPiece(move.getOpo().first, move.getOpo().second)); // we first move it
    gameController.changeTurn(); // change turn

    vector<unique_ptr<Piece>> alivePieces = gameController.alivePieces(); // Opponent alives
    for (const auto& piece : alivePieces) {
        vector<Move> possiblemoves = gameController.possiblemoves(piece); // each Opponent's piece move
        for (auto it: possiblemoves) {
            Board store2{*gameController.bd}; // store the changed board
            gameController.bd->movePiece(it.getNpo().first, it.getNpo().second, 
            gameController.bd->getPiece(it.getOpo().first, it.getOpo().second)); // move
            
            if (gameController.bd->getPiece(move.getNpo().first, move.getNpo().second)->isEmptyPiece()) { // the piece has moved is captured
                gameController.bd = make_unique<Board>(store); // recover the first stored board
                gameController.changeTurn(); // change back turn
                return true;
            }
            
        }
    }
        gameController.bd = make_unique<Board>(store); // recover the first stored board
        gameController.changeTurn(); // change back turn
        return false;
    // Generate all legal moves for alive pieces of the current player's color
    // for (const auto& piece : alivePieces) {
    //     if (piece->isWhiteSide() != isWhite) {
    //         vector<Move> pieceMoves = gameController.possiblemoves(piece, board);
    //         opponentMoves.insert(allMoves.end(), pieceMoves.begin(), pieceMoves.end());
    //     }
    // }

    // Check if any opponent's move can capture the piece at the new position
    // for (const auto& opponentMove : opponentMoves) {
    //     if (opponentMove.getNpo() == newPosition) {
    //         return false; // The piece can be captured at the new position
    //     }
    // }

    //return true; // The piece is safe from capture at the new position
}



bool Level3::moveCapturesOpponent(const Move& move, const Board& board) {
    // Check if the target position has an opponent's piece
    unique_ptr<Piece> targetPiece = board.getPiece(move.getNpo().first, move.getNpo().second);
    if (targetPiece && targetPiece->isWhiteTurn() != board.getPiece(move.getOpo().first, move.getOpo().second)->isWhiteTurn()) {
        return true; // The move captures an opponent's piece
    }
    return false; // No capture
}


bool Level3::moveGivesCheck(const Move& move, Gamecontroller& gameController) {
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
