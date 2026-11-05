#include "level4.h"
#include "gamecontroller.h"
#include "openingbook.h"
#include <algorithm>
#include <cctype>
#include <cstdlib>
#include <sstream>

namespace {
const int INF = 1000000;
const int MATE = 100000;
enum : uint8_t { TT_NONE = 0, TT_EXACT, TT_LOWER, TT_UPPER };

uint16_t encode(const Move &m) {
  if (m.isNull()) return 0;
  int from = m.getOpo().first * 8 + m.getOpo().second;
  int to = m.getNpo().first * 8 + m.getNpo().second;
  int promo = 0;
  if (m.ispromoted()) {
    switch (std::tolower(static_cast<unsigned char>(m.gettype2()))) {
    case 'q': promo = 1; break;
    case 'r': promo = 2; break;
    case 'b': promo = 3; break;
    case 'n': promo = 4; break;
    }
  }
  return static_cast<uint16_t>(1 | (from << 1) | (to << 7) | (promo << 13));
}

inline int sqIndex(std::pair<int, int> p) { return p.first * 8 + p.second; }

bool hasNonPawnMaterial(const Board &bd, bool white) {
  for (int r = 0; r < 8; ++r)
    for (int c = 0; c < 8; ++c) {
      char k = bd.keyAt(r, c);
      if (k == 'e' || (std::isupper(static_cast<unsigned char>(k)) != 0) != white) continue;
      char l = static_cast<char>(std::tolower(static_cast<unsigned char>(k)));
      if (l == 'n' || l == 'b' || l == 'r' || l == 'q') return true;
    }
  return false;
}

int toTT(int s, int ply) {
  if (s > MATE - Level4::MAX_PLY) return s + ply;
  if (s < -MATE + Level4::MAX_PLY) return s - ply;
  return s;
}
int fromTT(int s, int ply) {
  if (s > MATE - Level4::MAX_PLY) return s - ply;
  if (s < -MATE + Level4::MAX_PLY) return s + ply;
  return s;
}
} // namespace

Level4::Level4(int timeMs, bool useBook)
    : tt(1u << 20), timeMs{timeMs}, useBook{useBook} {
  for (auto &h : historyTable)
    for (int &v : h) v = 0;
  for (auto &v : moveStack) v.reserve(128);
  for (auto &v : scoreStack) v.reserve(128);
}

void Level4::checkTime() {
  if (std::chrono::steady_clock::now() >= deadline) stop = true;
}

void Level4::scoreMoves(const Board &, int ply, uint16_t ttMove) {
  const auto &moves = moveStack[ply];
  auto &scores = scoreStack[ply];
  scores.resize(moves.size());
  for (size_t i = 0; i < moves.size(); ++i) {
    const Move &m = moves[i];
    int s;
    if (ttMove && encode(m) == ttMove) {
      s = 10000000;
    } else if (m.iscapture() || m.ispromoted()) {
      s = 1000000 + pieceValue(m.gettype3()) * 16 - pieceValue(m.gettype1()) / 10;
      if (m.ispromoted()) s += pieceValue(m.gettype2()) * 16;
    } else if (ply < MAX_PLY && m == killers[ply][0]) {
      s = 900000;
    } else if (ply < MAX_PLY && m == killers[ply][1]) {
      s = 800000;
    } else {
      s = historyTable[sqIndex(m.getOpo())][sqIndex(m.getNpo())];
    }
    scores[i] = s;
  }
}

void Level4::pickNext(int ply, size_t i) {
  auto &moves = moveStack[ply];
  auto &scores = scoreStack[ply];
  size_t best = i;
  for (size_t j = i + 1; j < moves.size(); ++j)
    if (scores[j] > scores[best]) best = j;
  if (best != i) {
    std::swap(moves[i], moves[best]);
    std::swap(scores[i], scores[best]);
  }
}

int Level4::quiesce(Board &bd, int alpha, int beta, int ply) {
  if ((++nodes & 2047) == 0) checkTime();
  if (stop) return 0;
  if (ply >= MAX_PLY) return evaluate(bd);

  const bool inCheck = bd.inCheck();
  int best;
  if (inCheck) {
    best = -MATE + ply;
  } else {
    int stand = evaluate(bd);
    if (stand >= beta) return stand;
    if (stand > alpha) alpha = stand;
    best = stand;
  }

  auto &moves = moveStack[ply];
  moves.clear();
  bd.pseudoLegalMoves(moves, !inCheck); // all evasions when in check
  scoreMoves(bd, ply, 0);
  const bool side = bd.whiteToMove();
  int legal = 0;
  for (size_t i = 0; i < moves.size(); ++i) {
    pickNext(ply, i);
    const Move m = moves[i];
    // Delta pruning: even winning this piece can't raise alpha.
    if (!inCheck && !m.ispromoted() && best + pieceValue(m.gettype3()) + 200 < alpha) continue;
    bd.makeMove(m);
    if (bd.inCheck(side)) {
      bd.undoMove();
      continue;
    }
    ++legal;
    int s = -quiesce(bd, -beta, -alpha, ply + 1);
    bd.undoMove();
    if (stop) return 0;
    if (s > best) {
      best = s;
      if (s > alpha) {
        alpha = s;
        if (alpha >= beta) break;
      }
    }
  }
  if (inCheck && legal == 0) return -MATE + ply;
  return best;
}

int Level4::search(Board &bd, int depth, int alpha, int beta, int ply, bool allowNull) {
  if ((++nodes & 2047) == 0) checkTime();
  if (stop) return 0;
  if (bd.halfmoveClock() >= 100 || bd.isRepetition() || bd.insufficientMaterial()) return 0;

  // Mate-distance pruning
  alpha = std::max(alpha, -MATE + ply);
  beta = std::min(beta, MATE - ply - 1);
  if (alpha >= beta) return alpha;

  const bool inCheck = bd.inCheck();
  if (inCheck) ++depth; // check extension
  if (depth <= 0) return quiesce(bd, alpha, beta, ply);
  if (ply >= MAX_PLY - 1) return evaluate(bd);

  const bool pvNode = beta - alpha > 1;
  const uint64_t key = bd.hash();
  TTEntry &e = tt[key & (tt.size() - 1)];
  uint16_t ttMove = 0;
  if (e.key == key) {
    ttMove = e.move;
    if (!pvNode && e.depth >= depth) {
      int s = fromTT(e.score, ply);
      if (e.flag == TT_EXACT || (e.flag == TT_LOWER && s >= beta) ||
          (e.flag == TT_UPPER && s <= alpha))
        return s;
    }
  }

  const bool side = bd.whiteToMove();
  if (!pvNode && !inCheck) {
    const int staticEval = evaluate(bd);
    // Reverse futility pruning
    if (depth <= 3 && staticEval - 120 * depth >= beta && std::abs(beta) < MATE - MAX_PLY)
      return staticEval;
    // Null-move pruning
    if (allowNull && depth >= 3 && staticEval >= beta && hasNonPawnMaterial(bd, side)) {
      const int R = 2 + depth / 6;
      bd.makeNullMove();
      int s = -search(bd, depth - 1 - R, -beta, -beta + 1, ply + 1, false);
      bd.undoNullMove();
      if (stop) return 0;
      if (s >= beta) return s >= MATE - MAX_PLY ? beta : s;
    }
  }

  auto &moves = moveStack[ply];
  moves.clear();
  bd.pseudoLegalMoves(moves);
  scoreMoves(bd, ply, ttMove);

  int best = -INF, legal = 0;
  const int origAlpha = alpha;
  Move bestMove;
  for (size_t i = 0; i < moves.size(); ++i) {
    pickNext(ply, i);
    const Move m = moves[i];
    const bool quiet = !m.iscapture() && !m.ispromoted();
    bd.makeMove(m);
    if (bd.inCheck(side)) {
      bd.undoMove();
      continue;
    }
    ++legal;
    const bool givesCheck = bd.inCheck();
    int s;
    if (legal == 1) {
      s = -search(bd, depth - 1, -beta, -alpha, ply + 1, true);
    } else {
      // Late-move reductions for quiet moves ordered late
      int r = 0;
      if (depth >= 3 && quiet && !inCheck && !givesCheck && legal > 3) {
        r = 1 + (legal > 10 ? 1 : 0) + (depth >= 7 ? 1 : 0);
        if (pvNode) --r;
        if (m == killers[ply][0] || m == killers[ply][1]) --r;
        r = std::max(0, std::min(r, depth - 2));
      }
      s = -search(bd, depth - 1 - r, -alpha - 1, -alpha, ply + 1, true);
      if (!stop && s > alpha && r > 0) s = -search(bd, depth - 1, -alpha - 1, -alpha, ply + 1, true);
      if (!stop && s > alpha && s < beta) s = -search(bd, depth - 1, -beta, -alpha, ply + 1, true);
    }
    bd.undoMove();
    if (stop) return 0;

    if (s > best) {
      best = s;
      bestMove = m;
      if (s > alpha) {
        alpha = s;
        if (alpha >= beta) {
          if (quiet) {
            if (!(m == killers[ply][0])) {
              killers[ply][1] = killers[ply][0];
              killers[ply][0] = m;
            }
            int &h = historyTable[sqIndex(m.getOpo())][sqIndex(m.getNpo())];
            h += depth * depth;
            if (h > 400000)
              for (auto &row : historyTable)
                for (int &v : row) v /= 2;
          }
          break;
        }
      }
    }
  }

  if (legal == 0) return inCheck ? -MATE + ply : 0; // checkmate / stalemate

  e.key = key;
  e.depth = static_cast<int16_t>(depth);
  e.move = encode(bestMove);
  e.score = toTT(best, ply);
  e.flag = best <= origAlpha ? TT_UPPER : best >= beta ? TT_LOWER : TT_EXACT;
  return best;
}

Move Level4::think(Board &bd, int ms, int maxDepth) {
  const auto start = std::chrono::steady_clock::now();
  deadline = start + std::chrono::milliseconds(ms);
  nodes = 0;
  stop = false;
  for (auto &k : killers) k[0] = k[1] = Move{};
  for (auto &row : historyTable)
    for (int &v : row) v /= 8;

  std::vector<Move> root = bd.legalMoves();
  if (root.empty()) {
    info = "no legal moves";
    return Move{};
  }
  if (root.size() == 1) {
    info = "only legal move";
    return root[0];
  }

  // Start with the hash move (if any) and captures first.
  {
    const TTEntry &e = tt[bd.hash() & (tt.size() - 1)];
    const uint16_t ttMove = e.key == bd.hash() ? e.move : 0;
    std::stable_sort(root.begin(), root.end(), [&](const Move &a, const Move &b) {
      auto key = [&](const Move &m) {
        if (ttMove && encode(m) == ttMove) return 1 << 30;
        return pieceValue(m.gettype3()) * 16 - pieceValue(m.gettype1()) / 10 +
               (m.ispromoted() ? pieceValue(m.gettype2()) * 16 : 0);
      };
      return key(a) > key(b);
    });
  }

  std::vector<int> rootScores(root.size(), -INF);
  Move best = root[0];
  int bestScore = 0, completed = 0;

  for (int depth = 1; depth <= maxDepth; ++depth) {
    int alpha = -INF;
    const int beta = INF;
    int iterBest = -INF;
    Move iterMove;
    bool any = false;
    std::fill(rootScores.begin(), rootScores.end(), -INF);

    for (size_t i = 0; i < root.size(); ++i) {
      bd.makeMove(root[i]);
      int s;
      if (i == 0) {
        s = -search(bd, depth - 1, -beta, -alpha, 1, true);
      } else {
        s = -search(bd, depth - 1, -alpha - 1, -alpha, 1, true);
        if (!stop && s > alpha) s = -search(bd, depth - 1, -beta, -alpha, 1, true);
      }
      bd.undoMove();
      if (stop) break;
      rootScores[i] = s;
      if (s > iterBest) {
        iterBest = s;
        iterMove = root[i];
        any = true;
      }
      if (s > alpha) alpha = s;
    }

    if (stop) {
      // Moves searched before the timeout are reliable; the first one is the
      // previous best, so anything that beat it is an improvement.
      if (any) {
        best = iterMove;
        bestScore = iterBest;
      }
      break;
    }
    best = iterMove;
    bestScore = iterBest;
    completed = depth;

    // Re-order root moves: best first, then by score.
    std::vector<size_t> idx(root.size());
    for (size_t i = 0; i < idx.size(); ++i) idx[i] = i;
    std::stable_sort(idx.begin(), idx.end(),
                     [&](size_t a, size_t b) { return rootScores[a] > rootScores[b]; });
    std::vector<Move> reordered;
    for (size_t i : idx) reordered.push_back(root[i]);
    root.swap(reordered);

    if (bestScore >= MATE - depth) break; // found a forced mate
    const auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
                             std::chrono::steady_clock::now() - start).count();
    if (elapsed > ms / 2) break; // next iteration would not finish in time
  }

  const int whiteView = bd.whiteToMove() ? bestScore : -bestScore;
  std::ostringstream os;
  os << "depth " << std::max(completed, 1) << ", eval " << formatScore(whiteView);
  if (nodes > 0) os << ", " << nodes / 1000 << "k nodes";
  info = os.str();
  return best;
}

Move Level4::calculateBestMove(const Gamecontroller &gameController) {
  Board board{gameController.getBoard()};
  if (useBook && gameController.isStandardStart()) {
    const std::string bookMove =
        OpeningBook::instance().lookup(gameController.playedMoves(), rng);
    if (!bookMove.empty()) {
      for (const auto &m : board.legalMoves())
        if (m.toString() == bookMove) {
          info = "opening book";
          return m;
        }
    }
  }
  int ms = gameController.thinkTime() > 0 ? gameController.thinkTime() : timeMs;
  return think(board, ms);
}
