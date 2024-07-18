// Player.cc
#include "player.h"
#include <vector>
#include <iostream>
#include <sstream>


#include "player.h"

// Constructor implementation
//Player::Player(int id, const std::string& name, bool isWhite) : id(id), name(name), isWhite(isWhite), score(0), status("active") {}
//Player::Player(){}

//void Player::resignGame(Gamecontroller& gameController) {
//    gameController.handleResignation(this);
//
//}


// Getters

int Player::getId() const {
    return id;
}

//std::string Player::getName() const {
//    return name;
//}
//
//int Player::getScore() const {
//    return score;
//}
//
//bool Player::getIsWhite() const {
//    return isWhite;
//}
//
//std::string Player::getStatus() const {
//    return status;
//}

// Setters


void Player::setId(int newId) {
    id = newId;
}

//void Player::setName(const std::string& newName) {
//    name = newName;
//}
//
//void Player::setScore(int newScore) {
//    score = newScore;
//}
//
//void Player::setIsWhite(bool newIsWhite) {
//    isWhite = newIsWhite;
//}
//
//void Player::setStatus(const std::string& newStatus) {
//    status = newStatus;
//}
