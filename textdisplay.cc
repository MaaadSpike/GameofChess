#include "textdisplay.h"
#include "board.h"
#include <iostream>

void Textdisplay::printtd(const std::unique_ptr<Board>& bd) {
    for (int i = 0; i < 8; i++) {
        std::cout << 8 - i << " ";
        for (int j = 0; j < 8; j++) {
            char c = bd->getPiece(i, j)->key();
            if (c == 'e') {
                if (i % 2 == 0) {
                    if (j % 2 == 0) {
                        std::cout << " ";
                    }else {
                        std::cout << "_"; 
                    }
                } else {
                    if (j % 2 == 0) {
                        std::cout << "_";
                    }else {
                        std::cout << " "; 
                    }
                } 
            } else {
                std::cout << c;
            }
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;
    std::cout << "  abcdefgh" << std::endl;
}
