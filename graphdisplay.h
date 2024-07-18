#ifndef _GRAPHDISPLAY_H_
#define _GRAPHDISPLAY_H_
#include "board.h"
#include "window.h"

class Graphdisplay {
    Xwindow x;
    public:
    Graphdisplay();
    void update(const std::unique_ptr<Board> & b);
};
#endif
