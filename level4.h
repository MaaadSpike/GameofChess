#ifndef LEVEL4_H
#define LEVEL4_H

#include "algorithm.h"
#include <chrono>
#include <cstdint>
#include <vector>

// Level 4: a real chess engine.
//  * opening book (weighted tree, see openingbook.h)
//  * iterative-deepening negamax alpha-beta with principal variation search
//  * transposition table (Zobrist hashing)
//  * quiescence search on captures, check extension
//  * move ordering: hash move, MVV-LVA captures, killer moves, history heuristic
//  * null-move pruning, late-move reductions, reverse futility pruning
//  * time-limited: thinks for a fixed budget per move
class Level4 : public Algorithm {
public:
  static constexpr int MAX_PLY = 64;

private:
  struct TTEntry {
    uint64_t key = 0;
    int score = 0;
    int16_t depth = -1;
    uint8_t flag = 0;
    uint16_t move = 0;
  };
  std::vector<TTEntry> tt;
  Move killers[MAX_PLY][2];
  int historyTable[64][64];
  std::vector<Move> moveStack[MAX_PLY + 1];
  std::vector<int> scoreStack[MAX_PLY + 1];

  int timeMs;
  bool useBook;
  long long nodes = 0;
  bool stop = false;
  std::chrono::steady_clock::time_point deadline;

  int search(Board &bd, int depth, int alpha, int beta, int ply, bool allowNull);
  int quiesce(Board &bd, int alpha, int beta, int ply);
  void scoreMoves(const Board &bd, int ply, uint16_t ttMove);
  void pickNext(int ply, size_t i);
  void checkTime();

public:
  explicit Level4(int timeMs = 1200, bool useBook = true);
  Move calculateBestMove(const Gamecontroller &gameController) override;
  // Search a position directly (used for hints and tests).
  Move think(Board &bd, int timeMs, int maxDepth = MAX_PLY - 1);
  void setTime(int ms) { timeMs = ms; }
};

#endif // LEVEL4_H
