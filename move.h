#ifndef _MOVE_H_
#define _MOVE_H_
#include <utility>
#include <iostream>
#include <iostream>
#include <string>

// I want to store that this:
// <int,int> type (which is the original postion and type)
// <int,int> type (which is the new postion and type)
// <int,int> type (which is the capture postion and type, we can consider capturing nothing as capturing an empty piece)
class Move {
    std::pair<int, int> Npo; // new position
    std::pair<int, int> Opo; // original position
    std::pair<int, int> Cpo; // captured position
    char type1; //origianl type
    char type2; // New type
    char type3; // captured type
    std::string flag = "normal"; // decribel the move

    public:
    bool iscasteld(); //return the flag to tell whether this move is castling or not
    bool ispromoted(); //return the flag to tell whether this move is promotion or not
    bool isenpassant(); //return the flag to tell whether this move is enpassant or not
    std::pair<int,int> getNpo() const; // return the new position after movement
    std::pair<int,int> getOpo() const; // return the original position before movement
    std::pair<int,int> getCpo() const; // return the position of captured piece in this movement
    char gettype1();
    char gettype2();
    char gettype3();
    std::string getflag();

    void setNpo(const std::pair<int, int>& p); // set the pos of Npo
    void setOpo(const std::pair<int, int>& p); // set the pos of Opo
    void setCpo(const std::pair<int, int>& p); // set the pos of Cpo
    void settype1(const char& c); // set the type1 when move
    void settype2(const char& c); // set the type2 when move
    void settype3(const char& c);
    void setcastling(); // make flag castling
    void setpromotion(); // make flag promotion
    void setenpassant(); // make the flag enpassant

};

//std::istream & operator>>(std::istream & in, Move & m); //read the information form input

#endif
