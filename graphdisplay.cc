#include "graphdisplay.h"
#include <string>

Graphdisplay::Graphdisplay(): x{Xwindow{800,800}} {}

std::string myctos(char c) {
    if (c == 'r' || c == 'R') return "Rook";
    else if (c == 'k' || c == 'K') return "King";
    else if (c == 'b' || c == 'B') return "Bishop";
    else if (c == 'q' || c == 'Q') return "Queen";
    else if (c == 'n' || c == 'N') return "Knight";
    else if (c == 'p' || c == 'P') return "Pawn";
    else return "";
}
void Graphdisplay::update(const std::unique_ptr<Board> & bd) {
     for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            char c = bd->getPiece(i, j)->key();
            if (c == 'e') {
                if (i % 2 == 0) {
                    if (j % 2 == 0) {
                        x.fillRectangle(j*100, i*100, 100, 100, Xwindow::Black);
                    }else {
                        x.fillRectangle(j*100, i*100, 100, 100, Xwindow::White);
                    }
                } else {
                    if (j % 2 == 0) {
                        x.fillRectangle(j*100, i*100, 100, 100, Xwindow::White);
                    }else {
                        x.fillRectangle(j*100, i*100, 100, 100, Xwindow::Black);
                    }
                } 
            } else {
                if ('a' <= c && c <= 'z') {
                    x.fillRectangle(j*100, i*100, 100, 100, Xwindow::Blue);
                    std::string s = myctos(c);
                    x.drawString(j*100 + 40, i*100 + 50, s);
                }
                if ('A' <= c && c <= 'Z') {
                    x.fillRectangle(j*100, i*100, 100, 100, Xwindow::Green);
                    std::string s = myctos(c);
                    x.drawString(j*100 + 40, i*100 + 50, s);
                }
            }
        }
    }

}
