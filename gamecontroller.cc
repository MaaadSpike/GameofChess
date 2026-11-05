#include "gamecontroller.h"
#include "computer.h"
#include "level4.h"
#include <cctype>
#include <iostream>
#include <sstream>

Gamecontroller::Gamecontroller(bool cheat, bool graphics, int thinkMs)
    : cancheat{cheat}, thinkMs{thinkMs}, gd{std::make_unique<Graphdisplay>(graphics)},
      td{std::make_unique<Textdisplay>()}, bd{std::make_unique<Board>()},
      sb{std::make_unique<Scoreboard>()} {}

Gamecontroller::~Gamecontroller() = default;

// ======================= views =======================

void Gamecontroller::updatetd() { td->printtd(*bd); }

void Gamecontroller::updategd() {
  const Move *last = MoveHistory.empty() ? nullptr : MoveHistory.top().get();
  gd->update(*bd, last, statusLine());
}

void Gamecontroller::updatesb() { sb->addscores(gamestate); }

void Gamecontroller::refresh() {
  updatetd();
  updategd();
}

std::string Gamecontroller::statusLine() const {
  if (inSetup) return "Setup mode  -  " + curTurn() + " to move";
  if (gamestate == "none") return "Type: game <white> <black>    (" + sb->summary() + ")";
  if (gamestate != "continue") return endMessage + "    (" + sb->summary() + ")";
  std::string s = curTurn() + " to move";
  if (bd->inCheck()) s += "  -  Check!";
  return s;
}

void Gamecontroller::callprintsb() const { sb->printscoreboard(); }

// ======================= game flow =======================

void Gamecontroller::initializeGame(std::vector<std::unique_ptr<Player>> &&ps) {
  players = std::move(ps);
  if (customStart) {
    bd = std::make_unique<Board>(*customStart);
    customStart.reset();
    standardStart = false;
  } else {
    bd = std::make_unique<Board>();
    standardStart = true;
  }
  MoveHistory = {};
  RedoS = {};
  played.clear();
  gamestate = "continue";
  endMessage.clear();
  std::cout << "New game: White = " << players[0]->describe()
            << ", Black = " << players[1]->describe() << std::endl;
  afterPositionChange();
}

void Gamecontroller::finishGame(const std::string &result, const std::string &message) {
  gamestate = result;
  endMessage = message;
  updatesb();
}

// Print the board, then report check / the end of the game.
void Gamecontroller::afterPositionChange() {
  std::string message;
  const bool check = bd->inCheck();
  if (!bd->hasLegalMove()) {
    if (check) {
      const std::string winner = bd->whiteToMove() ? "Black" : "White";
      message = "Checkmate! " + winner + " wins!";
      finishGame(winner + "win", "Checkmate! " + winner + " wins");
    } else {
      message = "Stalemate!";
      finishGame("draw", "Stalemate - draw");
    }
  } else if (bd->halfmoveClock() >= 100) {
    message = "Draw by the fifty-move rule.";
    finishGame("draw", "Draw (fifty-move rule)");
  } else if (bd->repetitionCount() >= 3) {
    message = "Draw by threefold repetition.";
    finishGame("draw", "Draw (threefold repetition)");
  } else if (bd->insufficientMaterial()) {
    message = "Draw by insufficient material.";
    finishGame("draw", "Draw (insufficient material)");
  } else if (check) {
    message = curTurn() + " is in check.";
  }
  refresh();
  if (!message.empty()) std::cout << message << std::endl;
  if (gamestate != "continue")
    std::cout << "Start a new game with: game <white-player> <black-player>" << std::endl;
}

void Gamecontroller::applyMove(const Move &m, bool clearRedo) {
  bd->makeMove(m);
  MoveHistory.push(std::make_unique<Move>(m));
  if (clearRedo) RedoS = {};
  played.push_back(m.toString());
}

bool Gamecontroller::ischeck() const { return bd->inCheck(); }

bool Gamecontroller::ischeckmate() { return bd->inCheck() && !bd->hasLegalMove(); }

bool Gamecontroller::isdraw() {
  return (!bd->inCheck() && !bd->hasLegalMove()) || bd->halfmoveClock() >= 100 ||
         bd->repetitionCount() >= 3 || bd->insufficientMaterial();
}

std::vector<Move> Gamecontroller::legalMoves() const {
  Board copy{*bd};
  return copy.legalMoves();
}

int Gamecontroller::whomakethisstep() const {
  if (players.size() < 2) return 0;
  return players[curPlayerIndex()]->getId();
}

bool Gamecontroller::isComputerTurn() const {
  return players.size() == 2 && !players[curPlayerIndex()]->isHuman();
}

bool Gamecontroller::hasHuman() const {
  return players.size() == 2 && (players[0]->isHuman() || players[1]->isHuman());
}

bool Gamecontroller::makeMove(Move &m) {
  if (!gameRunning()) {
    std::cout << "No game in progress." << std::endl;
    return false;
  }
  const auto from = m.getOpo(), to = m.getNpo();
  const char piece = bd->keyAt(from.first, from.second);
  const std::string sq = Move::squareName(from);
  if (piece == 'e') {
    std::cout << "There is no piece on " << sq << "." << std::endl;
    return false;
  }
  const bool pieceWhite = std::isupper(static_cast<unsigned char>(piece)) != 0;
  if (pieceWhite != bd->whiteToMove()) {
    std::cout << "The piece on " << sq << " belongs to " << (pieceWhite ? "White" : "Black")
              << " - it is " << curTurn() << "'s turn." << std::endl;
    return false;
  }

  char promo = m.gettype2();
  if (promo != 'e') {
    promo = static_cast<char>(std::tolower(static_cast<unsigned char>(promo)));
    if (promo != 'q' && promo != 'r' && promo != 'b' && promo != 'n') {
      std::cout << "A pawn can only promote to Q, R, B or N." << std::endl;
      return false;
    }
  }

  std::vector<Move> legal = bd->legalMoves();
  for (const auto &lm : legal) {
    if (lm.getOpo() != from || lm.getNpo() != to) continue;
    if (lm.ispromoted() &&
        std::tolower(static_cast<unsigned char>(lm.gettype2())) != (promo == 'e' ? 'q' : promo))
      continue;
    applyMove(lm);
    afterPositionChange();
    return true;
  }

  // Explain why the move was rejected.
  std::vector<Move> pseudo;
  bd->pseudoLegalMoves(pseudo);
  bool geometric = false;
  for (const auto &pm : pseudo)
    if (pm.getOpo() == from && pm.getNpo() == to) geometric = true;
  const std::string mv = sq + Move::squareName(to);
  if (geometric && bd->inCheck())
    std::cout << "Illegal move " << mv << ": you must get out of check." << std::endl;
  else if (geometric)
    std::cout << "Illegal move " << mv << ": it would leave your king in check." << std::endl;
  else
    std::cout << "Illegal move " << mv << "." << std::endl;
  return false;
}

void Gamecontroller::computermakemove() {
  if (!gameRunning() || !isComputerTurn()) return;
  auto *c = dynamic_cast<Computer *>(players[curPlayerIndex()].get());
  if (!c) return;
  gd->setStatus(curTurn() + " (" + c->describe() + ") is thinking...");
  Move m = c->decideMove(*this);
  if (m.isNull()) return;
  std::cout << curTurn() << " (" << c->describe() << ") plays " << m.toString();
  if (!c->lastInfo().empty()) std::cout << "   [" << c->lastInfo() << "]";
  std::cout << std::endl;
  applyMove(m);
  afterPositionChange();
}

void Gamecontroller::handleResign() {
  if (!gameRunning()) {
    std::cout << "No game in progress." << std::endl;
    return;
  }
  const std::string winner = bd->whiteToMove() ? "Black" : "White";
  finishGame(winner + "win", curTurn() + " resigned - " + winner + " wins");
  updategd();
  std::cout << winner << " wins!" << std::endl;
  std::cout << "Start a new game with: game <white-player> <black-player>" << std::endl;
}

// ======================= setup mode =======================

void Gamecontroller::beginSetup() {
  inSetup = true;
  gamestate = "none";
  MoveHistory = {};
  RedoS = {};
  bd->makeblank();
  std::cout << "Setup mode. Commands: + K e1 | - e1 | = white/black | default | clear | done"
            << std::endl;
  refresh();
}

void Gamecontroller::makeboardblank() {
  bd->makeblank();
  refresh();
}

void Gamecontroller::setupboard(const std::string &s) {
  std::istringstream iss{s};
  std::string cmd;
  if (!(iss >> cmd)) return;
  if (cmd == "+") {
    std::string piece, square;
    std::pair<int, int> pos;
    if (!(iss >> piece >> square) || piece.size() != 1 ||
        std::string("KQRBNPkqrbnp").find(piece[0]) == std::string::npos ||
        !Move::parseSquare(square, pos)) {
      std::cout << "Usage: + <piece> <square>   e.g. + K e1  (uppercase = White)" << std::endl;
      return;
    }
    bd->placePiece(piece[0], pos);
  } else if (cmd == "-") {
    std::string square;
    std::pair<int, int> pos;
    if (!(iss >> square) || !Move::parseSquare(square, pos)) {
      std::cout << "Usage: - <square>   e.g. - e1" << std::endl;
      return;
    }
    bd->removePiece(pos);
  } else if (cmd == "=") {
    std::string colour;
    iss >> colour;
    for (char &ch : colour) ch = static_cast<char>(std::tolower(static_cast<unsigned char>(ch)));
    if (colour == "white") bd->setTurn(true);
    else if (colour == "black") bd->setTurn(false);
    else {
      std::cout << "Usage: = white   or   = black" << std::endl;
      return;
    }
  } else if (cmd == "default" || cmd == "standard") {
    bd = std::make_unique<Board>();
  } else if (cmd == "clear") {
    bd->makeblank();
  } else if (cmd == "fen") {
    std::string rest;
    std::getline(iss, rest);
    if (!bd->loadFEN(rest)) {
      std::cout << "Invalid FEN." << std::endl;
      return;
    }
  } else {
    std::cout << "Unknown setup command. Use: + K e1 | - e1 | = white/black | default | "
                 "clear | done" << std::endl;
    return;
  }
  refresh();
}

bool Gamecontroller::validsetupboard() {
  int whiteKings = 0, blackKings = 0;
  bool pawnOnEdge = false;
  for (int r = 0; r < 8; ++r)
    for (int c = 0; c < 8; ++c) {
      const char k = bd->keyAt(r, c);
      if (k == 'K') ++whiteKings;
      if (k == 'k') ++blackKings;
      if ((k == 'P' || k == 'p') && (r == 0 || r == 7)) pawnOnEdge = true;
    }
  if (whiteKings != 1 || blackKings != 1) {
    std::cout << "Invalid board: there must be exactly one white king and one black king."
              << std::endl;
    return false;
  }
  if (pawnOnEdge) {
    std::cout << "Invalid board: pawns cannot be on the first or last rank." << std::endl;
    return false;
  }
  if (bd->inCheck(true) || bd->inCheck(false)) {
    std::cout << "Invalid board: neither king may start in check." << std::endl;
    return false;
  }
  return true;
}

void Gamecontroller::endSetup() {
  bd->finishSetup();
  customStart = std::make_unique<Board>(*bd);
  inSetup = false;
  std::cout << "Setup complete (" << curTurn()
            << " to move). The next game will start from this position." << std::endl;
  updategd();
}

// ======================= bonus: undo / redo / hint =======================

void Gamecontroller::undo() {
  if (!cancheat) {
    std::cout << "undo is a bonus feature - run the program with -enablebonus." << std::endl;
    return;
  }
  if (!gameRunning()) {
    std::cout << "No game in progress." << std::endl;
    return;
  }
  auto madeByHuman = [this](const Move &m) {
    const bool white = std::isupper(static_cast<unsigned char>(m.gettype1())) != 0;
    return players[white ? 0 : 1]->isHuman();
  };
  if (MoveHistory.empty() ||
      (hasHuman() && MoveHistory.size() == 1 && !madeByHuman(*MoveHistory.top()))) {
    std::cout << "Nothing to undo." << std::endl;
    return;
  }
  // Take back moves until a human's move has been undone (so the human is to
  // move again); without human players undo a single move.
  int count = 0;
  while (!MoveHistory.empty()) {
    const bool human = madeByHuman(*MoveHistory.top());
    bd->undoMove();
    RedoS.push(std::move(MoveHistory.top()));
    MoveHistory.pop();
    played.pop_back();
    ++count;
    if (human || !hasHuman()) break;
  }
  std::cout << "Took back " << count << (count == 1 ? " move." : " moves.") << std::endl;
  afterPositionChange();
}

void Gamecontroller::redo() {
  if (!cancheat) {
    std::cout << "redo is a bonus feature - run the program with -enablebonus." << std::endl;
    return;
  }
  if (!gameRunning()) {
    std::cout << "No game in progress." << std::endl;
    return;
  }
  if (RedoS.empty()) {
    std::cout << "Nothing to redo." << std::endl;
    return;
  }
  int count = 0;
  do {
    Move m = *RedoS.top();
    RedoS.pop();
    applyMove(m, false);
    ++count;
  } while (!RedoS.empty() && isComputerTurn() && hasHuman());
  std::cout << "Replayed " << count << (count == 1 ? " move." : " moves.") << std::endl;
  afterPositionChange();
}

void Gamecontroller::hint() {
  if (!cancheat) {
    std::cout << "hint is a bonus feature - run the program with -enablebonus." << std::endl;
    return;
  }
  if (!gameRunning()) {
    std::cout << "No game in progress." << std::endl;
    return;
  }
  if (!hintEngine) hintEngine = std::make_unique<Level4>(800, false);
  Board copy{*bd};
  Move m = hintEngine->think(copy, 800);
  std::cout << "Hint: " << m.toString() << "   [" << hintEngine->lastInfo() << "]" << std::endl;
}
