#include "rook.h"
#include "board.h"
#include "piece.h"

Rook::Rook(std::pair<int, int> coord, bool whiteTurn)
    : Piece{coord, whiteTurn}, moved{false} {}

bool Rook::isMove(const std::pair<int, int> &old_coord,
                  const std::pair<int, int> &new_coord, Board &bd) const {
  std::vector<std::pair<int, int>> moves;

  int n = 8;
  int x = old_coord.first;
  int y = old_coord.second;

if (new_coord.first >= 8 || new_coord.first < 0 ||
  new_coord.second >= 8 || new_coord.second < 0) {
    return false;
  }
  // row left
  for (int i = x, j = y - 1; j >= 0; --j) {
    if (!bd.getPiece(i, j)->isEmptyPiece()) {
      if (bd.getPiece(i, j)->isWhiteTurn() != isWhiteTurn()) {
        moves.push_back({i, j});
      }
      break;
    }
    moves.push_back({i, j});
  }

  // row right
  for (int i = x, j = y + 1; j < n; ++j) {
    if (!bd.getPiece(i, j)->isEmptyPiece()) {
      if (bd.getPiece(i, j)->isWhiteTurn() != isWhiteTurn()) {
        moves.push_back({i, j});
      }
      break;
    }
    moves.push_back({i, j});
  }

  // col up
  for (int i = x + 1, j = y; i < n; ++i) {
    if (!bd.getPiece(i, j)->isEmptyPiece()) {
      if (bd.getPiece(i, j)->isWhiteTurn() != isWhiteTurn()) {
        moves.push_back({i, j});
      }
      break;
    }
    moves.push_back({i, j});
  }

  // col down
  for (int i = x - 1, j = y; i >= 0; --i) {
    if (!bd.getPiece(i, j)->isEmptyPiece()) {
      if (bd.getPiece(i, j)->isWhiteTurn() != isWhiteTurn()) {
        moves.push_back({i, j});
      }
      break;
    }
    moves.push_back({i, j});
  }

  for (auto move : moves) {
    if (move.first == new_coord.first && move.second == new_coord.second) {
      return true;
    }
  }
  return false;
}

bool Rook::isEmptyPiece() const { return false; }

char Rook::key() const {
  if (isWhiteTurn()) {
    return 'R';
  }
  return 'r';
}

bool Rook::hasMoved() const { return moved; }

void Rook::setMoved() { moved = true; }
