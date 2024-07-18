#include "piece.h"

Piece::Piece(std ::pair<int, int> coord, bool whiteTurn)
    : coordinate{coord}, whiteTurn{whiteTurn} {}

std::pair<int, int> Piece::getCoord() const { return coordinate; }

void Piece::setCoord(const std::pair<int, int> &new_coord) {
  coordinate = new_coord;
}

bool Piece::isWhiteTurn() const { return whiteTurn; }

bool Piece::hasMoved() const { return false; }
void Piece::setMoved() {}
