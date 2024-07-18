//level1
#include "level1.h"
#include <cstdlib> 
#include <ctime>   
#include <vector>
#include <algorithm> 

using namespace std;


vector<Move> Level1::calculateBestMove(Gamecontroller& gameController, bool isWhite) {
    vector<Move> allMoves;
    vector<Move> responsiveMoves;
    vector<unique_ptr<Piece>> alivePieces = gameController.alivePieces();
    
    std::cout << "16" << std::endl;
    // Check if the king is in check and generate responsive moves if so
    if (gameController.ischeck()) {
    std::cout << "19" << std::endl;
        responsiveMoves = generateCheckResponseMoves(alivePieces, gameController, isWhite);
        std::cout << "21" << std::endl;
        if (!responsiveMoves.empty()) {
            std::cout << "23" << std::endl;
            return responsiveMoves; // Return moves that respond to the check
        }
    }
    
    std::cout << "28" << std::endl;
    // Generate all legal moves for alive pieces of the current player's color
    for ( auto& piece : alivePieces) {
        std::cout << "31" << std::endl;
        if (piece->isWhiteTurn() == isWhite) {
            std::cout << "piece type: " << piece->key() << " " << piece->getCoord().first << piece->getCoord().second << std::endl;
            vector<Move> pieceMoves = gameController.possiblemoves(piece);
            std::cout << "35" << std::endl;
            allMoves.insert(allMoves.end(), pieceMoves.begin(), pieceMoves.end());
            std::cout << "37" << std::endl;
        }
    }
    std::cout << "40" << std::endl;
    // Return all legal moves if there are no responsive moves for a check
    return allMoves;
}
