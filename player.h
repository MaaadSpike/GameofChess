#ifndef PLAYER_H
#define PLAYER_H

#include <string>

// Abstract player. id 0 = human, 1..4 = computer difficulty level.
class Player {
protected:
  int id;
  bool isWhite;

public:
  Player(int id, bool isWhite) : id{id}, isWhite{isWhite} {}
  virtual ~Player() = default;

  virtual bool isHuman() const = 0;
  virtual void setDifficulty(const std::string &level) = 0;
  virtual std::string describe() const = 0; // "human", "computer3"

  int getId() const { return id; }
  void setId(int newId) { id = newId; }
  bool getIsWhite() const { return isWhite; }
};

#endif // PLAYER_H
