#include "gamecontroller.h"
#include <vector>
#include <iostream>
#include <sstream>
#include "move.h"
#include "computer.h"

Gamecontroller::Gamecontroller(bool cheat) : 
    curPlayerIndex{0}, curstep{0},
    cancheat{cheat},
    gd{std::make_unique<Graphdisplay>()},
    td{std::make_unique<Textdisplay>()},
    bd{std::make_unique<Board>()},
    sb{std::make_unique<Scoreboard>()} {}

void Gamecontroller::initializeGame(std::vector<std::unique_ptr<Player>>&& players) {
    this->players = std::move(players);
    bd = std::make_unique<Board>();
    curstep = 0;
    turn = "White";
    gamestate = "continue";
    MoveHistory = std::stack<std::unique_ptr<Move>>();
    RedoS = std::stack<std::unique_ptr<Move>>();
    updategd();
    updatetd();
}

int Gamecontroller::whomakethisstep() {
    return players[curPlayerIndex]->getId();

}

void Gamecontroller::computermakemove() {
    //std::cout << curPlayerIndex << std::endl;
    int level = players[curPlayerIndex]->getId();
    std::string s;
    if (level == 1) s = "level1";
    else if (level == 2) s = "level2";
    else if (level == 3) s = "level3";
    else s = "level4";
    bool isWhite = false;
    if (turn == "White") {
        isWhite = true;
    }
    Computer c{1, isWhite, s};
    //c.setDifficulty(s);
    c.decideMove(*this);
}

std::string Gamecontroller::curTurn() {
    return turn;
}

void Gamecontroller::changeTurn() {
    if (turn == "White") {
        curPlayerIndex = 1;
        turn = "Black";
    } else {
        curPlayerIndex = 0;
        turn = "White";
    }
}

std::string Gamecontroller::checkState() {
    return gamestate;
}

void Gamecontroller::updatetd(){
    td->printtd(bd);
}

void Gamecontroller::updategd() {
    gd->update(bd);
}

void Gamecontroller::updatesb() {
    sb->addscores(this->checkState());
}

std::pair<int,int> Gamecontroller::findKing() {
    if (turn == "White") {
        for (int i = 0;i < 8; i++) {
            for (int j = 0; j < 8; j++) {
                if (bd->getPiece(i,j)->key() == 'K') {
                    //std::cout << "my king is at " << i << " " << j << std::endl;
                    return std::pair<int,int>{i,j};
                }
            }
        }
    } 
    for (int i = 0;i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            if (bd->getPiece(i,j)->key() == 'k') {
                //std::cout << "my king is at " << i << " " << j << std::endl;
                return std::pair<int,int>{i,j};
            }
        }
    }
    return std::pair<int,int>{0,0}; // never reach(avoid warning)

}

// bool Gamecontroller::VHcheck(const char& type, const int& x, const int& y) {
//     // right side
//     int sum = 0;
//     for (int i = y; i >= 0; i--) {
//         if (bd->getPiece(x,i)->key() == type) {
//             sum += 1;
//             break; // break when find a type
//         } else if (bd->getPiece(i,y)->key() == 'e') {
//             sum += 0;
//         } else {
//             sum += 2;
//         }
//     }
//     if (sum == 1) { // only when there is 0* empty piece between a rook and king
//         return true;
//     }
//     // left side
//     sum = 0;
//     for (int i = y; i <= 7; i++) {
//         if (bd->getPiece(x,i)->key() == type) {
//             sum += 1;
//             break; // break when find a type
//         } else if (bd->getPiece(i, y)->key() == 'e') {
//             sum += 0;
//         } else {
//             sum += 2;
//         }
//     }
//     if (sum == 1) { // only when there is 0* empty piece between a rook and king
//         return true;
//     }
//     // up side
//     sum = 0;
//     for (int i = x; i >= 0; i--) {
//         if (bd->getPiece(i, y)->key() == type) {
//             sum += 1;
//             break; // break when find a type
//         } else if (bd->getPiece(i, y)->key() == 'e') {
//             sum += 0;
//         } else {
//             sum += 2;
//         }
//     }
//     if (sum == 1) { // only when there is 0* empty piece between a rook and king
//         return true;
//     }
//     // down side
//     sum = 0;
//     for (int i = x; i <= 7; i++) {
//         if (bd->getPiece(x, i)->key() == type) {
//             sum += 1;
//             break; // break when find a Rook
//         } else if (bd->getPiece(x, i)->key() == 'e') {
//             sum += 0;
//         } else {
//             sum += 2;
//         }
//     }
//     if (sum == 1) { // only when there is 0* empty piece between a rook and king
//         return true;
//     }

//     return false;
// }

// bool Gamecontroller::Dcheck(const char& type, const int& x, const int& y) {
//     // left upside
//     int sum = 0;
//     int u = x;
//     int v = y;
//     int c = 1;
//     while (1) {
//         u -= c;
//         v -= c;
//         if (u < 0 || v < 0) {
//             break;
//         }
//         if (bd->getPiece(u, v)->key() == type) {
//             sum += 1;
//             break;
//         } else if (bd->getPiece(u, v)->key() == 'e') {
//             sum += 0;
//             c++;
//         } else {
//             sum += 2;
//             c++;
//         } 
//     }
//     if (sum == 1) { // only when there is 0* empty piece between a rook and king
//         return true;
//     }

//     // right upside
//     sum = 0;
//     u = x;
//     v = y;
//     c = 1;
//     while (1) {
//         u -= c;
//         v += c;
//         if (u < 0 || v > 7) {
//             break;
//         }
//         if (bd->getPiece(u, v)->key() == type) {
//             sum += 1;
//             break;
//         } else if (bd->getPiece(u, v)->key() == 'e') {
//             sum += 0;
//             c++;
//         } else {
//             sum += 2;
//             c++;
//         } 
//     }
//     if (sum == 1) { // only when there is 0* empty piece between a rook and king
//         return true;
//     }

//     // left downside
//     sum = 0;
//     u = x;
//     v = y;
//     c = 1;
//     while (1) {
//         u += c;
//         v -= c;
//         if (u > 7 || v < 0) {
//             break;
//         }
//         if (bd->getPiece(u, v)->key() == type) {
//             sum += 1;
//             break;
//         } else if (bd->getPiece(u, v)->key() == 'e') {
//             sum += 0;
//             c++;
//         } else {
//             sum += 2;
//             c++;
//         } 
//     }
//     if (sum == 1) { // only when there is 0* empty piece between a rook and king
//         return true;
//     }
//     // right downside
//     sum = 0;
//     u = x;
//     v = y;
//     c = 1;
//     while (1) {
//         u += c;
//         v += c;
//         if (u > 7 || v > 7) {
//             break;
//         }
//         if (bd->getPiece(u, v)->key() == type) {
//             sum += 1;
//             break;
//         } else if (bd->getPiece(u, v)->key() == 'e') {
//             sum += 0;
//             c++;
//         } else {
//             sum += 2;
//             c++;
//         } 
//     }
//     if (sum == 1) { // only when there is 0* empty piece between a rook and king
//         return true;
//     }
//     return false;
// }

// bool Gamecontroller::ischeck() {
//     std::pair<int,int> pk = findKing();
//     int x = pk.first;
//     int y = pk.second;
    
//     if (turn == "White") {
//         //std::cout << "reach line 248" << std::endl;
//         // case: King check King 
//         for (int i = x - 1; i < x + 2; i++) { // 3x3 matrix
//             for (int j = y - 1; j < y + 2; j++) {
//                 if (0 <= i && i <= 7 && 0 <= j && j <= 7) {
//                     if (bd->getPiece(i,j)->key() == 'k'){
//                         return true;
//                     }
//                 }
//             }
//         }
//         std::cout << "reach line 259" << std::endl;
//         // case: Pawn check King
//         std::vector<std::pair<int,int>> vp;
//         vp.emplace_back(std::pair<int,int>{x-1, y-1});
//         vp.emplace_back(std::pair<int,int>{x-1, y+1});
//         for (auto it:vp) {
//             if (0 <= it.first && it.first <= 7 && 
//             0 <= it.second && it.second <= 7) {
//                 if (bd->getPiece(it.first,it.second)->key() == 'p'){
//                     return true;
//                 }
//             }
//         }
//         std::cout << "reach line 272" << std::endl;
//         // case: Knight check King
//         vp.clear();
//         vp.emplace_back(std::pair<int,int>{x+2, y-1});
//         vp.emplace_back(std::pair<int,int>{x+2, y+1});
//         vp.emplace_back(std::pair<int,int>{x-2, y-1});
//         vp.emplace_back(std::pair<int,int>{x-2, y+1});
//         vp.emplace_back(std::pair<int,int>{x+1, y-2});
//         vp.emplace_back(std::pair<int,int>{x+1, y+2});
//         vp.emplace_back(std::pair<int,int>{x-1, y-2});
//         vp.emplace_back(std::pair<int,int>{x-1, y+2});
//         for (auto it:vp) {
//             if (0 <= it.first && it.first <= 7 && 
//             0 <= it.second && it.second <= 7) {
//                 if (bd->getPiece(it.first,it.second)->key() == 'n'){
//                     return true;
//                 }
//             }
//         }
//         std::cout << "reach line 321" << std::endl;
//         // case: Rook check King
//         if (VHcheck('r', x, y)) {
//             return true;
//         }
//         std::cout << "reach line 326" << std::endl;
//         // case: Bishop check King
//         if (Dcheck('b', x, y)) {
//             return true;
//         }
//         std::cout << "reach line 331" << std::endl;
//         // case: Queen check King
//         if (VHcheck('q', x, y) || Dcheck('q', x, y)) {
//             return true;
//         }
//         std::cout << "reach line 228" << std::endl;
//         return false;
//     } else {
//         // case: King check King 
//         for (int i = x - 1; i < x + 2; i++) { // 3x3 matrix
//             for (int j = y - 1; j < y + 2; j++) {
//                 if (0 <= i && i <= 7 && 0 <= j && j <= 7) {
//                     if (bd->getPiece(i,j)->key() == 'K'){
//                         return true;
//                     }
//                 }
//             }
//         }

//         // case: Pawn check King
//         std::vector<std::pair<int,int>> vp;
//         vp.emplace_back(std::pair<int,int>{x+1, y+1});
//         vp.emplace_back(std::pair<int,int>{x+1, y-1});
//         for (auto it:vp) {
//             if (0 <= it.first && it.first <= 7 && 
//             0 <= it.second && it.second <= 7) {
//                 if (bd->getPiece(it.first,it.second)->key() == 'P'){
//                     return true;
//                 }
//             }
//         }

//         // case: Knight check King
//         vp.clear();
//         vp.emplace_back(std::pair<int,int>{x+2, y-1});
//         vp.emplace_back(std::pair<int,int>{x+2, y+1});
//         vp.emplace_back(std::pair<int,int>{x-2, y-1});
//         vp.emplace_back(std::pair<int,int>{x-2, y+1});
//         vp.emplace_back(std::pair<int,int>{x+1, y-2});
//         vp.emplace_back(std::pair<int,int>{x+1, y+2});
//         vp.emplace_back(std::pair<int,int>{x-1, y-2});
//         vp.emplace_back(std::pair<int,int>{x-1, y+2});
//         for (auto it:vp) {
//             if (0 <= it.first && it.first <= 7 && 
//             0 <= it.second && it.second <= 7) {
//                 if (bd->getPiece(it.first,it.second)->key() == 'N'){
//                     return true;
//                 }
//             }
//         }

//         // case: Rook check King
//         if (VHcheck('R', x, y)) {
//             return true;
//         }

//         // case: Bishop check King
//         if (Dcheck('B', x, y)) {
//             return true;
//         }

//         // case: Queen check King
//         if (VHcheck('Q', x, y) || Dcheck('Q', x, y)) {
//             return true;
//         }
//         return false;
//     }
// }

void Gamecontroller::VHthreats(const char& type, const int& x, const int& y, std::vector<std::unique_ptr<Piece>>& Threats) {
    // left side
    int sum = 0;
    for (int i = y; i >= 0; i--) {
        if (bd->getPiece(x,i)->key() == type) {
            Threats.emplace_back(bd->getPiece(i,y));
            sum += 1;
            if (sum != 1) { // if in this capture trail, there are other piece
                Threats.pop_back();
            }
            break; // break when find a type
        } else if (bd->getPiece(i,y)->key() == 'e') {
            sum += 0;
        } else {
            sum += 2;
        }
    }
    
    // right side
    sum = 0;
    for (int i = y; i <= 7; i++) {
        if (bd->getPiece(x, i)->key() == type) {
            Threats.emplace_back(bd->getPiece(i,y));
            sum += 1;
            if (sum != 1) { // if in this capture trail, there are other piece
                Threats.pop_back();
            }
            break; // break when find a type
        } else if (bd->getPiece(i, y)->key() == 'e') {
            sum += 0;
        } else {
            sum += 2;
        }
    }
       
    // up side
    sum = 0;
    for (int i = x; i >= 0; i--) {
        if (bd->getPiece(i, y)->key() == type) {
            Threats.emplace_back(bd->getPiece(x,i));
            sum += 1;
            if (sum != 1) { // if in this capture trail, there are other piece
                Threats.pop_back();
            }
            break; // break when find a type
        } else if (bd->getPiece(x, i)->key() == 'e') {
            sum += 0;
        } else {
            sum += 2;
        }
    }
       
    // down side
    sum = 0;
    for (int i = x; i <= 7; i++) {
        if (bd->getPiece(i, y)->key() == type) {
            Threats.emplace_back(bd->getPiece(x,i));
            sum += 1;
            if (sum != 1) { // if in this capture trail, there are other piece
                Threats.pop_back();
            }
            break; // break when find a Rook
        } else if (bd->getPiece(x, i)->key() == 'e') {
            sum += 0;
        } else {
            sum += 2;
        }
    }
}

void Gamecontroller::Dthreats(const char& type, const int& x, const int& y, std::vector<std::unique_ptr<Piece>>& Threats) {
    //// left upside
    int sum = 0;
    int u = x;
    int v = y;
    int c = 1;
    while (1) {
       u -= c;
       v -= c;
       if (u < 0 || v < 0) {
           break;
       }
       //std::cout << "type is " << bd->getPiece(u, v)->key() << u << v << std::endl;
       if (bd->getPiece(u, v)->key() == type) {
           Threats.emplace_back(bd->getPiece(u, v));
           sum += 1;
           if (sum != 1) { // if in this capture trail, there are other piece
               Threats.pop_back();
           }
           break;
       } else if (bd->getPiece(u, v)->key() == 'e') {
           sum += 0;
           c++;
       } else {
           sum += 2;
           c++;
       } 
    }
    //std::cout << "I reach line 488" << std::endl;
    // right downside
    sum = 0;
    u = x;
    v = y;
    c = 1;
    //sstd::cout << x << y << " king" << std::endl; 
    while (1) {
       u += c;
       v -= c;
       if (u > 7) break;
       if (v < 0) break;
       //std::cout << "type is " << bd->getPiece(u, v)->key() << u << v << std::endl;
       if (bd->getPiece(u, v)->key() == type) {
           Threats.emplace_back(bd->getPiece(u, v));
           sum += 1;
           if (sum != 1) { // if in this capture trail, there are other piece
               Threats.pop_back();
           }
           break;
       } else if (bd->getPiece(u, v)->key() == 'e') {
           sum += 0;
           c++;
           continue;
       } else {
           sum += 2;
           c++;
           continue;
       } 
    }
    //std::cout << "I reach line 515" << std::endl;
    // right upside
    sum = 0;
    u = x;
    v = y;
    c = 1;
    while (1) {
       u -= c;
       v += c;
       if (u < 0 || v > 7) {
           break;
       }
       //std::cout << "type is " << bd->getPiece(u, v)->key() << u << v << std::endl;
       if (bd->getPiece(u, v)->key() == type) {
           Threats.emplace_back(bd->getPiece(u, v));
           sum += 1;
           if (sum != 1) { // if in this capture trail, there are other piece
               Threats.pop_back();
           }
           break;
       } else if (bd->getPiece(u, v)->key() == 'e') {
           sum += 0;
           c++;
       } else {
           sum += 2;
           c++;
       } 
    }
    //std::cout << "I reach line 542" << std::endl;
    // right downside
    sum = 0;
    u = x;
    v = y;
    c = 1;
    while (1) {
       u += c;
       v += c;
       if (u > 7 || v > 7) {
           break;
       }
       //std::cout << "type is " << bd->getPiece(u, v)->key() << u << v << std::endl;
       if (bd->getPiece(u, v)->key() == type) {
           Threats.emplace_back(bd->getPiece(u, v));
           sum += 1;
           //
           if (sum != 1) { // if in this capture trail, there are other piece
               Threats.pop_back();
           }
           break;
       } else if (bd->getPiece(u, v)->key() == 'e') {
           sum += 0;
           c++;
       } else {
           sum += 2;
           c++;
       } 
    }
    //std::cout << "I reach line 569" << std::endl;
}

// void Gamecontroller::Dthreats(const char& type, const int& startX, const int& startY, std::vector<std::unique_ptr<Piece>>& Threats) {
//     checkDiagonalThreat(type, startX, startY, -1, -1, Threats); // Check left upside
//     checkDiagonalThreat(type, startX, startY, 1, -1, Threats);  // Check right downside
//     checkDiagonalThreat(type, startX, startY, -1, 1, Threats);  // Check left downside
//     checkDiagonalThreat(type, startX, startY, 1, 1, Threats);   // Check right upside
// }

// void Gamecontroller::checkDiagonalThreat(const char& type, int startX, int startY, int deltaX, int deltaY, std::vector<std::unique_ptr<Piece>>& Threats) {
//     int x = startX;
//     int y = startY;

//     while (x >= 0 && x <= 7 && y >= 0 && y <= 7) {
//         char pieceKey = bd->getPiece(x, y)->key();
//         if (pieceKey != 'e' && pieceKey != type) {
//             break; // Found a different piece blocking the path
//         }
//         if (pieceKey == type) {
//             Threats.emplace_back(bd->getPiece(x, y)); // Found the target piece
//             break;
//         }
//         x += deltaX;
//         y += deltaY;
//     }
// }

std::vector<std::unique_ptr<Piece>> Gamecontroller::findthreats() {
    std::pair<int,int> pk = findKing();
    int x = pk.first;
    int y = pk.second;
    std::vector<std::unique_ptr<Piece>> Threats;

    if (turn == "White") {
        // case: King check King 
        for (int i = x - 1; i < x + 2; i++) { // 3x3 matrix
            for (int j = y - 1; j < y + 2; j++) {
                if (0 <= i && i <= 7 && 0 <= j && j <= 7) {
                    if (bd->getPiece(i,j)->key() == 'k'){
                        Threats.emplace_back(bd->getPiece(i,j));
                    }
                }
            }
        }

        // case: Pawn check King
        std::vector<std::pair<int,int>> vp;
        vp.emplace_back(std::pair<int,int>{x-1, y+1});
        vp.emplace_back(std::pair<int,int>{x-1, y-1});
        for (auto it:vp) {
            if (0 <= it.first && it.first <= 7 && 
            0 <= it.second && it.second <= 7) {
                if (bd->getPiece(it.first,it.second)->key() == 'p'){
                    Threats.emplace_back(bd->getPiece(it.first,it.second));
                }
            }
        }

        // case: Knight check King
        vp.clear();
        vp.emplace_back(std::pair<int,int>{x+2, y-1});
        vp.emplace_back(std::pair<int,int>{x+2, y+1});
        vp.emplace_back(std::pair<int,int>{x-2, y-1});
        vp.emplace_back(std::pair<int,int>{x-2, y+1});
        vp.emplace_back(std::pair<int,int>{x+1, y-2});
        vp.emplace_back(std::pair<int,int>{x+1, y+2});
        vp.emplace_back(std::pair<int,int>{x-1, y-2});
        vp.emplace_back(std::pair<int,int>{x-1, y+2});
        for (auto it:vp) {
            if (0 <= it.first && it.first <= 7 && 
            0 <= it.second && it.second <= 7) {
                if (bd->getPiece(it.first,it.second)->key() == 'n'){
                    Threats.emplace_back(bd->getPiece(it.first,it.second));
                }
            }
        }
        // case: Rook check King
        VHthreats('r', x, y, Threats);

        // case: Bishop check King
        Dthreats('b', x, y, Threats);

        // case: Queen check King
        VHthreats('q', x, y, Threats);
        Dthreats('q', x, y, Threats);
        return Threats;
    } else {
       // std::cout << "black threats" << std::endl;
        // case: King check King 
        for (int i = x - 1; i < x + 2; i++) { // 3x3 matrix
            for (int j = y - 1; j < y + 2; j++) {
                if (0 <= i && i <= 7 && 0 <= j && j <= 7) {
                    if (bd->getPiece(x,y)->key() == 'K'){
                        Threats.emplace_back(bd->getPiece(i,j));
                    }
                }
            }
        }
        //std::cout << "I reach line 638" << std::endl;
        // case: Pawn check King
        std::vector<std::pair<int,int>> vp;
        vp.emplace_back(std::pair<int,int>{x+1, y+1});
        vp.emplace_back(std::pair<int,int>{x+1, y-1});
        for (auto it:vp) {
            if (0 <= it.first && it.first <= 7 && 
            0 <= it.second && it.second <= 7) {
                if (bd->getPiece(it.first,it.second)->key() == 'P'){
                    Threats.emplace_back(bd->getPiece(it.first,it.second));
                }
            }
        }
        //std::cout << "I reach line 651" << std::endl;
        // case: Knight check King
        vp.clear();
        vp.emplace_back(std::pair<int,int>{x+2, y-1});
        vp.emplace_back(std::pair<int,int>{x+2, y+1});
        vp.emplace_back(std::pair<int,int>{x-2, y-1});
        vp.emplace_back(std::pair<int,int>{x-2, y+1});
        vp.emplace_back(std::pair<int,int>{x+1, y-2});
        vp.emplace_back(std::pair<int,int>{x+1, y+2});
        vp.emplace_back(std::pair<int,int>{x-1, y-2});
        vp.emplace_back(std::pair<int,int>{x-1, y+2});
        for (auto it:vp) {
            if (0 <= it.first && it.first <= 7 && 
            0 <= it.second && it.second <= 7) {
                if (bd->getPiece(it.first,it.second)->key() == 'N'){
                    Threats.emplace_back(bd->getPiece(it.first,it.second));
                }
            }
        }

       // case: Rook check King
        VHthreats('R', x, y, Threats);

        // case: Bishop check King
        Dthreats('B', x, y, Threats);
        
        // case: Queen check King
        VHthreats('Q', x, y, Threats);
        Dthreats('Q', x, y, Threats);
        return Threats;
    }
}

bool Gamecontroller::ischeck() {
    //std::cout << turn << std::endl;
    if (findthreats().size() > 0) {
        return true;
    } return false;
}

std::vector<std::unique_ptr<Piece>> Gamecontroller::alivePieces() {
    std::vector<std::unique_ptr<Piece>> alives;
    if (turn == "White") {
        for (int i = 0; i < 8; i++) {
            for (int j = 0; j < 8; j++) {
                char c = bd->getPiece(i, j)->key();
                if ('A' <= c && c <= 'Z' && c != 'K') {
                    alives.emplace_back(bd->getPiece(i, j));
                }
            }
        }
        //std::cout << "I reach line 696" << std::endl;
        return alives;
    } else {
        for (int i = 0; i < 8; i++) {
            for (int j = 0; j < 8; j++) {
                char c = bd->getPiece(i, j)->key();
                if ('a' <= c && c <= 'z' && c != 'e' && c != 'k') {
                    alives.emplace_back(bd->getPiece(i, j));
                }
            }
        }
        //std::cout << "I reach line 707 I have "<< alives.size() << " lives"<< std::endl;
        return alives;
    }
}

void Gamecontroller::VHmoves(const int& x, const int& y, const std::unique_ptr<Piece>& p, std::vector<Move> & vm) const {
    // up side
    for (int i = x - 1; i >= 0; i--) {
        Move m;
        m.setOpo(std::pair<int,int>{x,y});
        m.setNpo(std::pair<int,int>{i,y});
        if (p->isMove(m.getOpo(), m.getNpo(), *bd)) {
            vm.emplace_back(m);
        }
    }
    
    // down side
    for (int i = x + 1; i <= 7; i++) {
        Move m;
        m.setOpo(std::pair<int,int>{x,y});
        m.setNpo(std::pair<int,int>{i,y});
        if (p->isMove(m.getOpo(), m.getNpo(), *bd)) {
            vm.emplace_back(m);
        }
    }
    
    // left side
    for (int i = y - 1; i >= 0; i--) {
        Move m;
        m.setOpo(std::pair<int,int>{x,y});
        m.setNpo(std::pair<int,int>{x,i});
        if (p->isMove(m.getOpo(), m.getNpo(), *bd)) {
            vm.emplace_back(m);
        }
    }
    
    // right side
    for (int i = y + 1; i <= 7; i++) {
        Move m;
        m.setOpo(std::pair<int,int>{x,y});
        m.setNpo(std::pair<int,int>{x,i});
        if (p->isMove(m.getOpo(), m.getNpo(), *bd)) {
            vm.emplace_back(m);
        }
    }
}

void Gamecontroller::Dmoves(const int& x, const int& y, const std::unique_ptr<Piece>& p, std::vector<Move> & vm) const {
    // left upside
    int u = x;
    int v = y;
    int c = 1;
    while (1) {
        u -= c;
        v -= c;
        if (u < 0 || v < 0) {
            break;
        }
        Move m;
        m.setOpo(std::pair<int,int>{x,y});
        m.setNpo(std::pair<int,int>{u,v});
        if (p->isMove(m.getOpo(), m.getNpo(), *bd)) {
            vm.emplace_back(m);
        }
        c++;
    }
    
    // left downside
    u = x;
    v = y;
    c = 1;
    while (1) {
        u += c;
        v -= c;
        if (u > 7 || v < 0) {
            break;
        }
        Move m;
        m.setOpo(std::pair<int,int>{x,y});
        m.setNpo(std::pair<int,int>{u,v});
        if (p->isMove(m.getOpo(), m.getNpo(), *bd)) {
            vm.emplace_back(m);
        }
        c++;
    }
    
    //  right upside
    u = x;
    v = y;
    c = 1;
    while (1) {
        u -= c;
        v += c;
        if (u < 0 || v > 7) {
            break;
        }
        Move m;
        m.setOpo(std::pair<int,int>{x,y});
        m.setNpo(std::pair<int,int>{u,v});
        if (p->isMove(m.getOpo(), m.getNpo(), *bd)) {
            vm.emplace_back(m);
        }
        c++;
    }
    
    // right downside
    u = x;
    v = y;
    c = 1;
    while (1) {
        u += c;
        v += c;
        if (u > 7 || v > 7) {
            break;
        }
        Move m;
        m.setOpo(std::pair<int,int>{x,y});
        m.setNpo(std::pair<int,int>{u,v});
        if (p->isMove(m.getOpo(), m.getNpo(), *bd)) {
            vm.emplace_back(m);
        }
        c++;
    }

}

std::vector<Move> Gamecontroller::possiblemoves(const std::unique_ptr<Piece>& p) const { 
    int x = p->getCoord().first;
    int y = p->getCoord().second;
    std::vector<Move> vm;
    // King
    
    if (p->key() == 'K' || p->key() == 'k') {
        for (int i = x - 1; i < x + 2; i++) { // 3x3 matrix
           for (int j = y - 1; j < y + 2; j++) {
               if (0 <= i && i <= 7 && 0 <= j && j <= 7 &&
               p->isMove(p->getCoord(), std::pair<int,int>{i,j}, *bd)) {
                    Move m;
                    m.setOpo(std::pair<int,int>{x,y});
                    m.setNpo(std::pair<int,int>{i,j});                
                    vm.emplace_back(m);
               }
           }
        }
        //std::cout << "I reach 849" << std::endl;
        Move m1;
        m1.setOpo(std::pair<int,int>{x,y});
        m1.setNpo(std::pair<int,int>{x,y+2});
        if (p->isMove(std::pair<int,int>{x, y}, std::pair<int,int>{x, y+2}, *bd)) { // check the castling is valid or not
            vm.emplace_back(m1);
        }
        //std::cout << "I reach 856" << std::endl;
        Move m2;
        m2.setOpo(std::pair<int,int>{x,y});
        m2.setNpo(std::pair<int,int>{x,y-2});
        if (p->isMove(std::pair<int,int>{x, y}, std::pair<int,int>{x, y-2}, *bd)) { // check the castling is valid or not
            vm.emplace_back(m2);
        }      
        return vm;
    }
    // Pawn
    //std::cout << "I reach 867" << std::endl;
    if (p->key() == 'P') { // white player
        int u1 = x-1;
        int v1 = y+1;
        int u2 = x-1;
        int v2 = y-1;
        int u3 = x-1;
        int v3 = y;
        if (u1 >= 0 && v1 <= 7) {
            Move m;
            m.setOpo(std::pair<int,int>{x, y});
            m.setNpo(std::pair<int,int>{u1, v1});
            if (p->isMove(std::pair<int,int>{x, y}, std::pair<int,int>{u1, v1}, *bd)) { // check the move is valid or not
                vm.emplace_back(m);
            }    
        }
        //std::cout << "I reach 884" << std::endl;
        if (u2 >= 0 && v2 >= 0) {
            Move m;
            m.setOpo(std::pair<int,int>{x, y});
            m.setNpo(std::pair<int,int>{u2, v2});
            if (p->isMove(std::pair<int,int>{x, y}, std::pair<int,int>{u2, v2}, *bd)) { // check the move is valid or not
                vm.emplace_back(m);
            }    
        }
        //std::cout << "I reach 893" << std::endl;
        if (v3 >= 0) {
            Move m;
            m.setOpo(std::pair<int,int>{x, y});
            m.setNpo(std::pair<int,int>{u3, v3});
            vm.emplace_back(m);   
        }
        //std::cout << "I reach 900" << std::endl;
        if (!p->hasMoved()) {
            Move m;
            m.setOpo(std::pair<int,int>{x, y});
            m.setNpo(std::pair<int,int>{x-2, y});
            if (p->isMove(std::pair<int,int>{x, y}, std::pair<int,int>{u2, v2}, *bd)) { // check the move is valid or not
                vm.emplace_back(m);
            }  
        }
        //std::cout << "I reach 909" << std::endl;
        
        Move lastmove;
        if (!MoveHistory.empty()) lastmove = *(MoveHistory.top());
        if (p->getCoord().first == 3 &&
        lastmove.gettype1() == 'p' &&
        ((lastmove.getOpo() == std::pair<int,int>{1, p->getCoord().second + 1} && lastmove.getNpo() == std::pair<int,int>{3, p->getCoord().second + 1}) ||
        (lastmove.getOpo() == std::pair<int,int>{1, p->getCoord().second - 1} && lastmove.getNpo() == std::pair<int,int>{3, p->getCoord().second - 1}))){
            if (p->getCoord().second + 1 <= 7) {
                Move m;
                m.setOpo(std::pair<int,int>{x,y});
                m.setNpo(std::pair<int,int>{x-1,y+1});
                vm.emplace_back(m);
            }
            if (p->getCoord().second - 1 >= 0) {
                Move m;
                m.setOpo(std::pair<int,int>{x,y});
                m.setNpo(std::pair<int,int>{x-1,y-1});
                vm.emplace_back(m);
            }
            
        }
    
        //std::cout << "I reach 929" << std::endl;
        return vm;
    }
    //std::cout << "I reach 926" << std::endl;
    if (p->key() == 'p') { // black player
        int u1 = x+1;
        int v1 = y+1;
        int u2 = x+1;
        int v2 = y-1;
        int u3 = x+1;
        int v3 = y;
        if (u1 <= 7 && v1 <= 7) {
            Move m;
            m.setOpo(std::pair<int,int>{x, y});
            m.setNpo(std::pair<int,int>{u1, v1});
            if (p->isMove(std::pair<int,int>{x, y}, std::pair<int,int>{u1, v1}, *bd)) { // check the move is valid or not
                vm.emplace_back(m);
            }    
        }
        //std::cout << "I reach 948" << std::endl;
        if (u2 <= 7 && v2 >= 0) {
            Move m;
            m.setOpo(std::pair<int,int>{x, y});
            m.setNpo(std::pair<int,int>{u2, v2});
            if (p->isMove(std::pair<int,int>{x, y}, std::pair<int,int>{u2, v2}, *bd)) { // check the move is valid or not
                vm.emplace_back(m);
            }    
        }
        //std::cout << "I reach 957" << std::endl;
        if (v3 <= 7) {
            Move m;
            m.setOpo(std::pair<int,int>{x, y});
            m.setNpo(std::pair<int,int>{u3, v3});
            if (p->isMove(std::pair<int,int>{x, y}, std::pair<int,int>{u3, v3}, *bd)) { // check the move is valid or not
                vm.emplace_back(m);
            }   
        }
        //std::cout << "I reach 966" << std::endl;
        //std::cout << "position(" << p->getCoord().first << p->getCoord().second << std::endl;
        if (!p->hasMoved()) {
            //std::cout << "I reach 968" << std::endl;
            Move m;
            m.setOpo(std::pair<int,int>{x, y});
            m.setNpo(std::pair<int,int>{x+2, y});
            if (p->isMove(std::pair<int,int>{x, y}, std::pair<int,int>{u2, v2}, *bd)) { // check the move is valid or not
                //std::cout << "I reach 986" << std::endl;
                vm.emplace_back(m);
            }
           // std::cout << "I reach 988" << std::endl;
        }
        //std::cout << "I reach 975" << std::endl;
        Move lastmove;
        if (!MoveHistory.empty()) lastmove = *(MoveHistory.top());
        if (p->getCoord().first == 4 &&
        lastmove.gettype1() == 'P' &&
        ((lastmove.getOpo() == std::pair<int,int>{6, p->getCoord().second + 1} && lastmove.getNpo() == std::pair<int,int>{4, p->getCoord().second + 1}) ||
        (lastmove.getOpo() == std::pair<int,int>{6, p->getCoord().second - 1} && lastmove.getNpo() == std::pair<int,int>{4, p->getCoord().second - 1}))){
            if (p->getCoord().second + 1 <= 7) {
                Move m;
                m.setOpo(std::pair<int,int>{x,y});
                m.setNpo(std::pair<int,int>{x+1,y+1});
                vm.emplace_back(m);
            }
            if (p->getCoord().second - 1 >= 0) {
                Move m;
                m.setOpo(std::pair<int,int>{x,y});
                m.setNpo(std::pair<int,int>{x+1,y-1});
                vm.emplace_back(m);
            }
            
        }
       // std::cout << "I reach 995" << std::endl;
        return vm;
    }
    //knight
    //std::cout << "I reach 988" << std::endl;
    if (p->key() == 'N' || p->key() == 'n') {
        //std::cout << "I reach 990" << std::endl;
        Move m1;
        m1.setOpo(std::pair<int,int>{x,y});
        m1.setNpo(std::pair<int,int>{x+2, y-1});
        if (0 <= x+2 && x+2 <= 7 &&
         0 <= y-1 && y-1 <= 7 &&
        p->isMove(m1.getOpo(), m1.getNpo(), *bd)) vm.emplace_back(m1);
        
        Move m2;
        m2.setOpo(std::pair<int,int>{x,y});
        m2.setNpo(std::pair<int,int>{x+2, y+1});
        if (0 <= x+2 && x+2 <= 7 &&
         0 <= y+1 && y+1 <= 7 &&
         p->isMove(m2.getOpo(), m2.getNpo(), *bd)) vm.emplace_back(m2);

        Move m3;
        m3.setOpo(std::pair<int,int>{x,y});
        m3.setNpo(std::pair<int,int>{x-2, y-1});
        if (0 <= x-2 && x-2 <= 7 &&
         0 <= y-1 && y-1 <= 7 &&
         p->isMove(m3.getOpo(), m3.getNpo(), *bd)) vm.emplace_back(m3);
        
        Move m4;
        m4.setOpo(std::pair<int,int>{x,y});
        m4.setNpo(std::pair<int,int>{x-2, y+1});
        if (0 <= x-2 && x-2 <= 7 &&
         0 <= y+1 && y+1 <= 7 &&
         p->isMove(m4.getOpo(), m4.getNpo(), *bd)) vm.emplace_back(m4);

        Move m5;
        m5.setOpo(std::pair<int,int>{x,y});
        m5.setNpo(std::pair<int,int>{x+1, y-2});
        if (0 <= x+1 && x+1 <= 7 &&
         0 <= y-2 && y-2 <= 7 &&
         p->isMove(m5.getOpo(), m5.getNpo(), *bd)) vm.emplace_back(m5);

        Move m6;
        m6.setOpo(std::pair<int,int>{x,y});
        m6.setNpo(std::pair<int,int>{x+1, y+2});
        if (0 <= x+1 && x+1 <= 7 &&
         0 <= y+2 && y+2 <= 7 &&
         p->isMove(m6.getOpo(), m6.getNpo(), *bd)) vm.emplace_back(m6);

        Move m7;
        m7.setOpo(std::pair<int,int>{x,y});
        m7.setNpo(std::pair<int,int>{x-1, y-2});
        if (0 <= x-1 && x-1 <= 7 &&
         0 <= y-2 && y-2 <= 7 &&
         p->isMove(m7.getOpo(), m7.getNpo(), *bd)) vm.emplace_back(m7);
        
        Move m8;
        m8.setOpo(std::pair<int,int>{x,y});
        m8.setNpo(std::pair<int,int>{x-1, y+2});
        if (0 <= x-1 && x-1 <= 7 &&
         0 <= y+2 && y+2 <= 7 &&
         p->isMove(m8.getOpo(), m8.getNpo(), *bd)) vm.emplace_back(m8);
        
        return vm;
    }
    //std::cout << "I reach 1032" << std::endl;
    // Rook
    if (p->key() == 'R' || p->key() == 'r') {
        VHmoves(x,y,p,vm);
        return vm;
    }
    
    //Bishop
    if (p->key() == 'B' || p->key() == 'b') {
        Dmoves(x,y,p,vm);
        return vm;
    }

    // Queen
    if (p->key() == 'Q' || p->key() == 'q') {
        VHmoves(x,y,p,vm);
        Dmoves(x,y,p,vm);
        return vm;
    }
    return vm;
}

bool Gamecontroller::dopossiblemoves(const std::unique_ptr<Piece>& p) {
    //std::cout << "position" << p->getCoord().first << ", " << p->getCoord().second << std::endl;
    std::vector<Move> vm = possiblemoves(p);
    //std::cout << "I reach " << std::endl;
    std::unique_ptr<Board> store = std::make_unique<Board>(*bd);
    for (auto it:vm) {
        bd->movePiece(it.getNpo().first, it.getNpo().second, p);
        if (!ischeck()) { // if the possible move doesn't let the player in check
            bd = std::make_unique<Board>(*store); // undo the move
            return true;
        }
        bd = std::make_unique<Board>(*store); // undo the move
    }
    return false;
}

bool Gamecontroller::ischeckmate() {
    if (!ischeck()){
        return false;
    } else {
        if (dopossiblemoves(bd->getPiece(findKing().first, findKing().second))) { // if we can move king
            return false;
        } 
        if (findthreats().size() == 1) { // if there is only one threat
            //std::cout << "I reach 1076" << std::endl;
            std::vector<std::unique_ptr<Piece>> va = std::move(alivePieces());
            for (auto &it:va) { // 
                //std::cout <<  "type: " << it->key() << " pos: "<< it->getCoord().first << it->getCoord().second << std::endl;
                if(dopossiblemoves(it)) { // if dopossiblemvoes is true, which means there is a way make the player not check
                    return false;
                }
            }
            std::cout << "Checkmate! " << turn << " wins!" << std::endl;
            gamestate = turn + "win";
            updatesb();
            return true;
            
        } 
        std::cout << "Checkmate! " << turn << " wins!" << std::endl;
        gamestate = turn + "win";
        updatesb();
        return true; // if there are more than one threats, and we cannot move king, then it is checkmate 
    }
}

bool Gamecontroller::dopossiblemoves2(std::pair<int,int> p1, std::pair<int,int> p2) {
    std::unique_ptr<Board> store = std::make_unique<Board>(*bd); // Store current board state

    bd->movePiece(p2.first, p2.second, bd->getPiece(p1.first, p1.second)); // Execute move
    if (turn == "White") {
        turn == "Black";
    } else {
        turn == "White";
    }
    bool moveLeadsToCheck = ischeck(); // Check if this move results in a check

    bd = std::make_unique<Board>(*store); // Undo the move by restoring the board
    if (turn == "White") {
        turn == "Black";
    } else {
        turn == "White";
    }
    return !moveLeadsToCheck; // Return true if move doesn't lead to check, otherwise false
}

bool Gamecontroller::makeMove(Move &m) {
    //std::cout << "turn: " << turn << std::endl;
    m.settype1(bd->getPiece(m.getOpo().first, m.getOpo().second)->key()); // set original type(type1)
    if (m.gettype1() == 'e') {
        std::cerr << "Cannot move empty piece!" << std::endl;
        return false;
    }
    if (turn == "White" && !('A' <= m.gettype1() && m.gettype1() < 'Z')) {
        std::cerr << "White cannot move Black" << std::endl;
        return false;
    }
    //std::cout << "type: " << m.gettype1() << std::endl;
    if ((turn == "Black") && !('a' <= m.gettype1() && m.gettype1() < 'z')) {
        std::cerr << "Black cannot move White" << std::endl;
        return false;
    }
    char cptype = bd->getPiece(m.getNpo().first, m.getNpo().second)->key(); // potential captured piece type
    if (!ischeck()) {
        //std::cout << "reach line 1089" << std::endl;
        if (curstep >= 1) {
            // check for enpassant
            Move lastmove = *(MoveHistory.top());
            char c = m.gettype1();
            if (c == 'P' && m.getOpo().first == 3 && m.getNpo().first == 2 &&
            (m.getNpo().second == m.getOpo().second + 1 || m.getNpo().second == m.getOpo().second - 1) &&
            0 <= m.getNpo().second && m.getNpo().second <= 7 &&
            lastmove.gettype1() == 'p' &&
            lastmove.getOpo() == std::pair<int,int>{m.getNpo().first-1,m.getNpo().second} &&
            lastmove.getNpo() == std::pair<int,int>{m.getNpo().first+1,m.getNpo().second}) { // white
                if (!dopossiblemoves2(m.getOpo(), m.getNpo()) || findthreats().size() > 0) { // if this move makes player in check
                    std::cout << "it is not a wide choice(You will be in check)" << std::endl;
                    return false;
                }
                std::cout << "White" << "use en passant!" << std::endl;
                bd->movePiece(m.getNpo().first, m.getNpo().second, bd->getPiece(m.getOpo().first, m.getOpo().second));
                m.setenpassant();
                m.settype2(m.gettype1()); // type1 = type2
                m.setCpo(std::pair<int,int>{m.getNpo().first+1, m.getNpo().second});
                m.settype3('p');
                bd->removePiece(m.getCpo());
                MoveHistory.push(std::make_unique<Move>(m)); // push move into history stack
                updategd();
                updatetd();
                changeTurn();
                curstep++;
                return true;
            }
            if (c == 'p' && m.getOpo().first == 4 && m.getNpo().first == 5 &&
            (m.getNpo().second == m.getOpo().second + 1 || m.getNpo().second == m.getOpo().second - 1) &&
            0 <= m.getNpo().second && m.getNpo().second <= 7 &&
            lastmove.gettype1() == 'P' &&
            lastmove.getOpo() == std::pair<int,int>{m.getNpo().first+1,m.getNpo().second} &&
            lastmove.getNpo() == std::pair<int,int>{m.getNpo().first-1,m.getNpo().second}) { // black
                if (!dopossiblemoves2(m.getOpo(), m.getNpo())) { // if this move makes player in check
                    std::cout << "it is not a wide choice(You will be in check)" << std::endl;
                    return false;
                }
                std::cout << "Black" << "use en passant!" << std::endl;
                bd->movePiece(m.getNpo().first, m.getNpo().second, bd->getPiece(m.getOpo().first, m.getOpo().second));
                m.setenpassant();
                m.settype2(m.gettype1()); // type1 = type2
                m.setCpo(std::pair<int,int>{m.getNpo().first-1, m.getNpo().second});
                m.settype3('P');
                bd->removePiece(m.getCpo());
                MoveHistory.push(std::make_unique<Move>(m)); // push move into history stack
                updategd();
                updatetd();
                changeTurn();
                curstep++;
                return true;
            }
        // end check enpassant
        }
        
        if ((!bd->getPiece(m.getOpo().first, m.getOpo().second)->isMove(m.getOpo(), m.getNpo(), *bd))) { // check validity
            std::cerr << "Invalid move!" << std::endl;
            return false;
        } 
        if (!dopossiblemoves2(m.getOpo(), m.getNpo())) { // if this move makes player in check
            std::cout << "it is not a wide choice(You will be in check)" << std::endl;
            return false;
        }
        std::string s = bd->movePiece(m.getNpo().first, m.getNpo().second, bd->getPiece(m.getOpo().first, m.getOpo().second));
        if (s == "normal") { // if it is a normal move
            m.settype2(m.gettype1()); // set the original type as a new piece
            m.setCpo(m.getNpo()); // the the capture postion be the New postion
            m.settype3(cptype); // set the capture type
        } else if (s == "castling") {
            std::cout << turn << "use castling!" << std::endl;
            m.setcastling();
            m.settype2(m.gettype1()); // type1 = type2
            m.setCpo(m.getNpo()); // the the capture postion be the New postion
            m.settype3(cptype); // set the capture type
        } else if (s == "promotion") { // promotion
            std::cout << turn << "use promotion!" << std::endl;
            m.setpromotion();
            m.setCpo(m.getNpo()); // the the capture postion be the New postion
            m.settype3(cptype); // set the capture type
            bd->placePiece(m.gettype2(),m.getNpo()); // place new piece in the position
        }
        MoveHistory.push(std::make_unique<Move>(m)); // push move into history stack
        updategd();
        updatetd();
        changeTurn();
        curstep++;
        return true;
    } else {
        std::cout << "You are in check" << std::endl;
        // check for enpassant
        Move lastmove = *(MoveHistory.top());
        char c = m.gettype1();
        if (c == 'P' && m.getOpo().first == 3 && m.getNpo().first == 2 &&
        (m.getNpo().second == m.getOpo().second + 1 || m.getNpo().second == m.getOpo().second - 1) &&
        0 <= m.getNpo().second && m.getNpo().second <= 7 &&
        lastmove.gettype1() == 'p' &&
        lastmove.getOpo() == std::pair<int,int>{m.getNpo().first-1,m.getNpo().second} &&
        lastmove.getNpo() == std::pair<int,int>{m.getNpo().first+1,m.getNpo().second}) { // white
            if (!dopossiblemoves2(m.getOpo(), m.getNpo())) { // if this move makes player in check
                std::cout << "it is not a wide choice(You will be in check)" << std::endl;
                return false;
            }
            std::cout << "White" << "use en passant!" << std::endl;
            bd->movePiece(m.getNpo().first, m.getNpo().second, bd->getPiece(m.getOpo().first, m.getOpo().second));
            m.setenpassant();
            m.settype2(m.gettype1()); // type1 = type2
            m.setCpo(std::pair<int,int>{m.getNpo().first+1, m.getNpo().second});
            m.settype3('p');
            bd->removePiece(m.getCpo());
            MoveHistory.push(std::make_unique<Move>(m)); // push move into history stack
            updategd();
            updatetd();
            changeTurn();
            curstep++;
            return true;
        }
        if (c == 'p' && m.getOpo().first == 4 && m.getNpo().first == 5 &&
        (m.getNpo().second == m.getOpo().second + 1 || m.getNpo().second == m.getOpo().second - 1) &&
        0 <= m.getNpo().second && m.getNpo().second <= 7 &&
        lastmove.gettype1() == 'P' &&
        lastmove.getOpo() == std::pair<int,int>{m.getNpo().first+1,m.getNpo().second} &&
        lastmove.getNpo() == std::pair<int,int>{m.getNpo().first-1,m.getNpo().second}) { // black
            if (!dopossiblemoves2(m.getOpo(), m.getNpo())) { // if this move makes player in check
                std::cout << "it is not a wide choice(You will be in check)" << std::endl;
                return false;
            }
            std::cout << "Black" << "use en passant!" << std::endl;
            bd->movePiece(m.getNpo().first, m.getNpo().second, bd->getPiece(m.getOpo().first, m.getOpo().second));
            m.setenpassant();
            m.settype2(m.gettype1()); // type1 = type2
            m.setCpo(std::pair<int,int>{m.getNpo().first-1, m.getNpo().second});
            m.settype3('P');
            bd->removePiece(m.getCpo());
            MoveHistory.push(std::make_unique<Move>(m)); // push move into history stack
            updategd();
            updatetd();
            changeTurn();
            curstep++;
            return true;
        }
        // end check enpassant
        if ((!bd->getPiece(m.getOpo().first, m.getOpo().second)->isMove(m.getOpo(), m.getNpo(), *bd))) { // check validity
            std::cerr << "Invalid move!" << std::endl;
            return false;
        } 
        if (dopossiblemoves2(m.getOpo(), m.getNpo())) { // same as previous
            std::string s = bd->movePiece(m.getNpo().first, m.getNpo().second, bd->getPiece(m.getOpo().first, m.getOpo().second));
            if (s == "normal") { // if it is a normal move
                m.settype2(m.gettype1()); // set the original type as a new piece
                m.setCpo(m.getNpo()); // the the capture postion be the New postion
                m.settype3(cptype); // set the capture type
            } else if (s == "enpassant") { // if it is an enpasssant
               std::cout << turn << "use en passant!" << std::endl;
                m.setenpassant();
                m.settype2(m.gettype1()); // type1 = type2
                if (turn == "White") {
                    m.setCpo(std::pair<int,int>{m.getNpo().first, m.getNpo().second+1});
                    m.settype3('p');
                } else {
                    m.setCpo(std::pair<int,int>{m.getNpo().first, m.getNpo().second-1});
                    m.settype3('p');
                }
            } else if (s == "castling") {
                std::cout << turn << "use castling!" << std::endl;
                m.setcastling();
                m.settype2(m.gettype1()); // type1 = type2
                m.setCpo(m.getNpo()); // the the capture postion be the New postion
                m.settype3(cptype); // set the capture type
            } else if (s == "promotion") { // promotion
                std::cout << turn << "use promotion!" << std::endl;
                m.setCpo(m.getNpo()); // the the capture postion be the New postion
                m.settype3(cptype); // set the capture type
                bd->placePiece(m.gettype2(),m.getNpo()); // place new piece in the position
            }
            MoveHistory.push(std::make_unique<Move>(m)); // push move into history stack
            updategd();
            updatetd();
            changeTurn();
            curstep++;
            return true;
        } else {
            std::cout << "Invalid move under check" << std::endl;
            return false;
        }
    }
}

bool Gamecontroller::isdraw() {
    if (!ischeck()) { // make sure the player is not in check
        if (!dopossiblemoves(bd->getPiece(findKing().first, findKing().second))) { // we cannot move king
            for (size_t i = 0; i < alivePieces().size(); i++) { // 
                if(!dopossiblemoves(alivePieces()[i])) { // if we can have a valid move
                    return false;
                }
            }
            std::cout << "Draw!" << std::endl;
            gamestate = "draw";
            //updatesb();
            return true;
        }
    } 
    return false;
}

void Gamecontroller::setupboard(const std::string& s) {
    if (curstep >= 1) {
        std::cerr << "You cannot set up the game now" << std::endl;
        return;
    } else {
        if (s[0] == '+') {
            std::pair<int,int> po{7 - (s[5] - '0' - 1), s[4] - 'a'};
            std::unique_ptr<Piece> p;
            char type = bd->getPiece(po.first, po.second)->key();
            bd->placePiece(s[2], po);
            //std::cout << "the original type is "<< type << std::endl;
            if (type == 'e') { // if there is already a piece on board
                updategd();
                updatetd();
            }
            return;
        } else if (s[0] == '-') {
            std::pair<int,int> po{7 - (s[3] - '0' - 1), s[2] - 'a'};
            bd->removePiece(po);
            updategd();
            updatetd();
            return;
        } else if (s[0] == '=') {
            std::istringstream iss{s};
            std::string colour;
            iss >> colour;
            iss >> colour;
            if (colour != "White" || colour != "Black") {
                std::cerr << "Invalid Colour" << std::endl;
                return;
            }
            turn = colour;
            return;
        } else if (s == "done") {
            std::cout << "quit set up mode" << std::endl;
            return;
        }
    }
}

void Gamecontroller::callprintsb() {
    sb->printscoreboard();
}

bool Gamecontroller::validsetupboard() {
  bool hasWhiteKing = false;
  bool hasBlackKing = false;
  for (int i = 0; i < 8; ++i) {
    for (int y = 0; y < 8; ++y) {
      // Check pawn endzone
      if (tolower(bd->getPiece(i, y)->key()) == 'p' && (i == 0 || i == 7)) {
        return false;
      }
      if (bd->getPiece(i, y)->key() == 'K') {
        // FIX: Does find threats work for both kings?
        std::string temp = turn;
        turn = "White";
        if (findthreats().size()) {
          turn = temp;
          return false;
        }
        turn = temp;
        hasWhiteKing = true;
      }
      if (bd->getPiece(i, y)->key() == 'k') {
        // FIX: Does find threats work for both kings?
        std::string temp = turn;
        turn = "Black";
        if (findthreats().size()) {
          turn = temp;
          return false;
        }
        turn = temp;
        hasBlackKing = true;
      }
    }
  }
  // Check board contains two kings
  if (!(hasBlackKing && hasWhiteKing)) {
    return false;
  }
  return true;
}

void Gamecontroller::makeboardblank() {
    bd->makeblank();
    updategd();
    updatetd();
}

void Gamecontroller::handleResign() {
  if (turn == "White") {
    sb->addscores("Blackwin");
  } else {
    sb->addscores("Whitewin");
  }
}
