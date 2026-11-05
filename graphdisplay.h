#ifndef _GRAPHDISPLAY_H_
#define _GRAPHDISPLAY_H_
#include "board.h"
#include "window.h"
#include <memory>
#include <string>

// Graphical view of the board (X11).
//  * vector-drawn pieces, wooden board, rank/file coordinates
//  * last move and checked king are highlighted
//  * only squares that changed are redrawn, and frames are double-buffered
//  * if no X display is available it silently turns itself off
class Graphdisplay {
  std::unique_ptr<Xwindow> x;
  static const int SQ = 80, MARGIN = 28, STATUS = 44;

  char drawnKey[8][8];
  int drawnTint[8][8];
  std::string drawnStatus;
  bool needFull = true;

  // colour ids
  int cBg, cLight, cDark, cLightHi, cDarkHi, cCheck, cCoord, cStatusBg, cStatusText;
  int cWhiteFill, cWhiteLine, cBlackFill, cBlackLine, cBlackDetail;

  void drawSquare(int r, int c, char key, int tint);
  void drawPiece(int px, int py, char key);
  void drawFrame();
  void drawStatus(const std::string &status);

public:
  explicit Graphdisplay(bool enabled = true);
  bool active() const { return x && !x->isClosed(); }
  // lastMove may be nullptr; status is shown under the board.
  void update(const Board &bd, const Move *lastMove, const std::string &status);
  void setStatus(const std::string &status); // update only the status line
};
#endif
