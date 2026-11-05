#include "computer.h"
#include "gamecontroller.h"
#include "human.h"
#include "player.h"
#include <algorithm>
#include <cctype>
#include <cstdlib>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

namespace {

std::string lower(std::string s) {
  for (char &c : s) c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
  return s;
}

// "human", "computer1".."computer4", also "computer[2]" / "computer 2" style.
std::unique_ptr<Player> createPlayer(std::string s, bool isWhite) {
  s = lower(s);
  std::string digits;
  for (char c : s)
    if (std::isdigit(static_cast<unsigned char>(c))) digits += c;
  std::string word;
  for (char c : s)
    if (std::isalpha(static_cast<unsigned char>(c))) word += c;
  if (word == "human" && digits.empty()) return std::make_unique<Human>(0, isWhite);
  if (word == "computer") {
    int level = digits.empty() ? 4 : std::atoi(digits.c_str());
    if (level < 1 || level > 4) return nullptr;
    return std::make_unique<Computer>(level, isWhite, "level" + std::to_string(level));
  }
  return nullptr;
}

void printHelp(bool bonus) {
  std::cout <<
      "Commands:\n"
      "  game <white> <black>   start a game; players: human, computer1 .. computer4\n"
      "  move <from> <to> [P]   e.g. move e2 e4, move e7 e8 Q (promotion piece optional,\n"
      "                         defaults to a queen); 'move e2e4' also works\n"
      "  move                   let the computer play when it is its turn\n"
      "  play                   computer vs computer: play the game to the end\n"
      "  resign                 concede the current game\n"
      "  setup                  set up a custom position (before a game):\n"
      "                           + K e1   place a piece    - e1      remove a piece\n"
      "                           = black  side to move     default   standard position\n"
      "                           clear    empty board      done      leave setup mode\n"
      "  board                  print the board again\n";
  if (bonus)
    std::cout << "  undo / redo            take back / replay moves\n"
                 "  hint                   ask the engine for a suggestion\n";
  else
    std::cout << "  (start with -enablebonus for undo / redo / hint)\n";
  std::cout << "  help                   this message\n"
               "  quit                   (or Ctrl-D) print the final score and exit\n";
}

void usage(const char *prog) {
  std::cerr << "usage: " << prog << " [-enablebonus] [-text] [-time <ms>]\n"
            << "  -enablebonus   enable undo / redo / hint\n"
            << "  -text          text only (no graphics window)\n"
            << "  -time <ms>     thinking time of computer4 per move (default 1200)\n";
}

bool parseHumanMove(const std::vector<std::string> &args, Move &m) {
  std::string from, to, promo;
  if (args.size() == 1 && (args[0].size() == 4 || args[0].size() == 5)) {
    from = args[0].substr(0, 2);
    to = args[0].substr(2, 2);
    if (args[0].size() == 5) promo = args[0].substr(4, 1);
  } else if (args.size() == 2 || args.size() == 3) {
    from = args[0];
    to = args[1];
    if (args.size() == 3) promo = args[2];
  } else {
    return false;
  }
  std::pair<int, int> f, t;
  if (!Move::parseSquare(from, f) || !Move::parseSquare(to, t)) return false;
  if (promo.size() > 1) return false;
  m.setOpo(f);
  m.setNpo(t);
  m.settype2(promo.empty() ? 'e' : promo[0]);
  return true;
}

} // namespace

int main(int argc, char *argv[]) {
  bool bonus = false, graphics = true;
  int thinkMs = 1200;
  for (int i = 1; i < argc; ++i) {
    std::string a = argv[i];
    if (a == "-enablebonus") bonus = true;
    else if (a == "-text" || a == "-nographics") graphics = false;
    else if (a == "-time" && i + 1 < argc) thinkMs = std::max(50, std::atoi(argv[++i]));
    else {
      usage(argv[0]);
      return 1;
    }
  }

  Gamecontroller gc{bonus, graphics, thinkMs};
  std::cout << "Chess - type 'help' for commands, e.g.  game human computer4" << std::endl;

  // In human-vs-computer games the computer answers immediately.
  auto autoplay = [&gc]() {
    while (gc.gameRunning() && gc.isComputerTurn() && gc.hasHuman()) gc.computermakemove();
  };

  std::string line;
  while (std::getline(std::cin, line)) {
    std::istringstream iss{line};
    std::string command;
    if (!(iss >> command)) continue;
    command = lower(command);
    std::vector<std::string> args;
    for (std::string a; iss >> a;) args.push_back(a);

    if (command == "game") {
      if (gc.gameRunning()) {
        std::cout << "A game is already in progress - resign it first." << std::endl;
        continue;
      }
      if (args.size() != 2) {
        std::cout << "Usage: game <white> <black>   (human, computer1 .. computer4)" << std::endl;
        continue;
      }
      std::vector<std::unique_ptr<Player>> players;
      players.emplace_back(createPlayer(args[0], true));
      players.emplace_back(createPlayer(args[1], false));
      if (!players[0] || !players[1]) {
        std::cout << "Invalid player. Use human or computer1 .. computer4." << std::endl;
        continue;
      }
      gc.initializeGame(std::move(players));
      autoplay();
    } else if (command == "move") {
      if (!gc.gameRunning()) {
        std::cout << "No game in progress. Start one with: game <white> <black>" << std::endl;
        continue;
      }
      if (gc.isComputerTurn()) {
        if (!args.empty())
          std::cout << "It is the computer's turn - type 'move' to let it play." << std::endl;
        else
          gc.computermakemove();
        autoplay();
        continue;
      }
      Move m;
      if (!parseHumanMove(args, m)) {
        std::cout << "Usage: move <from> <to> [promotion]   e.g. move e2 e4" << std::endl;
        continue;
      }
      if (gc.makeMove(m)) autoplay();
    } else if (command == "play") {
      if (!gc.gameRunning()) {
        std::cout << "No game in progress." << std::endl;
      } else if (gc.hasHuman()) {
        std::cout << "'play' runs computer-vs-computer games; use 'move' instead." << std::endl;
      } else {
        while (gc.gameRunning()) gc.computermakemove();
      }
    } else if (command == "resign") {
      gc.handleResign();
    } else if (command == "setup") {
      if (!gc.canSetup()) {
        std::cout << "Finish (or resign) the current game before entering setup mode."
                  << std::endl;
        continue;
      }
      gc.beginSetup();
      bool done = false;
      while (!done && std::getline(std::cin, line)) {
        std::istringstream ls{line};
        std::string first;
        if (!(ls >> first)) continue;
        if (lower(first) == "done") {
          if (gc.validsetupboard()) {
            gc.endSetup();
            done = true;
          }
        } else if (lower(first) == "help") {
          std::cout << "+ K e1 | - e1 | = white/black | default | clear | done" << std::endl;
        } else {
          gc.setupboard(line);
        }
      }
      if (!done) break; // input ended inside setup mode
    } else if (command == "undo") {
      gc.undo();
    } else if (command == "redo") {
      gc.redo();
      autoplay();
    } else if (command == "hint") {
      gc.hint();
    } else if (command == "board" || command == "print") {
      gc.printBoard();
    } else if (command == "help") {
      printHelp(gc.cheatsEnabled());
    } else if (command == "quit" || command == "exit") {
      break;
    } else {
      std::cout << "Unknown command '" << command << "'. Type 'help' for the list of commands."
                << std::endl;
    }
  }
  gc.callprintsb();
  return 0;
}
