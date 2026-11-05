#ifndef __WINDOW_H__
#define __WINDOW_H__
#include <X11/Xlib.h>
#include <string>
#include <utility>
#include <vector>

// Thin wrapper around an X11 window.
//
// All drawing goes to an off-screen pixmap (double buffering); present()
// pushes the finished frame to the screen in one step, so the board never
// flickers. The pixmap is also installed as the window background, so the
// X server repaints the window by itself when it is uncovered — no event
// loop is needed while the program waits for console input.
class Xwindow {
  Display *d = nullptr;
  Window w = 0;
  int s = 0;
  GC gc = nullptr;
  Pixmap buffer = 0;
  int width, height;
  XFontStruct *font = nullptr;
  XFontStruct *bigFont = nullptr;
  Atom wmDelete = 0;
  bool closed = false;
  std::vector<unsigned long> colours;

  void processEvents();

public:
  // Throws std::runtime_error if no X display is available.
  Xwindow(int width = 500, int height = 500, const std::string &title = "Chess");
  ~Xwindow();
  Xwindow(const Xwindow &) = delete;
  Xwindow &operator=(const Xwindow &) = delete;

  enum { White = 0, Black, Red, Green, Blue }; // built-in colours

  int addColour(const std::string &spec); // e.g. "#F0D9B5"; returns colour id
  bool isClosed() const { return closed; }

  void fillRectangle(int x, int y, int width, int height, int colour = Black);
  void fillPolygon(const std::vector<std::pair<int, int>> &pts, int colour);
  void drawPolygon(const std::vector<std::pair<int, int>> &pts, int colour, int lineWidth);
  void fillCircle(int cx, int cy, int r, int colour);
  void drawLine(int x1, int y1, int x2, int y2, int colour, int lineWidth = 1);
  void drawString(int x, int y, const std::string &msg, int colour = Black, bool big = false);
  int stringWidth(const std::string &msg, bool big = false) const;
  int fontAscent(bool big = false) const;

  void present(); // show the back buffer
};

#endif
