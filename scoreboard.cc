#include "scoreboard.h"
#include <iostream>
#include <sstream>

void Scoreboard::addscores(const std::string &s) {
  if (s == "Whitewin") {
    whitescores += 1;
  } else if (s == "Blackwin") {
    blackscores += 1;
  } else if (s == "draw") {
    whitescores += 0.5;
    blackscores += 0.5;
  }
}

std::string Scoreboard::summary() const {
  std::ostringstream os;
  os << "White " << whitescores << " - Black " << blackscores;
  return os.str();
}

void Scoreboard::printscoreboard() const {
  std::cout << "Final Score:" << std::endl;
  std::cout << "White: " << whitescores << std::endl;
  std::cout << "Black: " << blackscores << std::endl;
}
