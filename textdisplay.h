#ifndef _TEXTDISPLAY_H_
#define _TESTDISPLAY_H_
#include "board.h"
#include <memory>

class Textdisplay{
    public:
    void printtd(const std::unique_ptr<Board>& bd);
};
#endif
