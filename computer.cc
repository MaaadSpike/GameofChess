#include "computer.h"

#include "player.h"
#include "algorithm.h"
#include "level1.h" 
#include "level2.h"
#include "level3.h"
#include "gamecontroller.h"
#include <ctime>

// bool isvalidmove(const Move& m, const GameController & gameController) {
//     char c = gameController.getBoard().getPiece(m.getOpo().first, m.getOpo().second);

//     if (c == 'P' && m.getOpo().first == 3 && m.getNpo().first == 2 &&
//             (m.getNpo().second == m.getOpo().second + 1 || m.getNpo().second == m.getOpo().second - 1) &&
//             0 <= m.getNpo().second && m.getNpo().second <= 7 &&
//             lastmove.gettype1() == 'p' &&
//             lastmove.getOpo() == std::pair<int,int>{m.getNpo().first-1,m.getNpo().second} &&
//             lastmove.getNpo() == std::pair<int,int>{m.getNpo().first+1,m.getNpo().second}) {
//                 if (!gameController.dopossiblemoves2(m.getOpo(), m.getNpo())) { // if this move makes player in check
//                     //std::cout << "it is not a wide choice(You will be in check)" << std::endl;
//                     return false;
//                 }  
//             return true;
//     }
//     if (c == 'p' && m.getOpo().first == 4 && m.getNpo().first == 5 &&
//         (m.getNpo().second == m.getOpo().second + 1 || m.getNpo().second == m.getOpo().second - 1) &&
//         0 <= m.getNpo().second && m.getNpo().second <= 7 &&
//         lastmove.gettype1() == 'P' &&
//         lastmove.getOpo() == std::pair<int,int>{m.getNpo().first+1,m.getNpo().second} &&
//         lastmove.getNpo() == std::pair<int,int>{m.getNpo().first-1,m.getNpo().second}) { // black
//             if (!gameController.dopossiblemoves2(m.getOpo(), m.getNpo())) { // if this move makes player in check
//                 //std::cout << "it is not a wide choice(You will be in check)" << std::endl;
//                 return false;
//             }
//             return true;
//         }
//     if ((!bd->getPiece(m.getOpo().first, m.getOpo().second)->isMove(m.getOpo(), m.getNpo(), *bd))) { // check validity
//             //std::cerr << "Invalid move!" << std::endl;
//             return false;
//     }
//     return true;
// }
// Computer::Computer(const std::string& level) : Player(), difficultyLevel(level) {
//      setDifficulty(level);
// }
//Computer::Computer();

Computer::~Computer() {
    delete this->algorithm;
}

void Computer::setDifficulty(const std::string& level) {
    difficultyLevel = level;
    // Instantiate the appropriate Algorithm based on difficulty level
    if (level == "level1") {
        algorithm = new Level1();
    } else if (level == "level2") {
        algorithm = new Level2();
    } else if (level == "level3") {
        algorithm = new Level3();
    }
}
// A method to decide the best move
Move Computer::decideMove(Gamecontroller& gameController) {
    bool isWhite = false;
    if (gameController.curTurn() == "White") {
        isWhite = true;
    } 
    std::vector<Move> moves = algorithm->calculateBestMove(gameController, isWhite);
    // Handle no valid moves found
    if (moves.empty()) {
        throw std::runtime_error("No valid moves available");
    }
    // If the difficulty level is Level 3, apply the top move directly
    if (difficultyLevel == "level3") {
        for (auto& move : moves) {
            if (gameController.makeMove(move)) {
                return move;
            }
        }
    }
    // For Level 1 and Level 2, scramble the list of moves
    std::srand(std::time(nullptr)); // Seed for random generator
    std::random_shuffle(moves.begin(), moves.end());
    // Apply the first valid move from the scrambled list
    for (auto& move : moves) {
        if (gameController.makeMove(move)) {
            return move;
        }
    }
    // If no move could be made, throw an exception
    throw std::runtime_error("No valid moves could be applied");
}



// bool Computer::makeMove(Gamecontroller& gameController) {
//     Board& board = gameController.getBoard();
//     Move move = algorithm->calculateBestMove(board, isWhite);
//     return board.makeMove(move);
// }
