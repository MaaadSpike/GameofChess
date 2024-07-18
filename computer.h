//computer.h

#include "player.h"
#include "algorithm.h"
#include "gamecontroller.h"
#include <string>

// Forward declaration
class GameController;

class Computer : public Player {
private:
    std::string difficultyLevel;
    Algorithm* algorithm; // Pointer to an instance of a class derived from Algorithm
    
public:
    // Constructor
    //Computer();
    Computer(int id, bool isWhite, const std::string& difficultyLevel) : Player(id, isWhite), difficultyLevel(difficultyLevel), algorithm(nullptr) {
    setDifficulty(difficultyLevel);
}

    // Override the virtual makeMove method from Player
    //virtual bool makeMove(GameController& gameController, const Move& move) override;

    // Set the difficulty level and appropriate algorithm
    void setDifficulty(const std::string& level) override;

    //funciton that decides the move ultimately:
    Move decideMove(Gamecontroller& gameController);

    // Destructor
    //virtual ~Computer();
    ~Computer();
    // Additional methods as needed
};
