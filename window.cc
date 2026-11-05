#include "window.h"
#include <X11/Xutil.h>
#include <stdexcept>

namespace {
XFontStruct *loadFirstFont(Display *d, const std::vector<const char *> &names) {
  for (const char *n : names)
    if (XFontStruct *f = XLoadQueryFont(d, n)) return f;
  return nullptr;
}
} // namespace

Xwindow::Xwindow(int width, int height, const std::string &title)
    : width{width}, height{height} {
  d = XOpenDisplay(nullptr);
  if (!d) throw std::runtime_error("cannot open X display");
  s = DefaultScreen(d);
  w = XCreateSimpleWindow(d, RootWindow(d, s), 10, 10, width, height, 0,
                          BlackPixel(d, s), BlackPixel(d, s));
  XStoreName(d, w, title.c_str());
  XSelectInput(d, w, StructureNotifyMask);

  // Ask the window manager to notify us (instead of killing the program)
  // when the user closes the window.
  wmDelete = XInternAtom(d, "WM_DELETE_WINDOW", False);
  XSetWMProtocols(d, w, &wmDelete, 1);

  // Fixed size window
  XSizeHints hints;
  hints.flags = PSize | PMinSize | PMaxSize;
  hints.width = hints.base_width = hints.min_width = hints.max_width = width;
  hints.height = hints.base_height = hints.min_height = hints.max_height = height;
  XSetWMNormalHints(d, w, &hints);

  buffer = XCreatePixmap(d, w, width, height, DefaultDepth(d, s));
  gc = XCreateGC(d, buffer, 0, nullptr);
  XSetLineAttributes(d, gc, 1, LineSolid, CapRound, JoinRound);

  font = loadFirstFont(d, {"-*-helvetica-bold-r-normal--14-*-*-*-*-*-*-*",
                           "-*-dejavu sans-bold-r-normal--14-*-*-*-*-*-*-*",
                           "7x14bold", "9x15bold", "fixed"});
  bigFont = loadFirstFont(d, {"-*-helvetica-bold-r-normal--18-*-*-*-*-*-*-*",
                              "-*-dejavu sans-bold-r-normal--18-*-*-*-*-*-*-*",
                              "9x18bold", "10x20", "9x15bold", "fixed"});

  // Built-in colours (same order as the enum)
  for (const char *c : {"#FFFFFF", "#000000", "#FF0000", "#00A000", "#0000FF"}) addColour(c);

  XMapRaised(d, w);
  XFlush(d);
}

Xwindow::~Xwindow() {
  if (font) XFreeFont(d, font);
  if (bigFont) XFreeFont(d, bigFont);
  XFreePixmap(d, buffer);
  XFreeGC(d, gc);
  XDestroyWindow(d, w);
  XCloseDisplay(d);
}

int Xwindow::addColour(const std::string &spec) {
  XColor xc;
  Colormap cmap = DefaultColormap(d, s);
  unsigned long pixel = BlackPixel(d, s);
  if (XParseColor(d, cmap, spec.c_str(), &xc) && XAllocColor(d, cmap, &xc)) pixel = xc.pixel;
  colours.push_back(pixel);
  return static_cast<int>(colours.size()) - 1;
}

void Xwindow::fillRectangle(int x, int y, int width, int height, int colour) {
  XSetForeground(d, gc, colours[colour]);
  XFillRectangle(d, buffer, gc, x, y, width, height);
}

void Xwindow::fillPolygon(const std::vector<std::pair<int, int>> &pts, int colour) {
  std::vector<XPoint> xp;
  xp.reserve(pts.size());
  for (auto &p : pts) xp.push_back({static_cast<short>(p.first), static_cast<short>(p.second)});
  XSetForeground(d, gc, colours[colour]);
  XFillPolygon(d, buffer, gc, xp.data(), static_cast<int>(xp.size()), Complex, CoordModeOrigin);
}

void Xwindow::drawPolygon(const std::vector<std::pair<int, int>> &pts, int colour, int lineWidth) {
  std::vector<XPoint> xp;
  xp.reserve(pts.size() + 1);
  for (auto &p : pts) xp.push_back({static_cast<short>(p.first), static_cast<short>(p.second)});
  if (!xp.empty()) xp.push_back(xp.front());
  XSetForeground(d, gc, colours[colour]);
  XSetLineAttributes(d, gc, lineWidth, LineSolid, CapRound, JoinRound);
  XDrawLines(d, buffer, gc, xp.data(), static_cast<int>(xp.size()), CoordModeOrigin);
}

void Xwindow::fillCircle(int cx, int cy, int r, int colour) {
  XSetForeground(d, gc, colours[colour]);
  XFillArc(d, buffer, gc, cx - r, cy - r, 2 * r, 2 * r, 0, 360 * 64);
}

void Xwindow::drawLine(int x1, int y1, int x2, int y2, int colour, int lineWidth) {
  XSetForeground(d, gc, colours[colour]);
  XSetLineAttributes(d, gc, lineWidth, LineSolid, CapRound, JoinRound);
  XDrawLine(d, buffer, gc, x1, y1, x2, y2);
}

void Xwindow::drawString(int x, int y, const std::string &msg, int colour, bool big) {
  XFontStruct *f = big ? bigFont : font;
  if (f) XSetFont(d, gc, f->fid);
  XSetForeground(d, gc, colours[colour]);
  XDrawString(d, buffer, gc, x, y, msg.c_str(), static_cast<int>(msg.length()));
}

int Xwindow::stringWidth(const std::string &msg, bool big) const {
  XFontStruct *f = big ? bigFont : font;
  if (!f) return static_cast<int>(msg.size()) * 7;
  return XTextWidth(f, msg.c_str(), static_cast<int>(msg.size()));
}

int Xwindow::fontAscent(bool big) const {
  XFontStruct *f = big ? bigFont : font;
  return f ? f->ascent : 10;
}

void Xwindow::processEvents() {
  while (XPending(d)) {
    XEvent e;
    XNextEvent(d, &e);
    if (e.type == ClientMessage && static_cast<Atom>(e.xclient.data.l[0]) == wmDelete) {
      closed = true;
      XUnmapWindow(d, w);
    }
  }
}

void Xwindow::present() {
  processEvents();
  if (closed) return;
  XSetWindowBackgroundPixmap(d, w, buffer);
  XCopyArea(d, buffer, w, gc, 0, 0, width, height, 0, 0);
  XFlush(d);
}
