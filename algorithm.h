#ifndef _ALGORITHM_H
#define _ALGORITHM_H

#include "board.h"
#include "move.h"
#include <random>
#include <string>
#include <vector>

class Gamecontroller;

// Strategy interface for computer players (Level1 ... Level4).
class Algorithm {
protected:
  std::mt19937 rng;
  std::string info; // short description of the last decision (depth, eval...)

public:
  Algorithm();
  virtual ~Algorithm() = default;

  // Pick a legal move for the side to move in gc's current position.
  virtual Move calculateBestMove(const Gamecontroller &gameController) = 0;
  const std::string &lastInfo() const { return info; }

  // ---------- shared helpers ----------
  Move selectRandomMove(const std::vector<Move> &moves);
  static int pieceValue(char pieceType); // centipawns, king = 0
  static bool givesCheck(Board &bd, const Move &m);
  static bool isCheckmateMove(Board &bd, const Move &m);
  // Static evaluation from the point of view of the side to move.
  static int evaluate(const Board &bd);
  // Formats a score given from White's point of view ("+0.35", "White mates in 3").
  static std::string formatScore(int centipawns);
};

#endif // _ALGORITHM_H
