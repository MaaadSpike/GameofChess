#include "textdisplay.h"
#include <iostream>

void Textdisplay::printtd(const Board &bd) {
  std::string out;
  out.reserve(128);
  for (int i = 0; i < 8; ++i) {
    out += static_cast<char>('8' - i);
    out += ' ';
    for (int j = 0; j < 8; ++j) {
      char c = bd.keyAt(i, j);
      if (c == 'e') c = (i + j) % 2 == 0 ? ' ' : '_';
      out += c;
    }
    out += '\n';
  }
  out += "\n  abcdefgh\n";
  std::cout << out << std::flush;
}
