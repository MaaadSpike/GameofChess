#include "computer.h"
#include "gamecontroller.h"
#include "human.h"
#include "player.h"
#include <iostream>
#include <memory>
#include <sstream>
#include <string>

std::unique_ptr<Player> createPlayer(std::string s, bool isWhite) {
  std::unique_ptr<Player> p;
  std::string level;
  if (s == "human") {
    p = std::make_unique<Human>(0, isWhite);
    // p->setId(0);
  } else if (s == "computer1") {
    level = "level1";
    p = std::make_unique<Computer>(1, isWhite, level);
    // p->setId(1);
  } else if (s == "computer2") {
    level = "level2";
    p = std::make_unique<Computer>(2, isWhite, level);
    // p->setId(2);
  } else if (s == "computer3") {
    level = "level3";
    p = std::make_unique<Computer>(3, isWhite, level);
    // p->setId(3);
  } else {
    level = "level4";
    p = std::make_unique<Computer>(4, isWhite, level);
    // p->setId(4);
  }
  return p;
}

bool checkvalid(std::string s) { // for move only
  if (s.size() != 2 || !('a' <= s[0] && s[0] <= 'h') ||
      !('1' <= s[1] && s[1] <= '8')) {
    return false;
  }
  return true;
}

std::pair<int, int> stopair(const std::string s) { // for move only
  int x = 7 - (s[1] - '0' - 1);
  int y = s[0] - 'a';
  std::pair<int, int> p{x, y};
  return p;
}

int main(int argc, char *argv[]) {
  std::unique_ptr<Gamecontroller> gc;
  if (argc == 1) {
    gc = std::make_unique<Gamecontroller>(false);
  } else if (argc == 2 && std::string{argv[1]} == "-enablebonus") {
    gc = std::make_unique<Gamecontroller>(true);
  } else {
    std::cerr << "chess [none] or [-enablebonus]" << std::endl;
    return 1;
  }
  bool gamestarted = false;
  std::string command;

  while (1) {
    if (gamestarted && gc->whomakethisstep() >= 1) {
      std::cout << "computer moves" << std::endl;
      gc->computermakemove();
    }
    if (!(std::cin >> command)) {
      break;
    }
    if (command == "game") {
      std::string s1;
      std::string s2;
      if (std::cin >> s1 >> s2) {
        std::vector<std::unique_ptr<Player>> players;
        players.emplace_back(createPlayer(s1, true));
        players.emplace_back(createPlayer(s2, false));
        gc->initializeGame(std::move(players));
        gamestarted = true;
      } else {
        std::cerr << "Invalid player" << std::endl;
      }
      continue;
    }

    if (gamestarted == false) { // we have to set players
      std::cout << "You haven't settle players" << std::endl;
      continue;
    }

    if (command == "resign") {
      std::cout << "you resign this game, Please start a new game" << std::endl;
      gamestarted = false;
      // FIX: resign should increment scoreboard by 1
      gc->handleResign();
      continue;
    }

    if (gc->ischeckmate()) {
      // updatesb
      gamestarted = false;
      std::cout << "please start a new game" << std::endl;
      continue;
    }

    // if (gc->whomakethisstep() >= 1) {
    //   std::cout << "computer moves" << std::endl;
    //     gc->computermakemove();
    // }

    if (gc->whomakethisstep() == 0 && (command == "move")) {
      std::string s;
      std::getline(std::cin, s);
      Move m;
      if (s.size() == 6) {
        std::istringstream iss{s};
        std::string s1;
        std::string s2;
        iss >> s1 >> s2;
        if (!checkvalid(s1) || !checkvalid(s2)) {
          std::cout << "Invalid move1" << std::endl;
          continue;
        }
        m.setOpo(stopair(s1));
        m.setNpo(stopair(s2));
      } else if (s.size() == 8) {
        std::istringstream iss{s};
        std::string s1;
        std::string s2;
        char c;
        // FIX: must get input before cond
        iss >> s1 >> s2 >> c;
        if (c == 'k' || c == 'K') {
          std::cout << "You cannot change to King" << std::endl;
          continue;
        }
        // FIX: Pond promotion to other team
        if ((!checkvalid(s1) || !checkvalid(s2) ||
             !(c == 'r' || c == 'b' || c == 'n' || c == 'q')) &&
            gc->curTurn() == "Black") {
          std::cout << "Invalid move2" << std::endl;
          continue;
        }
        // FIX: Pond promotion to other team
        if ((!checkvalid(s1) || !checkvalid(s2) ||
             !(c == 'R' || c == 'B' || c == 'N' || c == 'Q')) &&
            gc->curTurn() == "White") {
          std::cout << "Invalid move2" << std::endl;
          continue;
        }

        m.setOpo(stopair(s1));
        m.setNpo(stopair(s2));
        m.settype2(c);
      } else {
        std::cout << "Invalid move3" << std::endl;
        continue;
      }
      // std::cout << "this move is from (" << m.getOpo().first <<
      // m.getOpo().second << ") to (" << m.getNpo().first << m.getNpo().second
      // << ")" << std::endl;

      gc->makeMove(m);
      continue;
    }

    // std::cout << "I reach line 83" << std::endl;
    if (command == "setup") {
      // FIX: set board to empty board
      gc->makeboardblank();

      std::string s;
      while (1) {
        std::getline(std::cin, s);
        if (s == "done") {
          if (gc->validsetupboard()) {
            break;
          }
          // FIX: check if the board is valid
          std::cout << "Invalid board" << '\n';
          continue;
        }
        gc->setupboard(s);
      }
      continue;
    }
  }
  gc->callprintsb();
}
