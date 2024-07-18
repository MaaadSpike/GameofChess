#include "board.h"
#include "king.h"
#include "queen.h"
#include "bishop.h"
#include "rook.h"
#include "pawn.h"
#include "knight.h"
#include "empty.h"
#include <memory>
#include <iostream>

Board::Board() {
    for (int i = 0; i < 8; i++) {
        std::vector<std::unique_ptr<Piece>> vp;
        if (i == 0) { // black
            vp.emplace_back(std::make_unique<Rook>(std::pair<int,int>{i, 0}, false));
            vp.emplace_back(std::make_unique<Knight>(std::pair<int,int>{i, 1}, false));
            vp.emplace_back(std::make_unique<Bishop>(std::pair<int,int>{i, 2}, false));
            vp.emplace_back(std::make_unique<Queen>(std::pair<int,int>{i, 3}, false));
            vp.emplace_back(std::make_unique<King>(std::pair<int,int>{i, 4}, false));
            vp.emplace_back(std::make_unique<Bishop>(std::pair<int,int>{i, 5}, false));
            vp.emplace_back(std::make_unique<Knight>(std::pair<int,int>{i, 6}, false));
            vp.emplace_back(std::make_unique<Rook>(std::pair<int,int>{i, 7}, false));
        } else if (i == 1) { // black
            for (int j = 0; j < 8; j++) {
                vp.emplace_back(std::make_unique<Pawn>(std::pair<int,int>{i, j}, false));
            }
        } else if (i == 6) { // white
            for (int j = 0; j < 8; j++) {
                vp.emplace_back(std::make_unique<Pawn>(std::pair<int,int>{i, j}, true));
            }
        } else if (i == 7) { // white
            vp.emplace_back(std::make_unique<Rook>(std::pair<int,int>{i, 0}, true));
            vp.emplace_back(std::make_unique<Knight>(std::pair<int,int>{i, 1}, true));
            vp.emplace_back(std::make_unique<Bishop>(std::pair<int,int>{i, 2}, true));
            vp.emplace_back(std::make_unique<Queen>(std::pair<int,int>{i, 3}, true));
            vp.emplace_back(std::make_unique<King>(std::pair<int,int>{i, 4}, true));
            vp.emplace_back(std::make_unique<Bishop>(std::pair<int,int>{i, 5}, true));
            vp.emplace_back(std::make_unique<Knight>(std::pair<int,int>{i, 6}, true));
            vp.emplace_back(std::make_unique<Rook>(std::pair<int,int>{i, 7}, true));
        } else {
            for (int j = 0; j < 8; j++) { // empty
                 vp.emplace_back(std::make_unique<Empty>(std::pair<int,int>{i, j}, false));
            }
        }
        board.emplace_back(std::move(vp));
    }
}

std::unique_ptr<Piece> Board::copyPiece(const std::unique_ptr<Piece> & p) const {
    std::unique_ptr<Piece> np;
    if (p->key() == 'k') {
        np = std::make_unique<King>(p->getCoord(), false);
        if(p->hasMoved()) {
            np->setMoved();
        }
    }
    else if (p->key() == 'q') np = std::make_unique<Queen>(p->getCoord(), false);
    else if (p->key() == 'b') np = std::make_unique<Bishop>(p->getCoord(), false);
    else if (p->key() == 'r') np = std::make_unique<Rook>(p->getCoord(), false);
    else if (p->key() == 'n') np = std::make_unique<Knight>(p->getCoord(), false);
    else if (p->key() == 'p') {
        np = std::make_unique<Pawn>(p->getCoord(), false);
        if(p->hasMoved()) {
            np->setMoved();
        }
    }
    // white player
    else if (p->key() == 'K') {
        np = std::make_unique<King>(p->getCoord(), true);
        if(p->hasMoved()) {
            np->setMoved();
        }
    }
    else if (p->key() == 'Q') np = std::make_unique<Queen>(p->getCoord(), true);
    else if (p->key() == 'B') np = std::make_unique<Bishop>(p->getCoord(), true);
    else if (p->key() == 'R') np = std::make_unique<Rook>(p->getCoord(), true);
    else if (p->key() == 'N') np = std::make_unique<Knight>(p->getCoord(), true);
    else if (p->key() == 'P') {
        np = std::make_unique<Pawn>(p->getCoord(), true);
        if(p->hasMoved()) {
            np->setMoved();
        }
    }
    // empty
    else np = std::make_unique<Empty>(p->getCoord(), false);
    return np;
}

void Board::makeblank() {
    //std::cout << "I reach make blank" << std::endl;
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            board[i][j] = std::make_unique<Empty>(std::pair<int, int>{i,j}, false);
        }
    }
}

Board::Board(const Board & other) {
    board.resize(8);
    for(int i = 0; i < 8; i++) {
        board[i].resize(8);
        for(int j = 0; j < 8; j++) {
            board[i][j] = copyPiece(other.board[i][j]);
        }
    }
}


std::string Board::movePiece(int x, int y, const std::unique_ptr<Piece>& p) {
    if (p->key() == 'P' && p->getCoord().first == 1) { // white
        board[x][y] = copyPiece(p);
        board[p->getCoord().first][p->getCoord().second] = std::make_unique<Empty>(p->getCoord(), false);
        board[x][y]->setCoord(std::pair<int,int>{x,y});
        p->setMoved();
        return "promotion";
    } else if (p->key() == 'p' && p->getCoord().first == 6) { // black
        board[x][y] = copyPiece(p);
        board[p->getCoord().first][p->getCoord().second] = std::make_unique<Empty>(p->getCoord(), false);
        board[x][y]->setCoord(std::pair<int,int>{x,y});
        p->setMoved();
        return "promotion";
    } else if (p->key() == 'K' && p->getCoord() == std::pair<int,int>{7, 4} && x == 7 && y == 2) { // white king left
        board[7][2] = copyPiece(p);
        board[7][4] = std::make_unique<Empty>(p->getCoord(), false);
        board[x][y]->setCoord(std::pair<int,int>{7,2});
        board[7][3] = std::make_unique<Rook>(std::pair<int,int>{7,3}, true);
        board[7][0] = std::make_unique<Empty>(std::pair<int,int>{7, 0}, false);
        p->setMoved();
        board[7][3]->setMoved();
        return "castling";
    } else if (p->key() == 'K' && p->getCoord() == std::pair<int,int>{7, 4} && x == 7 && y == 6) { // white king right
        board[7][6] = copyPiece(p);
        board[7][4] = std::make_unique<Empty>(p->getCoord(), false);
        board[x][y]->setCoord(std::pair<int,int>{7,6});
        board[7][5] = std::make_unique<Rook>(std::pair<int,int>{7,5}, true);
        board[7][7] = std::make_unique<Empty>(std::pair<int,int>{7,7}, false);
        p->setMoved();
        board[7][3]->setMoved();
        return "castling";
    } else if (p->key() == 'k' && p->getCoord() == std::pair<int,int>{0, 4} && x == 0 && y == 2) { // white king left
        board[0][2] = copyPiece(p);
        board[0][4] = std::make_unique<Empty>(p->getCoord(), false);
        board[x][y]->setCoord(std::pair<int,int>{0,2});
        board[0][3] = std::make_unique<Rook>(std::pair<int,int>{0,3}, false);
        board[0][0] = std::make_unique<Empty>(std::pair<int,int>{0, 0}, false);
        p->setMoved();
        board[7][3]->setMoved();
        return "castling";
    } else if (p->key() == 'k' && p->getCoord() == std::pair<int,int>{0, 4} && x == 0 && y == 6) { // white king right
        board[0][6] = copyPiece(p);
        board[0][4] = std::make_unique<Empty>(p->getCoord(), false);
        board[x][y]->setCoord(std::pair<int,int>{0,6});
        board[0][5] = std::make_unique<Rook>(std::pair<int,int>{0,5}, false);
        board[0][7] = std::make_unique<Empty>(std::pair<int,int>{0,7}, false);
        p->setMoved();
        board[7][3]->setMoved();
        return "castling";
    }
    board[x][y] = copyPiece(p);
    board[p->getCoord().first][p->getCoord().second] = std::make_unique<Empty>(p->getCoord(), false);
    board[x][y]->setCoord(std::pair<int,int>{x,y});
    board[x][y]->setMoved();
    return "normal";
}

std::unique_ptr<Piece> Board::getPiece(int x, int y) const {
    return copyPiece(board[x][y]);
}

void Board::placePiece(char c, const std::pair<int,int>& Npo) {
    //std::cout << "new type is "<< c << " postition is " << Npo.first <<  Npo.second <<std::endl;
    // black player
    if (c == 'k') board[Npo.first][Npo.second] = std::make_unique<King>(Npo, false);
    else if (c == 'q') board[Npo.first][Npo.second] = std::make_unique<Queen>(Npo, false);
    else if (c == 'b') board[Npo.first][Npo.second] = std::make_unique<Bishop>(Npo, false);
    else if (c == 'r') board[Npo.first][Npo.second] = std::make_unique<Rook>(Npo, false);
    else if (c == 'n') board[Npo.first][Npo.second] = std::make_unique<Knight>(Npo, false);
    else if (c == 'p') board[Npo.first][Npo.second] = std::make_unique<Pawn>(Npo, false);
    // white player
    else if (c == 'K') board[Npo.first][Npo.second] = std::make_unique<King>(Npo, true);
    else if (c == 'Q') board[Npo.first][Npo.second] = std::make_unique<Queen>(Npo, true);
    else if (c == 'B') board[Npo.first][Npo.second] = std::make_unique<Bishop>(Npo, true);
    else if (c == 'R') board[Npo.first][Npo.second] = std::make_unique<Rook>(Npo, true);
    else if (c == 'N') board[Npo.first][Npo.second] = std::make_unique<Knight>(Npo, true);
    else if (c == 'P') board[Npo.first][Npo.second] = std::make_unique<Pawn>(Npo, true);
    // empty
    else board[Npo.first][Npo.second] = std::make_unique<Empty>(Npo, false);
}

void Board::removePiece(const std::pair<int,int>& po) {
    board[po.first][po.second] = std::make_unique<Empty>(po, false);
}
