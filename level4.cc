#include "level4.h"
#include "move.h"
#include "gamecontroller.h"
#include <algorithm> // For std::sort
#include <iostream>
#include <ctime>
std::vector<Move> Level4::calculateBestMove(Gamecontroller& gameController, bool isWhite) {
    std::vector<Move> allMoves;
    std::vector<Move> responsiveMoves;
    std::vector<std::pair<Move, int>> scoredMoves; // Pair of Move and its score

    std::vector<std::unique_ptr<Piece>> alivePieces = gameController.alivePieces();
    Board board{*gameController.bd};
    std::cout << "1" << std::endl;

    // Check if the king is in check
    if (gameController.ischeck()) {
        responsiveMoves = generateCheckResponseMoves(alivePieces, gameController, isWhite);
        if (!responsiveMoves.empty()) {
            return responsiveMoves; // Return moves that respond to the check
        }
    }

    // Generate all possible moves
    for ( auto& piece : alivePieces) {
         std::cout << "all possible" << std::endl;
        if (piece->isWhiteTurn() == isWhite) {
            std::vector<Move> pieceMoves = gameController.possiblemoves(piece);
            allMoves.insert(allMoves.end(), pieceMoves.begin(), pieceMoves.end());

            // Evaluate and prioritize moves
            for (const auto& move : pieceMoves) {
                std::cout << "proir" << std::endl;
                int moveScore = 0;
                int captureScore = moveCapturesOpponent(move, board);
                //int captureScore = 0;

                if (moveAvoidsCapture(move, gameController)) {
                    std::cout << "proir cap" << std::endl;
                    moveScore += 10;
                }
                if (captureScore > 0) { // Only add score if capturing more valuable piece
                    std::cout << "cap score" << std::endl;
                    moveScore += captureScore + 20;
                }
                if (moveGivesCheck(move, gameController)) {
                    std::cout << "move gives check" << std::endl;
                    moveScore += 30;
                }

                if (moveScore > 0) {
                    std::cout << "move score > 0 "<< std::endl;
                    scoredMoves.push_back(std::make_pair(move, moveScore));
                    std::cout << "move score > 0 end"<< std::endl;
                }
            }
        }
    }

    std::cout << "finish loop"<< std::endl;

    // Sort moves based on their scores
    std::sort(scoredMoves.begin(), scoredMoves.end(), 
        [](const std::pair<Move, int>& a, const std::pair<Move, int>& b) {
            return a.second > b.second; // Sort in descending order of score
        }
    );
    std::cout << "prioritize move"<< std::endl;
    // Extract the moves from scoredMoves
    std::vector<Move> prioritizedMoves;
    for (const auto& scoredMove : scoredMoves) {
        prioritizedMoves.push_back(scoredMove.first);
    }
    std::cout << "check prioritize move"<< std::endl;
    // If there are prioritized moves, return them
    if (!prioritizedMoves.empty()) {
        std::cout << "return prior"<< std::endl;
        return prioritizedMoves;
    }

    // If no prioritized moves are available, return all legal moves
    std::cout << "returning"<< std::endl;
    return allMoves;
}

int Level4::moveCapturesOpponent(const Move& move, const Board& board) {
    std::cout << "capture opponent" << std::endl;
    
    auto targetPiece = board.getPiece(move.getNpo().first, move.getNpo().second);
    auto movingPiece = board.getPiece(move.getOpo().first, move.getOpo().second);

    if (targetPiece && !targetPiece->isEmptyPiece() && movingPiece) {
        std::cout << "capture opponent in if" << std::endl;
        // Ensure the pieces belong to opposing sides
        if (targetPiece->isWhiteTurn() != movingPiece->isWhiteTurn()) {
            int targetValue = getPieceValue(targetPiece->key());
            int movingValue = getPieceValue(movingPiece->key());

            // The score is higher if a less valuable piece captures a more valuable one
            return std::max(targetValue - movingValue, 0);
        }
    }
    return 0;
}

bool Level4::moveAvoidsCapture(const Move& move, Gamecontroller& gameController) {
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
    return false;
}
            
       

bool Level4::moveGivesCheck(const Move& move, Gamecontroller& gameController) {
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
// Additional methods as needed



int Level4::getPieceValue(char pieceType) {
    switch (pieceType) {
        case 'P': case 'p': return 1; // Pawn
        case 'N': case 'n': return 3; // Knight
        case 'B': case 'b': return 3; // Bishop
        case 'R': case 'r': return 5; // Rook
        case 'Q': case 'q': return 9; // Queen
        case 'K': case 'k': return 0; // King (shouldn't be captured)
        default: return 0;  // Unknown piece type
    }
}
