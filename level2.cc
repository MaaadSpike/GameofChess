#include "level2.h"
#include "gamecontroller.h"

Move Level2::calculateBestMove(const Gamecontroller &gameController) {
  Board board{gameController.getBoard()};
  std::vector<Move> moves = board.legalMoves();
  if (moves.empty()) return Move{};

  int bestScore = -1;
  std::vector<Move> best;
  for (const auto &m : moves) {
    int score = 0;
    if (m.iscapture()) score += 10 + pieceValue(m.gettype3()) / 10;
    if (m.ispromoted()) score += 10 + pieceValue(m.gettype2()) / 10;
    board.makeMove(m);
    if (board.inCheck()) {
      score += board.hasLegalMove() ? 15 : 10000; // check / checkmate
    }
    board.undoMove();
    if (score > bestScore) {
      bestScore = score;
      best.clear();
    }
    if (score == bestScore) best.push_back(m);
  }
  info = bestScore >= 10000 ? "checkmate!" : bestScore > 0 ? "capture/check" : "random move";
  return selectRandomMove(best);
}
