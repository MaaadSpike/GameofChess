#ifndef _SCOREBOARD_H_
#define _SCOREBOARD_H_
#include <string>
#include <iostream>
class Scoreboard {
    public:
    double whitescores = 0;
    double blackscores = 0;
    void addscores(std::string);
    double getwhites() {
        return whitescores;
    }
    double getblacks() {
        return blackscores;
    }
    void printscoreboard();
};
#endif
