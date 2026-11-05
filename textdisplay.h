#ifndef _TEXTDISPLAY_H_
#define _TEXTDISPLAY_H_
#include "board.h"

// Text view: prints the board in the standard format
//   8 rnbqkbnr      (uppercase = White, lowercase = Black,
//   ...              ' ' = light empty square, '_' = dark empty square)
//   1 RNBQKBNR
//
//     abcdefgh
class Textdisplay {
public:
  void printtd(const Board &bd);
};
#endif
