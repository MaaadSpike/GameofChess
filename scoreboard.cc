#include "scoreboard.h"
#include <iostream>

void Scoreboard::addscores(std::string s) {
    if (s == "Whitewin") {
        whitescores += 1;
    } else if (s == "Blackwin") {
        blackscores += 1;
    } else { // draw
        whitescores += 0.5;
        blackscores += 0.5;
    }
}

void Scoreboard::printscoreboard() {
        std::cout << "Final Score:" << std:: endl;
        std::cout << "White: " << whitescores <<std::endl;
        std::cout << "Black: " << blackscores << std::endl;
    }