#include "queen.h"
#include "board.h"
#include "piece.h"

Queen::Queen(std::pair<int, int> coord, bool whiteTurn)
    : Piece{coord, whiteTurn} {}

bool Queen::isMove(const std::pair<int, int> &old_coord,
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

  // top left diagonal
  for (int i = x - 1, j = y - 1; i >= 0 && j >= 0; --i, --j) {
    if (!bd.getPiece(i, j)->isEmptyPiece()) {
      if (bd.getPiece(i, j)->isWhiteTurn() != isWhiteTurn()) {
        moves.push_back({i, j});
      }
      break;
    }
    moves.push_back({i, j});
  }

  // top right diagonal
  for (int i = x - 1, j = y + 1; i >= 0 && j < n; i--, j++) {
    if (!bd.getPiece(i, j)->isEmptyPiece()) {
      if (bd.getPiece(i, j)->isWhiteTurn() != isWhiteTurn()) {
        moves.push_back({i, j});
      }
      break;
    }
    moves.push_back({i, j});
  }

  // bottom left diagonal
  for (int i = x + 1, j = y - 1; i < n && j >= 0; i++, j--) {
    if (!bd.getPiece(i, j)->isEmptyPiece()) {
      if (bd.getPiece(i, j)->isWhiteTurn() != isWhiteTurn()) {
        moves.push_back({i, j});
      }
      break;
    }
    moves.push_back({i, j});
  }

  // bottom right diagonal
  for (int i = x + 1, j = y + 1; i < n && j < n; i++, j++) {
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

bool Queen::isEmptyPiece() const { return false; }

char Queen::key() const {
  if (isWhiteTurn()) {
    return 'Q';
  }
  return 'q';
}
