#include "level3.h"
#include "gamecontroller.h"
#include <algorithm>

namespace {
const int INF = 1000000;
const int MATE = 100000;

int mvvLva(const Move &m) {
  return Algorithm::pieceValue(m.gettype3()) * 10 - Algorithm::pieceValue(m.gettype1()) / 10 +
         (m.ispromoted() ? Algorithm::pieceValue(m.gettype2()) * 10 : 0);
}
} // namespace

int Level3::quiesce(Board &bd, int alpha, int beta, int ply) {
  int stand = evaluate(bd);
  if (stand >= beta || ply > 12) return stand;
  if (stand > alpha) alpha = stand;

  std::vector<Move> moves;
  bd.pseudoLegalMoves(moves, true);
  std::sort(moves.begin(), moves.end(),
            [](const Move &a, const Move &b) { return mvvLva(a) > mvvLva(b); });
  const bool side = bd.whiteToMove();
  for (const auto &m : moves) {
    bd.makeMove(m);
    if (bd.inCheck(side)) {
      bd.undoMove();
      continue;
    }
    int s = -quiesce(bd, -beta, -alpha, ply + 1);
    bd.undoMove();
    if (s >= beta) return s;
    if (s > alpha) alpha = s;
  }
  return alpha;
}

// Opponent to move: best reply followed by quiescence.
int Level3::reply(Board &bd, int alpha, int beta) {
  if (bd.halfmoveClock() >= 100 || bd.isRepetition() || bd.insufficientMaterial()) return 0;
  std::vector<Move> moves = bd.legalMoves();
  if (moves.empty()) return bd.inCheck() ? -MATE : 0;
  int best = -INF;
  for (const auto &m : moves) {
    bd.makeMove(m);
    int s = -quiesce(bd, -beta, -alpha, 0);
    bd.undoMove();
    if (s > best) best = s;
    if (s > alpha) alpha = s;
    if (alpha >= beta) break;
  }
  return best;
}

Move Level3::calculateBestMove(const Gamecontroller &gameController) {
  Board board{gameController.getBoard()};
  std::vector<Move> moves = board.legalMoves();
  if (moves.empty()) return Move{};

  std::uniform_int_distribution<int> noise(-12, 12);
  int bestScore = -INF;
  Move best = moves.front();
  for (const auto &m : moves) {
    board.makeMove(m);
    int score;
    if (board.inCheck() && !board.hasLegalMove()) {
      score = MATE; // checkmate now
    } else {
      score = -reply(board, -INF, INF);
      if (board.inCheck()) score += 15; // likes checks
    }
    board.undoMove();
    score += noise(rng);
    if (score > bestScore) {
      bestScore = score;
      best = m;
    }
  }
  info = bestScore >= MATE - 100 ? "checkmate!" : "2-ply search, eval " + formatScore(board.whiteToMove() ? bestScore : -bestScore);
  return best;
}
