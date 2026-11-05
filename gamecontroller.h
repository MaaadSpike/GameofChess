#ifndef _GAMECONTROLLER_H_
#define _GAMECONTROLLER_H_
#include "board.h"
#include "graphdisplay.h"
#include "move.h"
#include "player.h"
#include "scoreboard.h"
#include "textdisplay.h"
#include <memory>
#include <stack>
#include <string>
#include <vector>

class Level4;

// Controller of the MVC design: owns the board (model), the text and
// graphical views, the scoreboard and the two players, validates and plays
// moves, detects the end of the game and keeps the undo/redo stacks.
class Gamecontroller {
  bool cancheat; // bonus features: undo / redo / hint
  int thinkMs;   // thinking time of the level-4 computer

  std::unique_ptr<Graphdisplay> gd;
  std::unique_ptr<Textdisplay> td;
  std::unique_ptr<Board> bd;
  std::unique_ptr<Scoreboard> sb;
  std::vector<std::unique_ptr<Player>> players; // [0] white, [1] black

  std::string gamestate = "none"; // none / continue / Whitewin / Blackwin / draw
  std::string endMessage;

  std::stack<std::unique_ptr<Move>> MoveHistory;
  std::stack<std::unique_ptr<Move>> RedoS;

  std::unique_ptr<Board> customStart; // position prepared in setup mode
  bool standardStart = true;
  bool inSetup = false;
  std::vector<std::string> played;    // moves of this game, e.g. "e2e4"
  std::unique_ptr<Level4> hintEngine;

  void applyMove(const Move &m, bool clearRedo = true);
  void afterPositionChange();
  void finishGame(const std::string &result, const std::string &message);
  std::string statusLine() const;
  void refresh();
  int curPlayerIndex() const { return bd->whiteToMove() ? 0 : 1; }

public:
  explicit Gamecontroller(bool cheat, bool graphics = true, int thinkMs = 1200);
  ~Gamecontroller();

  void initializeGame(std::vector<std::unique_ptr<Player>> &&players);
  bool gameRunning() const { return gamestate == "continue"; }
  std::string curTurn() const { return bd->whiteToMove() ? "White" : "Black"; }
  std::string checkState() const { return gamestate; }

  void updatetd();
  void updategd();
  void updatesb();
  void printBoard() { refresh(); }

  bool ischeck() const;
  bool ischeckmate();
  bool isdraw();

  // Human move: m holds the from/to squares and, for a promotion, the piece
  // letter in type2 ('e' = not given -> queen). Returns false if illegal.
  bool makeMove(Move &m);
  void computermakemove();
  int whomakethisstep() const; // 0 = human, 1..4 = computer level
  bool isComputerTurn() const;
  bool hasHuman() const;
  void handleResign();

  // ----- setup mode -----
  bool canSetup() const { return !gameRunning(); }
  void beginSetup();
  void setupboard(const std::string &s);
  bool validsetupboard();
  void endSetup();
  void makeboardblank();

  // ----- bonus features (-enablebonus) -----
  bool cheatsEnabled() const { return cancheat; }
  void undo();
  void redo();
  void hint();

  void callprintsb() const;

  // ----- read access for the computer players -----
  const Board &getBoard() const { return *bd; }
  bool isStandardStart() const { return standardStart; }
  const std::vector<std::string> &playedMoves() const { return played; }
  int thinkTime() const { return thinkMs; }
  std::vector<Move> legalMoves() const;
};
#endif
