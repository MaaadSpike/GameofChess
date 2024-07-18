#ifndef _BOARD_H_
#define _BOARD_H_
#include <vector>
#include "piece.h"
#include <string>
#include <utility>
#include <memory>

class Board{
    std::vector<std::vector<std::unique_ptr<Piece>>> board;
    
    public:
    Board(); // constructor
    Board(const Board & other); // copy assignment
    std::string movePiece(int x, int y, const std::unique_ptr<Piece>& p);// move the piece p to postion x, y, return a string that decrible this move("normal", "castling", "promotion")
    std::unique_ptr<Piece> getPiece(int x, int y) const; // return the piece ptr at position <x,y>
    // setup model
    void placePiece(char c, const std::pair<int,int>& Npo); // place the piece with type c in position Npo
    void removePiece(const std::pair<int,int>& po); // remove the piece at position p
    std::unique_ptr<Piece> copyPiece(const std::unique_ptr<Piece> & p) const;
    void makeblank();
};
#endif
