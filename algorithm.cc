#include "algorithm.h"
#include <algorithm>
#include <cctype>
#include <cstdio>
#include <cstdlib>

namespace {

// Piece-square tables (centipawns), written from White's point of view with
// index 0 = a8 ... 63 = h1, i.e. exactly our (row * 8 + col) layout.
// Black pieces use the vertically mirrored square.
const int PAWN_PST[64] = {
     0,  0,  0,  0,  0,  0,  0,  0,
    50, 50, 50, 50, 50, 50, 50, 50,
    10, 10, 20, 30, 30, 20, 10, 10,
     5,  5, 10, 25, 25, 10,  5,  5,
     0,  0,  0, 20, 20,  0,  0,  0,
     5, -5,-10,  0,  0,-10, -5,  5,
     5, 10, 10,-20,-20, 10, 10,  5,
     0,  0,  0,  0,  0,  0,  0,  0};
const int KNIGHT_PST[64] = {
   -50,-40,-30,-30,-30,-30,-40,-50,
   -40,-20,  0,  0,  0,  0,-20,-40,
   -30,  0, 10, 15, 15, 10,  0,-30,
   -30,  5, 15, 20, 20, 15,  5,-30,
   -30,  0, 15, 20, 20, 15,  0,-30,
   -30,  5, 10, 15, 15, 10,  5,-30,
   -40,-20,  0,  5,  5,  0,-20,-40,
   -50,-40,-30,-30,-30,-30,-40,-50};
const int BISHOP_PST[64] = {
   -20,-10,-10,-10,-10,-10,-10,-20,
   -10,  0,  0,  0,  0,  0,  0,-10,
   -10,  0,  5, 10, 10,  5,  0,-10,
   -10,  5,  5, 10, 10,  5,  5,-10,
   -10,  0, 10, 10, 10, 10,  0,-10,
   -10, 10, 10, 10, 10, 10, 10,-10,
   -10,  5,  0,  0,  0,  0,  5,-10,
   -20,-10,-10,-10,-10,-10,-10,-20};
const int ROOK_PST[64] = {
     0,  0,  0,  0,  0,  0,  0,  0,
     5, 10, 10, 10, 10, 10, 10,  5,
    -5,  0,  0,  0,  0,  0,  0, -5,
    -5,  0,  0,  0,  0,  0,  0, -5,
    -5,  0,  0,  0,  0,  0,  0, -5,
    -5,  0,  0,  0,  0,  0,  0, -5,
    -5,  0,  0,  0,  0,  0,  0, -5,
     0,  0,  0,  5,  5,  0,  0,  0};
const int QUEEN_PST[64] = {
   -20,-10,-10, -5, -5,-10,-10,-20,
   -10,  0,  0,  0,  0,  0,  0,-10,
   -10,  0,  5,  5,  5,  5,  0,-10,
    -5,  0,  5,  5,  5,  5,  0, -5,
     0,  0,  5,  5,  5,  5,  0, -5,
   -10,  5,  5,  5,  5,  5,  0,-10,
   -10,  0,  5,  0,  0,  0,  0,-10,
   -20,-10,-10, -5, -5,-10,-10,-20};
const int KING_MG_PST[64] = {
   -30,-40,-40,-50,-50,-40,-40,-30,
   -30,-40,-40,-50,-50,-40,-40,-30,
   -30,-40,-40,-50,-50,-40,-40,-30,
   -30,-40,-40,-50,-50,-40,-40,-30,
   -20,-30,-30,-40,-40,-30,-30,-20,
   -10,-20,-20,-20,-20,-20,-20,-10,
    20, 20,  0,  0,  0,  0, 20, 20,
    20, 30, 10,  0,  0, 10, 30, 20};
const int KING_EG_PST[64] = {
   -50,-40,-30,-20,-20,-30,-40,-50,
   -30,-20,-10,  0,  0,-10,-20,-30,
   -30,-10, 20, 30, 30, 20,-10,-30,
   -30,-10, 30, 40, 40, 30,-10,-30,
   -30,-10, 30, 40, 40, 30,-10,-30,
   -30,-10, 20, 30, 30, 20,-10,-30,
   -30,-30,  0,  0,  0,  0,-30,-30,
   -50,-30,-30,-30,-30,-30,-30,-50};

const int PASSED_MG[8] = {0, 5, 10, 18, 30, 50, 80, 0};
const int PASSED_EG[8] = {0, 10, 20, 35, 60, 100, 150, 0};

inline int centerDistance(int r, int c) {
  int dr = r < 4 ? 3 - r : r - 4;
  int dc = c < 4 ? 3 - c : c - 4;
  return dr + dc; // 0 (centre) .. 6 (corner)
}

} // namespace

Algorithm::Algorithm() : rng{std::random_device{}()} {}

Move Algorithm::selectRandomMove(const std::vector<Move> &moves) {
  if (moves.empty()) return Move{};
  std::uniform_int_distribution<size_t> dist(0, moves.size() - 1);
  return moves[dist(rng)];
}

int Algorithm::pieceValue(char pieceType) {
  switch (std::tolower(static_cast<unsigned char>(pieceType))) {
  case 'p': return 100;
  case 'n': return 320;
  case 'b': return 330;
  case 'r': return 500;
  case 'q': return 900;
  default: return 0;
  }
}

bool Algorithm::givesCheck(Board &bd, const Move &m) {
  bd.makeMove(m);
  bool chk = bd.inCheck();
  bd.undoMove();
  return chk;
}

bool Algorithm::isCheckmateMove(Board &bd, const Move &m) {
  bd.makeMove(m);
  bool mate = bd.inCheck() && !bd.hasLegalMove();
  bd.undoMove();
  return mate;
}

std::string Algorithm::formatScore(int cp) {
  if (cp >= 90000 || cp <= -90000) {
    int plies = 100000 - std::abs(cp);
    int moves = (plies + 1) / 2;
    return std::string(cp > 0 ? "White" : "Black") + " mates in " + std::to_string(moves);
  }
  char buf[32];
  std::snprintf(buf, sizeof buf, "%+.2f", cp / 100.0);
  return buf;
}

// Tapered evaluation: material + piece-square tables + pawn structure +
// rook files + bishop pair + king shelter + endgame "mop-up" knowledge.
int Algorithm::evaluate(const Board &bd) {
  int mg[2] = {0, 0}, eg[2] = {0, 0}; // [0] black, [1] white
  int phase = 0;
  int bishops[2] = {0, 0};
  int nonPawn[2] = {0, 0};
  int pawnCount[2] = {0, 0};
  int pawnsOnFile[2][8] = {{0}};
  int minPawnRow[2][8], maxPawnRow[2][8];
  std::pair<int, int> king[2] = {{-1, -1}, {-1, -1}};
  for (int f = 0; f < 8; ++f) {
    minPawnRow[0][f] = minPawnRow[1][f] = 8;
    maxPawnRow[0][f] = maxPawnRow[1][f] = -1;
  }

  for (int r = 0; r < 8; ++r)
    for (int c = 0; c < 8; ++c) {
      char k = bd.keyAt(r, c);
      if (k == 'e') continue;
      const int side = std::isupper(static_cast<unsigned char>(k)) ? 1 : 0;
      const int sq = side ? r * 8 + c : (7 - r) * 8 + c;
      switch (std::tolower(static_cast<unsigned char>(k))) {
      case 'p':
        mg[side] += 100 + PAWN_PST[sq];
        eg[side] += 110 + PAWN_PST[sq] / 2;
        ++pawnCount[side];
        ++pawnsOnFile[side][c];
        minPawnRow[side][c] = std::min(minPawnRow[side][c], r);
        maxPawnRow[side][c] = std::max(maxPawnRow[side][c], r);
        break;
      case 'n':
        mg[side] += 320 + KNIGHT_PST[sq];
        eg[side] += 310 + KNIGHT_PST[sq];
        phase += 1;
        nonPawn[side] += 320;
        break;
      case 'b':
        mg[side] += 330 + BISHOP_PST[sq];
        eg[side] += 330 + BISHOP_PST[sq];
        phase += 1;
        ++bishops[side];
        nonPawn[side] += 330;
        break;
      case 'r':
        mg[side] += 500 + ROOK_PST[sq];
        eg[side] += 520 + ROOK_PST[sq];
        phase += 2;
        nonPawn[side] += 500;
        break;
      case 'q':
        mg[side] += 900 + QUEEN_PST[sq];
        eg[side] += 940 + QUEEN_PST[sq];
        phase += 4;
        nonPawn[side] += 900;
        break;
      case 'k':
        mg[side] += KING_MG_PST[sq];
        eg[side] += KING_EG_PST[sq];
        king[side] = {r, c};
        break;
      }
    }

  // Pawn structure: doubled, isolated, passed pawns.
  for (int side = 0; side < 2; ++side) {
    const int enemy = 1 - side;
    for (int f = 0; f < 8; ++f) {
      const int n = pawnsOnFile[side][f];
      if (n == 0) continue;
      if (n > 1) {
        mg[side] -= 12 * (n - 1);
        eg[side] -= 20 * (n - 1);
      }
      const bool left = f > 0 && pawnsOnFile[side][f - 1] > 0;
      const bool right = f < 7 && pawnsOnFile[side][f + 1] > 0;
      if (!left && !right) {
        mg[side] -= 10 * n;
        eg[side] -= 15 * n;
      }
      // Passed pawn: look at the most advanced pawn on this file.
      const int r = side ? minPawnRow[1][f] : maxPawnRow[0][f];
      bool passed = true;
      for (int ff = std::max(0, f - 1); ff <= std::min(7, f + 1) && passed; ++ff) {
        if (side) { // white pawn at row r: blocked by black pawns with row < r
          if (minPawnRow[enemy][ff] < r) passed = false;
        } else {    // black pawn at row r: blocked by white pawns with row > r
          if (maxPawnRow[enemy][ff] > r) passed = false;
        }
      }
      if (passed) {
        const int adv = side ? 7 - r : r; // 1..6 ranks advanced
        mg[side] += PASSED_MG[adv];
        eg[side] += PASSED_EG[adv];
      }
    }
  }

  // Rooks on open / half-open files.
  for (int r = 0; r < 8; ++r)
    for (int c = 0; c < 8; ++c) {
      char k = bd.keyAt(r, c);
      if (k != 'R' && k != 'r') continue;
      const int side = k == 'R' ? 1 : 0;
      if (pawnsOnFile[side][c] == 0) {
        const int bonus = pawnsOnFile[1 - side][c] == 0 ? 20 : 10;
        mg[side] += bonus;
        eg[side] += bonus / 2;
      }
    }

  // Bishop pair.
  for (int side = 0; side < 2; ++side)
    if (bishops[side] >= 2) {
      mg[side] += 30;
      eg[side] += 50;
    }

  // King shelter (middlegame): pawns in front of a castled king.
  for (int side = 0; side < 2; ++side) {
    const auto kp = king[side];
    if (kp.first < 0) continue;
    const int homeRow = side ? 7 : 0;
    const int dir = side ? -1 : 1;
    if (kp.first != homeRow || (kp.second >= 3 && kp.second <= 4)) continue;
    const char pawn = side ? 'P' : 'p';
    int shield = 0;
    for (int c = std::max(0, kp.second - 1); c <= std::min(7, kp.second + 1); ++c) {
      if (bd.keyAt(homeRow + dir, c) == pawn) shield += 12;
      else if (bd.keyAt(homeRow + 2 * dir, c) == pawn) shield += 6;
      else shield -= 10;
    }
    mg[side] += shield;
  }

  // Mop-up: with a decisive material edge and no enemy pawns, drive the
  // enemy king to the edge and bring our king closer (helps finish mates).
  for (int side = 0; side < 2; ++side) {
    const int enemy = 1 - side;
    if (pawnCount[enemy] == 0 && king[0].first >= 0 && king[1].first >= 0 &&
        nonPawn[side] - nonPawn[enemy] >= 400) {
      const auto ek = king[enemy], ok = king[side];
      const int dist = std::abs(ek.first - ok.first) + std::abs(ek.second - ok.second);
      eg[side] += 10 * centerDistance(ek.first, ek.second) + 4 * (14 - dist);
    }
  }

  phase = std::min(phase, 24);
  const int mgScore = mg[1] - mg[0];
  const int egScore = eg[1] - eg[0];
  int score = (mgScore * phase + egScore * (24 - phase)) / 24;

  // Without pawns, a small material edge (e.g. a lone minor piece) can't win.
  if (pawnCount[1] == 0 && score > 0 && nonPawn[1] - nonPawn[0] < 400) score /= 4;
  if (pawnCount[0] == 0 && score < 0 && nonPawn[0] - nonPawn[1] < 400) score /= 4;

  score += bd.whiteToMove() ? 10 : -10; // tempo
  return bd.whiteToMove() ? score : -score;
}
