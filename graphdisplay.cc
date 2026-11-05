#include "graphdisplay.h"
#include <cctype>
#include <iostream>
#include <stdexcept>

namespace {

// Piece artwork in a 100x100 box: a list of filled shapes (polygons and
// circles) that together form the silhouette, plus a few detail lines.
struct Shape {
  bool circle;
  std::vector<std::pair<int, int>> pts; // polygon points
  int cx, cy, r;                        // circle
};
struct Art {
  std::vector<Shape> shapes;
  std::vector<std::vector<int>> details; // {x1,y1,x2,y2}
};

Shape poly(std::vector<std::pair<int, int>> p) { return Shape{false, std::move(p), 0, 0, 0}; }
Shape circ(int cx, int cy, int r) { return Shape{true, {}, cx, cy, r}; }

const std::vector<std::pair<int, int>> BASE = {{20, 90}, {80, 90}, {80, 83}, {73, 77}, {27, 77}, {20, 83}};

const Art &artFor(char k) {
  static const Art pawn{{poly(BASE), poly({{32, 78}, {68, 78}, {59, 56}, {41, 56}}),
                         poly({{37, 59}, {63, 59}, {63, 52}, {37, 52}}), circ(50, 40, 13)},
                        {}};
  static const Art rook{{poly(BASE), poly({{30, 78}, {70, 78}, {66, 40}, {34, 40}}),
                         poly({{26, 44}, {74, 44}, {74, 17}, {64, 17}, {64, 25}, {56, 25},
                               {56, 17}, {44, 17}, {44, 25}, {36, 25}, {36, 17}, {26, 17}})},
                        {{33, 44, 67, 44}, {31, 70, 69, 70}}};
  static const Art knight{{poly(BASE),
                           poly({{28, 78}, {74, 78}, {74, 60}, {71, 42}, {64, 28}, {56, 20},
                                 {54, 9},  {48, 18}, {40, 21}, {30, 30}, {20, 42}, {17, 51},
                                 {23, 56}, {32, 53}, {41, 48}, {47, 47}, {37, 61}, {29, 70}})},
                          {{36, 30, 39, 33}, {58, 24, 69, 46}}};
  static const Art bishop{{poly(BASE), poly({{33, 78}, {67, 78}, {58, 60}, {42, 60}}),
                           poly({{36, 63}, {64, 63}, {64, 56}, {36, 56}}),
                           poly({{50, 17}, {60, 26}, {65, 38}, {62, 50}, {56, 57}, {44, 57},
                                 {38, 50}, {35, 38}, {40, 26}}),
                           circ(50, 13, 6)},
                          {{56, 29, 47, 42}, {36, 63, 64, 63}}};
  static const Art queen{{poly(BASE), poly({{30, 78}, {70, 78}, {63, 52}, {37, 52}}),
                          poly({{31, 56}, {69, 56}, {79, 24}, {71, 40}, {64, 17}, {57, 38},
                                {50, 12}, {43, 38}, {36, 17}, {29, 40}, {21, 24}}),
                          circ(21, 22, 6), circ(36, 15, 6), circ(50, 10, 6), circ(64, 15, 6),
                          circ(79, 22, 6)},
                         {{33, 56, 67, 56}, {32, 68, 68, 68}}};
  static const Art king{{poly(BASE), poly({{30, 78}, {70, 78}, {64, 48}, {36, 48}}),
                         poly({{33, 51}, {67, 51}, {74, 35}, {63, 27}, {50, 31}, {37, 27}, {26, 35}}),
                         poly({{46, 6}, {54, 6}, {54, 31}, {46, 31}}),
                         poly({{39, 12}, {61, 12}, {61, 20}, {39, 20}})},
                        {{35, 51, 65, 51}, {32, 66, 68, 66}}};
  switch (std::tolower(static_cast<unsigned char>(k))) {
  case 'k': return king;
  case 'q': return queen;
  case 'r': return rook;
  case 'b': return bishop;
  case 'n': return knight;
  default: return pawn;
  }
}

enum Tint { NONE = 0, LASTMOVE = 1, CHECK = 2 };

} // namespace

Graphdisplay::Graphdisplay(bool enabled) {
  if (!enabled) return;
  try {
    x = std::make_unique<Xwindow>(SQ * 8 + 2 * MARGIN, SQ * 8 + 2 * MARGIN + STATUS, "Chess");
  } catch (const std::exception &) {
    std::cout << "(No X display found - running in text-only mode.)" << std::endl;
    return;
  }
  cBg = x->addColour("#262421");
  cLight = x->addColour("#F0D9B5");
  cDark = x->addColour("#B58863");
  cLightHi = x->addColour("#CDD26A");
  cDarkHi = x->addColour("#AAA23A");
  cCheck = x->addColour("#E2554F");
  cCoord = x->addColour("#BFB6A8");
  cStatusBg = x->addColour("#1B1A18");
  cStatusText = x->addColour("#EDE6DA");
  cWhiteFill = x->addColour("#FBFAF5");
  cWhiteLine = x->addColour("#232323");
  cBlackFill = x->addColour("#353330");
  cBlackLine = x->addColour("#0C0C0C");
  cBlackDetail = x->addColour("#9C978F");
  for (auto &row : drawnKey)
    for (char &k : row) k = '?';
}

void Graphdisplay::drawFrame() {
  const int W = SQ * 8 + 2 * MARGIN;
  x->fillRectangle(0, 0, W, SQ * 8 + 2 * MARGIN + STATUS, cBg);
  const int asc = x->fontAscent();
  for (int i = 0; i < 8; ++i) {
    std::string rank(1, static_cast<char>('8' - i));
    std::string file(1, static_cast<char>('a' + i));
    const int cy = MARGIN + i * SQ + SQ / 2 + asc / 2 - 1;
    x->drawString((MARGIN - x->stringWidth(rank)) / 2, cy, rank, cCoord);
    x->drawString(W - MARGIN + (MARGIN - x->stringWidth(rank)) / 2, cy, rank, cCoord);
    const int cx = MARGIN + i * SQ + (SQ - x->stringWidth(file)) / 2;
    x->drawString(cx, (MARGIN + asc) / 2 - 1, file, cCoord);
    x->drawString(cx, MARGIN + 8 * SQ + (MARGIN + asc) / 2 - 1, file, cCoord);
  }
}

void Graphdisplay::drawPiece(int px, int py, char key) {
  const Art &art = artFor(key);
  const bool white = std::isupper(static_cast<unsigned char>(key)) != 0;
  const int fill = white ? cWhiteFill : cBlackFill;
  const int line = white ? cWhiteLine : cBlackLine;
  const int detail = white ? cWhiteLine : cBlackDetail;
  auto tx = [&](int v) { return px + v * SQ / 100; };
  auto ty = [&](int v) { return py + v * SQ / 100; };
  auto scaled = [&](const std::vector<std::pair<int, int>> &pts) {
    std::vector<std::pair<int, int>> out;
    for (auto &p : pts) out.push_back({tx(p.first), ty(p.second)});
    return out;
  };

  // Pass 1: thick outline of every shape -> one merged silhouette border.
  for (const auto &s : art.shapes) {
    if (s.circle) {
      x->fillCircle(tx(s.cx), ty(s.cy), s.r * SQ / 100 + 2, line);
    } else {
      auto pts = scaled(s.pts);
      x->fillPolygon(pts, line);
      x->drawPolygon(pts, line, 4);
    }
  }
  // Pass 2: fill colour on top.
  for (const auto &s : art.shapes) {
    if (s.circle) x->fillCircle(tx(s.cx), ty(s.cy), s.r * SQ / 100, fill);
    else x->fillPolygon(scaled(s.pts), fill);
  }
  // Pass 3: details.
  for (const auto &d : art.details) x->drawLine(tx(d[0]), ty(d[1]), tx(d[2]), ty(d[3]), detail, 2);
}

void Graphdisplay::drawSquare(int r, int c, char key, int tint) {
  const int px = MARGIN + c * SQ, py = MARGIN + r * SQ;
  const bool light = (r + c) % 2 == 0;
  int colour = light ? cLight : cDark;
  if (tint == LASTMOVE) colour = light ? cLightHi : cDarkHi;
  x->fillRectangle(px, py, SQ, SQ, colour);
  if (tint == CHECK) {
    x->fillCircle(px + SQ / 2, py + SQ / 2, SQ / 2 - 2, cCheck);
  }
  if (key != 'e') drawPiece(px, py, key);
}

void Graphdisplay::drawStatus(const std::string &status) {
  const int W = SQ * 8 + 2 * MARGIN;
  const int top = SQ * 8 + 2 * MARGIN;
  x->fillRectangle(0, top, W, STATUS, cStatusBg);
  const int asc = x->fontAscent(true);
  x->drawString((W - x->stringWidth(status, true)) / 2, top + (STATUS + asc) / 2 - 2, status,
                cStatusText, true);
  drawnStatus = status;
}

void Graphdisplay::update(const Board &bd, const Move *lastMove, const std::string &status) {
  if (!active()) return;
  if (needFull) {
    drawFrame();
    for (int r = 0; r < 8; ++r)
      for (int c = 0; c < 8; ++c) {
        drawnKey[r][c] = '?';
        drawnTint[r][c] = -1;
      }
    drawnStatus = "\x01";
  }
  // Work out highlight tints
  int tint[8][8] = {{0}};
  if (lastMove && !lastMove->isNull()) {
    tint[lastMove->getOpo().first][lastMove->getOpo().second] = LASTMOVE;
    tint[lastMove->getNpo().first][lastMove->getNpo().second] = LASTMOVE;
  }
  for (bool white : {true, false}) {
    auto kp = bd.findKing(white);
    if (kp.first >= 0 && bd.inCheck(white)) tint[kp.first][kp.second] = CHECK;
  }
  // Redraw only what changed
  for (int r = 0; r < 8; ++r)
    for (int c = 0; c < 8; ++c) {
      const char k = bd.keyAt(r, c);
      if (k != drawnKey[r][c] || tint[r][c] != drawnTint[r][c]) {
        drawSquare(r, c, k, tint[r][c]);
        drawnKey[r][c] = k;
        drawnTint[r][c] = tint[r][c];
      }
    }
  if (status != drawnStatus) drawStatus(status);
  needFull = false;
  x->present();
}

void Graphdisplay::setStatus(const std::string &status) {
  if (!active() || needFull) return;
  if (status != drawnStatus) {
    drawStatus(status);
    x->present();
  }
}
