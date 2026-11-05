#include "board.h"
#include "empty.h"
#include <cctype>
#include <cstdlib>
#include <sstream>

namespace {

// ---------- Zobrist hashing ----------
struct Zobrist {
  uint64_t piece[12][64];
  uint64_t castle[16];
  uint64_t ep[8];
  uint64_t side;
  Zobrist() {
    uint64_t s = 0x9E3779B97F4A7C15ULL;
    auto next = [&s]() { // splitmix64
      uint64_t z = (s += 0x9E3779B97F4A7C15ULL);
      z = (z ^ (z >> 30)) * 0xBF58476D1CE4E5B9ULL;
      z = (z ^ (z >> 27)) * 0x94D049BB133111EBULL;
      return z ^ (z >> 31);
    };
    for (auto &p : piece)
      for (auto &v : p) v = next();
    for (auto &v : castle) v = next();
    for (auto &v : ep) v = next();
    side = next();
  }
};
const Zobrist Z;

int pieceIndex(char k) {
  switch (k) {
  case 'P': return 0; case 'N': return 1; case 'B': return 2;
  case 'R': return 3; case 'Q': return 4; case 'K': return 5;
  case 'p': return 6; case 'n': return 7; case 'b': return 8;
  case 'r': return 9; case 'q': return 10; case 'k': return 11;
  default: return -1;
  }
}

inline uint64_t zp(char k, int r, int c) { return Z.piece[pieceIndex(k)][r * 8 + c]; }

// Castling rights that survive a move touching each square.
int castleMask(int r, int c) {
  if (r == 7 && c == 4) return ~(Board::WHITE_OO | Board::WHITE_OOO);
  if (r == 7 && c == 7) return ~Board::WHITE_OO;
  if (r == 7 && c == 0) return ~Board::WHITE_OOO;
  if (r == 0 && c == 4) return ~(Board::BLACK_OO | Board::BLACK_OOO);
  if (r == 0 && c == 7) return ~Board::BLACK_OO;
  if (r == 0 && c == 0) return ~Board::BLACK_OOO;
  return ~0;
}

const char *START_FEN = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

} // namespace

// ======================= construction =======================

Board::Board() {
  board.resize(8);
  for (int i = 0; i < 8; ++i) {
    board[i].resize(8);
    for (int j = 0; j < 8; ++j) {
      board[i][j] = std::make_unique<Empty>(std::pair<int, int>{i, j}, false);
      cells[i][j] = 'e';
    }
  }
  loadFEN(START_FEN);
}

Board::Board(const Board &other)
    : whiteMove{other.whiteMove}, castle{other.castle}, ep{other.ep},
      halfmove{other.halfmove}, fullmove{other.fullmove}, zkey{other.zkey},
      keyHistory{other.keyHistory} {
  board.resize(8);
  for (int i = 0; i < 8; ++i) {
    board[i].resize(8);
    for (int j = 0; j < 8; ++j) {
      board[i][j] = other.board[i][j]->clone();
      cells[i][j] = other.cells[i][j];
    }
  }
  kingPos[0] = other.kingPos[0];
  kingPos[1] = other.kingPos[1];
  // The undo history is not copied: a copy starts "fresh" from this position
  // (but remembers earlier positions for repetition detection).
}

Board &Board::operator=(const Board &other) {
  if (this != &other) {
    Board tmp{other};
    *this = std::move(tmp);
  }
  return *this;
}

// ======================= helpers =======================

std::unique_ptr<Piece> Board::takeEmpty(int r, int c) {
  if (emptyPool.empty()) return std::make_unique<Empty>(std::pair<int, int>{r, c}, false);
  auto p = std::move(emptyPool.back());
  emptyPool.pop_back();
  p->setCoord({r, c});
  return p;
}

std::unique_ptr<Piece> Board::lift(int r, int c) {
  auto p = std::move(board[r][c]);
  board[r][c] = takeEmpty(r, c);
  cells[r][c] = 'e';
  return p;
}

void Board::swapSquares(int r1, int c1, int r2, int c2) {
  std::swap(board[r1][c1], board[r2][c2]);
  board[r1][c1]->setCoord({r1, c1});
  board[r2][c2]->setCoord({r2, c2});
  std::swap(cells[r1][c1], cells[r2][c2]);
}

void Board::recomputeKey() {
  zkey = 0;
  for (int r = 0; r < 8; ++r)
    for (int c = 0; c < 8; ++c)
      if (cells[r][c] != 'e') zkey ^= zp(cells[r][c], r, c);
  zkey ^= Z.castle[castle];
  if (ep.first >= 0) zkey ^= Z.ep[ep.second];
  if (!whiteMove) zkey ^= Z.side;
}

void Board::locateKings() {
  kingPos[0] = kingPos[1] = {-1, -1};
  for (int r = 0; r < 8; ++r)
    for (int c = 0; c < 8; ++c) {
      if (cells[r][c] == 'K') kingPos[1] = {r, c};
      if (cells[r][c] == 'k') kingPos[0] = {r, c};
    }
}

// ======================= attacks / check =======================

bool Board::isAttacked(int r, int c, bool byWhite) const {
  // Pawns
  if (byWhite) {
    if (r + 1 < 8) {
      if (c > 0 && cells[r + 1][c - 1] == 'P') return true;
      if (c < 7 && cells[r + 1][c + 1] == 'P') return true;
    }
  } else {
    if (r - 1 >= 0) {
      if (c > 0 && cells[r - 1][c - 1] == 'p') return true;
      if (c < 7 && cells[r - 1][c + 1] == 'p') return true;
    }
  }
  const char N = byWhite ? 'N' : 'n', K = byWhite ? 'K' : 'k';
  const char B = byWhite ? 'B' : 'b', R = byWhite ? 'R' : 'r';
  const char Q = byWhite ? 'Q' : 'q';

  static const int knight[8][2] = {{-2, -1}, {-2, 1}, {-1, -2}, {-1, 2},
                                   {1, -2},  {1, 2},  {2, -1},  {2, 1}};
  for (auto &o : knight) {
    int rr = r + o[0], cc = c + o[1];
    if (inBounds(rr, cc) && cells[rr][cc] == N) return true;
  }
  static const int king[8][2] = {{-1, -1}, {-1, 0}, {-1, 1}, {0, -1},
                                 {0, 1},   {1, -1}, {1, 0},  {1, 1}};
  for (auto &o : king) {
    int rr = r + o[0], cc = c + o[1];
    if (inBounds(rr, cc) && cells[rr][cc] == K) return true;
  }
  static const int straight[4][2] = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}};
  for (auto &d : straight) {
    int rr = r + d[0], cc = c + d[1];
    while (inBounds(rr, cc)) {
      char k = cells[rr][cc];
      if (k != 'e') {
        if (k == R || k == Q) return true;
        break;
      }
      rr += d[0];
      cc += d[1];
    }
  }
  static const int diag[4][2] = {{-1, -1}, {-1, 1}, {1, -1}, {1, 1}};
  for (auto &d : diag) {
    int rr = r + d[0], cc = c + d[1];
    while (inBounds(rr, cc)) {
      char k = cells[rr][cc];
      if (k != 'e') {
        if (k == B || k == Q) return true;
        break;
      }
      rr += d[0];
      cc += d[1];
    }
  }
  return false;
}

bool Board::inCheck(bool white) const {
  auto kp = kingPos[white ? 1 : 0];
  if (kp.first < 0) return false;
  return isAttacked(kp.first, kp.second, !white);
}

// ======================= move generation =======================

void Board::pseudoLegalMoves(std::vector<Move> &out, bool capturesOnly) const {
  for (int r = 0; r < 8; ++r)
    for (int c = 0; c < 8; ++c) {
      char k = cells[r][c];
      if (k == 'e') continue;
      if ((std::isupper(static_cast<unsigned char>(k)) != 0) != whiteMove) continue;
      board[r][c]->possibleMoves(*this, out, capturesOnly);
    }
}

std::vector<Move> Board::legalMoves() {
  std::vector<Move> pseudo, out;
  pseudo.reserve(64);
  pseudoLegalMoves(pseudo);
  const bool side = whiteMove;
  for (const auto &m : pseudo) {
    makeMove(m);
    if (!inCheck(side)) out.push_back(m);
    undoMove();
  }
  return out;
}

bool Board::hasLegalMove() {
  std::vector<Move> pseudo;
  pseudoLegalMoves(pseudo);
  const bool side = whiteMove;
  for (const auto &m : pseudo) {
    makeMove(m);
    bool ok = !inCheck(side);
    undoMove();
    if (ok) return true;
  }
  return false;
}

// ======================= make / undo =======================

void Board::makeMove(const Move &m) {
  Undo u;
  u.m = m;
  u.castle = castle;
  u.ep = ep;
  u.halfmove = halfmove;
  u.fullmove = fullmove;
  u.zkey = zkey;

  const int fr = m.getOpo().first, fc = m.getOpo().second;
  const int tr = m.getNpo().first, tc = m.getNpo().second;
  const char mk = cells[fr][fc];
  const bool white = std::isupper(static_cast<unsigned char>(mk)) != 0;
  const char lower = static_cast<char>(std::tolower(static_cast<unsigned char>(mk)));

  if (ep.first >= 0) zkey ^= Z.ep[ep.second];

  // Remove captured piece
  if (m.isenpassant()) {
    zkey ^= zp(cells[fr][tc], fr, tc);
    u.captured = lift(fr, tc);
  } else if (cells[tr][tc] != 'e') {
    zkey ^= zp(cells[tr][tc], tr, tc);
    u.captured = lift(tr, tc);
  }

  // Move the piece (swap with the Empty on the destination)
  zkey ^= zp(mk, fr, fc);
  swapSquares(fr, fc, tr, tc);
  u.moverMoved = board[tr][tc]->hasMoved();
  board[tr][tc]->setMoved(true);

  if (m.ispromoted()) {
    char pk = m.gettype2();
    pk = white ? static_cast<char>(std::toupper(static_cast<unsigned char>(pk)))
               : static_cast<char>(std::tolower(static_cast<unsigned char>(pk)));
    u.promotedPawn = std::move(board[tr][tc]);
    board[tr][tc] = Piece::create(pk, {tr, tc});
    board[tr][tc]->setMoved(true);
    cells[tr][tc] = pk;
    zkey ^= zp(pk, tr, tc);
  } else {
    zkey ^= zp(mk, tr, tc);
  }

  if (m.iscasteld()) {
    const int rf = tc == 6 ? 7 : 0, rt = tc == 6 ? 5 : 3;
    const char rk = cells[tr][rf];
    zkey ^= zp(rk, tr, rf) ^ zp(rk, tr, rt);
    swapSquares(tr, rf, tr, rt);
    u.rookMoved = board[tr][rt]->hasMoved();
    board[tr][rt]->setMoved(true);
  }

  if (lower == 'k') kingPos[white ? 1 : 0] = {tr, tc};

  zkey ^= Z.castle[castle];
  castle &= castleMask(fr, fc) & castleMask(tr, tc);
  zkey ^= Z.castle[castle];

  ep = {-1, -1};
  if (lower == 'p' && std::abs(tr - fr) == 2) {
    // Only record the e.p. square if an enemy pawn could actually use it
    const char enemyPawn = white ? 'p' : 'P';
    if ((tc > 0 && cells[tr][tc - 1] == enemyPawn) ||
        (tc < 7 && cells[tr][tc + 1] == enemyPawn)) {
      ep = {(fr + tr) / 2, fc};
      zkey ^= Z.ep[fc];
    }
  }

  halfmove = (lower == 'p' || u.captured) ? 0 : halfmove + 1;
  if (!white) ++fullmove;
  whiteMove = !whiteMove;
  zkey ^= Z.side;

  history.push_back(std::move(u));
  keyHistory.push_back(zkey);
}

void Board::undoMove() {
  if (history.empty()) return;
  if (history.back().isNull) {
    undoNullMove();
    return;
  }
  Undo u = std::move(history.back());
  history.pop_back();
  keyHistory.pop_back();
  whiteMove = !whiteMove;

  const Move &m = u.m;
  const int fr = m.getOpo().first, fc = m.getOpo().second;
  const int tr = m.getNpo().first, tc = m.getNpo().second;

  if (m.iscasteld()) {
    const int rf = tc == 6 ? 7 : 0, rt = tc == 6 ? 5 : 3;
    swapSquares(tr, rt, tr, rf);
    board[tr][rf]->setMoved(u.rookMoved);
  }
  if (m.ispromoted()) {
    board[tr][tc] = std::move(u.promotedPawn);
    board[tr][tc]->setCoord({tr, tc});
    cells[tr][tc] = board[tr][tc]->key();
  }
  swapSquares(fr, fc, tr, tc);
  board[fr][fc]->setMoved(u.moverMoved);
  if (cells[fr][fc] == 'K') kingPos[1] = {fr, fc};
  if (cells[fr][fc] == 'k') kingPos[0] = {fr, fc};

  if (u.captured) {
    const int cr = m.isenpassant() ? fr : tr;
    const int cc = tc;
    emptyPool.push_back(std::move(board[cr][cc]));
    board[cr][cc] = std::move(u.captured);
    board[cr][cc]->setCoord({cr, cc});
    cells[cr][cc] = board[cr][cc]->key();
  }

  castle = u.castle;
  ep = u.ep;
  halfmove = u.halfmove;
  fullmove = u.fullmove;
  zkey = u.zkey;
}

void Board::makeNullMove() {
  Undo u;
  u.isNull = true;
  u.ep = ep;
  u.halfmove = halfmove;
  u.zkey = zkey;
  u.castle = castle;
  u.fullmove = fullmove;
  if (ep.first >= 0) zkey ^= Z.ep[ep.second];
  ep = {-1, -1};
  whiteMove = !whiteMove;
  zkey ^= Z.side;
  halfmove = 0; // repetition checks must not look across a null move
  history.push_back(std::move(u));
  keyHistory.push_back(zkey);
}

void Board::undoNullMove() {
  Undo u = std::move(history.back());
  history.pop_back();
  keyHistory.pop_back();
  whiteMove = !whiteMove;
  ep = u.ep;
  halfmove = u.halfmove;
  zkey = u.zkey;
  castle = u.castle;
  fullmove = u.fullmove;
}

// ======================= draw rules =======================

bool Board::isRepetition() const {
  const int n = static_cast<int>(keyHistory.size());
  const int stop = std::max(0, n - 1 - halfmove);
  for (int i = n - 3; i >= stop; i -= 2)
    if (keyHistory[i] == zkey) return true;
  return false;
}

int Board::repetitionCount() const {
  const int n = static_cast<int>(keyHistory.size());
  const int stop = std::max(0, n - 1 - halfmove);
  int count = 1;
  for (int i = n - 3; i >= stop; i -= 2)
    if (keyHistory[i] == zkey) ++count;
  return count;
}

bool Board::insufficientMaterial() const {
  int minors = 0, whiteBishopColor = -1, blackBishopColor = -1, bishops = 0, knights = 0;
  for (int r = 0; r < 8; ++r)
    for (int c = 0; c < 8; ++c) {
      switch (cells[r][c]) {
      case 'e': case 'k': case 'K': break;
      case 'P': case 'p': case 'R': case 'r': case 'Q': case 'q': return false;
      case 'N': case 'n': ++minors; ++knights; break;
      case 'B': ++minors; ++bishops; whiteBishopColor = (r + c) % 2; break;
      case 'b': ++minors; ++bishops; blackBishopColor = (r + c) % 2; break;
      default: break;
      }
    }
  if (minors <= 1) return true; // K v K, K+minor v K
  // K+B v K+B with bishops on the same colour
  if (minors == 2 && bishops == 2 && knights == 0 && whiteBishopColor >= 0 &&
      blackBishopColor >= 0 && whiteBishopColor == blackBishopColor)
    return true;
  return false;
}

// ======================= setup mode =======================

void Board::placePiece(char c, const std::pair<int, int> &Npo) {
  if (!inBounds(Npo.first, Npo.second)) return;
  board[Npo.first][Npo.second] = Piece::create(c, Npo);
  cells[Npo.first][Npo.second] = board[Npo.first][Npo.second]->key();
  locateKings();
  recomputeKey();
}

void Board::removePiece(const std::pair<int, int> &po) { placePiece('e', po); }

void Board::makeblank() {
  for (int i = 0; i < 8; ++i)
    for (int j = 0; j < 8; ++j) {
      board[i][j] = std::make_unique<Empty>(std::pair<int, int>{i, j}, false);
      cells[i][j] = 'e';
    }
  whiteMove = true;
  castle = 0;
  ep = {-1, -1};
  halfmove = 0;
  fullmove = 1;
  history.clear();
  locateKings();
  recomputeKey();
  keyHistory.assign(1, zkey);
}

void Board::setTurn(bool white) {
  whiteMove = white;
  recomputeKey();
}

void Board::finishSetup() {
  castle = 0;
  auto unmoved = [this](int r, int c, char k) {
    return cells[r][c] == k && !board[r][c]->hasMoved();
  };
  if (unmoved(7, 4, 'K')) {
    if (unmoved(7, 7, 'R')) castle |= WHITE_OO;
    if (unmoved(7, 0, 'R')) castle |= WHITE_OOO;
  }
  if (unmoved(0, 4, 'k')) {
    if (unmoved(0, 7, 'r')) castle |= BLACK_OO;
    if (unmoved(0, 0, 'r')) castle |= BLACK_OOO;
  }
  ep = {-1, -1};
  halfmove = 0;
  history.clear();
  locateKings();
  recomputeKey();
  keyHistory.assign(1, zkey);
}

bool Board::loadFEN(const std::string &fen) {
  std::istringstream iss{fen};
  std::string placement, side = "w", rights = "-", eps = "-";
  int hm = 0, fm = 1;
  if (!(iss >> placement)) return false;
  iss >> side >> rights >> eps >> hm >> fm;

  // Validate placement before touching the board
  {
    int r = 0, c = 0;
    for (char ch : placement) {
      if (ch == '/') { ++r; c = 0; continue; }
      if (std::isdigit(static_cast<unsigned char>(ch))) { c += ch - '0'; continue; }
      if (std::string("KQRBNPkqrbnp").find(ch) == std::string::npos) return false;
      ++c;
      if (c > 8 || r > 7) return false;
    }
    if (r != 7) return false;
  }

  for (int i = 0; i < 8; ++i)
    for (int j = 0; j < 8; ++j) {
      board[i][j] = std::make_unique<Empty>(std::pair<int, int>{i, j}, false);
      cells[i][j] = 'e';
    }
  int r = 0, c = 0;
  for (char ch : placement) {
    if (ch == '/') { ++r; c = 0; continue; }
    if (std::isdigit(static_cast<unsigned char>(ch))) { c += ch - '0'; continue; }
    if (c < 8) {
      board[r][c] = Piece::create(ch, {r, c});
      cells[r][c] = ch;
    }
    ++c;
  }
  whiteMove = (side != "b");
  castle = 0;
  for (char ch : rights) {
    if (ch == 'K') castle |= WHITE_OO;
    if (ch == 'Q') castle |= WHITE_OOO;
    if (ch == 'k') castle |= BLACK_OO;
    if (ch == 'q') castle |= BLACK_OOO;
  }
  // Pieces that lost their castling rights are considered to have moved
  if (!(castle & (WHITE_OO | WHITE_OOO)) && cells[7][4] == 'K') board[7][4]->setMoved();
  if (!(castle & (BLACK_OO | BLACK_OOO)) && cells[0][4] == 'k') board[0][4]->setMoved();
  ep = {-1, -1};
  std::pair<int, int> sq;
  if (eps != "-" && Move::parseSquare(eps, sq)) ep = sq;
  halfmove = hm;
  fullmove = fm;
  history.clear();
  locateKings();
  recomputeKey();
  keyHistory.assign(1, zkey);
  return true;
}

std::string Board::toFEN() const {
  std::string s;
  for (int r = 0; r < 8; ++r) {
    int empty = 0;
    for (int c = 0; c < 8; ++c) {
      if (cells[r][c] == 'e') { ++empty; continue; }
      if (empty) { s += static_cast<char>('0' + empty); empty = 0; }
      s += cells[r][c];
    }
    if (empty) s += static_cast<char>('0' + empty);
    if (r < 7) s += '/';
  }
  s += whiteMove ? " w " : " b ";
  std::string rights;
  if (castle & WHITE_OO) rights += 'K';
  if (castle & WHITE_OOO) rights += 'Q';
  if (castle & BLACK_OO) rights += 'k';
  if (castle & BLACK_OOO) rights += 'q';
  s += rights.empty() ? "-" : rights;
  s += ' ';
  s += ep.first >= 0 ? Move::squareName(ep) : "-";
  s += ' ' + std::to_string(halfmove) + ' ' + std::to_string(fullmove);
  return s;
}
