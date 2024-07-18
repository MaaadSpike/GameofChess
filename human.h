#ifndef HUMAN_H
#define HUMAN_H

#include "player.h"
#include "gamecontroller.h"
#include "move.h"
#include <string>

class Human : public Player {
public:
    //Human();
    Human(int id, bool isWhite): Player{id, isWhite} {}

    // Implement the resignGame function from the Player class
    //void resignGame(Gamecontroller& gameController) override;
    void setDifficulty(const std::string& level) override;
    //check move
    Move checkMove(Gamecontroller& gameController, Move& move);

    // Additional methods as needed
    // Getters

    int getId() const;
    //std::string getName() const;
    //int getScore() const;
    //bool getIsWhite() const;
    //std::string getStatus() const;

    // Setters

    void setId(int newId);
    //void setName(const std::string& newName);
    //void setScore(int newScore);
    //void setIsWhite(bool newIsWhite);
    //void setStatus(const std::string& newStatus);
};

#endif // HUMAN_H
