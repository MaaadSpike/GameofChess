#include "level1.h"
#include "gamecontroller.h"

Move Level1::calculateBestMove(const Gamecontroller &gameController) {
  Board board{gameController.getBoard()};
  info = "random move";
  return selectRandomMove(board.legalMoves());
}
