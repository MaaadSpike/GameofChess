#ifndef _BOARD_H_
#define _BOARD_H_
#include "move.h"
#include "piece.h"
#include <cstdint>
#include <memory>
#include <string>
#include <utility>
#include <vector>

// The Board owns the 64 Piece objects (empty squares hold an Empty piece)
// and knows the full rules of chess: legal move generation, check,
// castling rights, en passant, promotion, and draw conditions.
//
// Moves are applied in place with makeMove() and reverted with undoMove(),
// which is what makes both undo/redo and the computer's search fast.
class Board {
public:
  enum CastleRight { WHITE_OO = 1, WHITE_OOO = 2, BLACK_OO = 4, BLACK_OOO = 8 };

private:
  std::vector<std::vector<std::unique_ptr<Piece>>> board; // [row][col]
  char cells[8][8];                 // fast mirror of board[r][c]->key()
  bool whiteMove = true;            // side to move
  int castle = 0;                   // CastleRight bit set
  std::pair<int, int> ep{-1, -1};   // en passant target square
  int halfmove = 0;                 // plies since last capture / pawn move
  int fullmove = 1;
  uint64_t zkey = 0;                // Zobrist hash of the position
  std::pair<int, int> kingPos[2];   // [0] black king, [1] white king

  struct Undo {
    Move m;
    std::unique_ptr<Piece> captured;     // captured piece (nullptr if none)
    std::unique_ptr<Piece> promotedPawn; // the pawn that promoted
    bool moverMoved = false;
    bool rookMoved = false;
    int castle = 0;
    std::pair<int, int> ep{-1, -1};
    int halfmove = 0;
    int fullmove = 1;
    uint64_t zkey = 0;
    bool isNull = false;
  };
  std::vector<Undo> history;
  std::vector<uint64_t> keyHistory;               // for repetition detection
  std::vector<std::unique_ptr<Piece>> emptyPool;  // recycled Empty pieces

  std::unique_ptr<Piece> takeEmpty(int r, int c);
  std::unique_ptr<Piece> lift(int r, int c); // remove piece, leave Empty
  void swapSquares(int r1, int c1, int r2, int c2);
  void recomputeKey();
  void locateKings();

public:
  Board(); // standard starting position
  Board(const Board &other);
  Board &operator=(const Board &other);
  Board(Board &&) = default;
  Board &operator=(Board &&) = default;

  static bool inBounds(int r, int c) { return r >= 0 && r < 8 && c >= 0 && c < 8; }

  // ----- queries -----
  char keyAt(int r, int c) const { return cells[r][c]; }
  const Piece &at(int r, int c) const { return *board[r][c]; }
  std::unique_ptr<Piece> getPiece(int x, int y) const { return board[x][y]->clone(); }
  bool whiteToMove() const { return whiteMove; }
  int castlingRights() const { return castle; }
  std::pair<int, int> enPassantSquare() const { return ep; }
  int halfmoveClock() const { return halfmove; }
  int fullmoveNumber() const { return fullmove; }
  uint64_t hash() const { return zkey; }
  int plyCount() const { return static_cast<int>(history.size()); }
  std::pair<int, int> findKing(bool white) const { return kingPos[white ? 1 : 0]; }

  bool isAttacked(int r, int c, bool byWhite) const;
  bool inCheck(bool white) const;
  bool inCheck() const { return inCheck(whiteMove); } // side to move

  void pseudoLegalMoves(std::vector<Move> &out, bool capturesOnly = false) const;
  std::vector<Move> legalMoves();
  bool hasLegalMove();

  // ----- making moves -----
  void makeMove(const Move &m); // m must come from the move generator
  void undoMove();
  void makeNullMove();          // pass (used by the engine's null-move pruning)
  void undoNullMove();

  // ----- draw rules -----
  bool isRepetition() const;   // position occurred before (engine use)
  int repetitionCount() const; // how many times the current position occurred
  bool insufficientMaterial() const;

  // ----- setup mode -----
  void placePiece(char c, const std::pair<int, int> &Npo);
  void removePiece(const std::pair<int, int> &po);
  void makeblank();
  void setTurn(bool white);
  void finishSetup(); // derive castling rights, reset history
  bool loadFEN(const std::string &fen);
  std::string toFEN() const;
};
#endif
