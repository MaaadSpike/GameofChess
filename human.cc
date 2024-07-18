#include "human.h"

//Human::Human(int id, const std::string& name, bool isWhite)
//    : Player(id, name, isWhite) {}
//Human::Human();

void Human::setDifficulty(const std::string& level) {
    return;
}
//void Human::resignGame(Gamecontroller& gameController) {
//    // Set the player's status to indicate a resignation (or loss)
//    setStatus("lost");
//
//    // Additional logic for resigning can be added here
//    // For example, notifying the gameController about the resignation
//}

Move Human::checkMove(Gamecontroller& gameController, Move& move) {
    // Assume move is already obtained from user input
    // Check and execute the move using the game controller
    if (gameController.makeMove(move)) {
        return move; // Return the move if it's valid
    } else {
        throw std::invalid_argument("Invalid move attempted.");
    }
}

// Getters

int Human::getId() const {
    return id;
}

//std::string Human::getName() const {
//    return name;
//}
//
//int Human::getScore() const {
//    return score;
//}
//
//bool Human::getIsWhite() const {
//    return isWhite;
//}
//
//std::string Human::getStatus() const {
//    return status;
//}

// Setters
void Human::setId(int newId) {
    id = newId;
}

//void Human::setName(const std::string& newName) {
//    name = newName;
//}
//
//void Human::setScore(int newScore) {
//    score = newScore;
//}
//
//void Human::setIsWhite(bool newIsWhite) {
//    isWhite = newIsWhite;
//}
//
//void Human::setStatus(const std::string& newStatus) {
//    status = newStatus;
//}
