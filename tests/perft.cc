// Move-generator correctness test: counts leaf nodes of the legal-move tree
// and compares with well-known reference values (https://www.chessprogramming.org/Perft_Results).
#include "../board.h"
#include "../openingbook.h"
#include <chrono>
#include <iostream>

static unsigned long long perft(Board &b, int depth) {
  if (depth == 0) return 1;
  std::vector<Move> moves = b.legalMoves();
  if (depth == 1) return moves.size();
  unsigned long long n = 0;
  for (const auto &m : moves) {
    b.makeMove(m);
    n += perft(b, depth - 1);
    b.undoMove();
  }
  return n;
}

int main() {
  struct Case { const char *name, *fen; int depth; unsigned long long expected; };
  const Case cases[] = {
      {"start", "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1", 4, 197281ULL},
      {"start", "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1", 5, 4865609ULL},
      {"kiwipete", "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1", 3, 97862ULL},
      {"kiwipete", "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1", 4, 4085603ULL},
      {"pos3", "8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 0 1", 5, 674624ULL},
      {"pos4", "r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1", 4, 422333ULL},
      {"pos5", "rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8", 4, 2103487ULL},
      {"pos6", "r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - - 0 10", 3, 89890ULL},
  };
  bool ok = true;
  for (const auto &c : cases) {
    Board b;
    b.loadFEN(c.fen);
    std::string before = b.toFEN();
    auto t0 = std::chrono::steady_clock::now();
    unsigned long long n = perft(b, c.depth);
    double secs = std::chrono::duration<double>(std::chrono::steady_clock::now() - t0).count();
    bool pass = n == c.expected && b.toFEN() == before;
    ok = ok && pass;
    std::cout << (pass ? "PASS " : "FAIL ") << c.name << " depth " << c.depth << ": " << n
              << " (expected " << c.expected << ")  " << secs << "s\n";
  }
  // Every opening-book line must consist of legal moves.
  int lines = 0;
  for (const auto &line : OpeningBook::instance().lines()) {
    Board b;
    ++lines;
    for (const auto &mv : line) {
      bool found = false;
      for (const auto &m : b.legalMoves())
        if (m.toString() == mv) {
          b.makeMove(m);
          found = true;
          break;
        }
      if (!found) {
        std::cout << "FAIL opening book: illegal move " << mv << " in line " << lines << "\n";
        ok = false;
        break;
      }
    }
  }
  std::cout << "Checked " << lines << " opening-book lines.\n";
  std::cout << (ok ? "All perft tests passed." : "PERFT FAILURES!") << std::endl;
  return ok ? 0 : 1;
}
