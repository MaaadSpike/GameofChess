#ifndef PLAYER_H
#define PLAYER_H

#include <string>
#include "board.h"
#include "gamecontroller.h"
#include "move.h"

class Player {
protected:
    int id;
    //std::string name;
    //int score;
    bool isWhite;
    //std::string status;

public:
    Player(int id ,bool isWhite): id{id}, isWhite{isWhite} {}
    //Player(int id, const std::string& name, bool isWhite); // Constructor

    // Virtual functions
    //virtual bool makeMove(Board& board, const Move& move) = 0;
    //virtual void resignGame(Gamecontroller& gameController) = 0;
    virtual void setDifficulty(const std::string& level) = 0;
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

#endif // PLAYER_H
