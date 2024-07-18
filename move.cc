#include "move.h"
#include <iostream>

bool Move::iscasteld() {
    return flag == "castling";
}

bool Move::ispromoted() {
    return flag == "promotion";
}

bool Move::isenpassant() {
    return flag == "enpassant";
}

std::pair<int,int> Move::getNpo() const {
    return Npo;
}

std::pair<int,int> Move::getOpo() const {
    return Opo;
}

std::pair<int,int> Move::getCpo() const {
    return Cpo;
}

char Move::gettype1(){
    return type1;
}
char Move::gettype2(){
    return type2;
}
char Move::gettype3(){
    return type3;
}
std::string Move::getflag(){
    return flag;
}

void Move::setNpo(const std::pair<int, int>& p) {
    Npo = p;
}

void Move::setOpo(const std::pair<int, int>& p) {
    Opo = p;
}

void Move::setCpo(const std::pair<int, int>& p) {
    Cpo = p;
}

void Move::settype1(const char& c) {
    type1 = c;
}

void Move::settype2(const char& c) {
    type2 = c;
}

void Move::settype3(const char& c) {
    type3 = c;
}

void Move::setcastling() {
    flag = "castling";
}

void Move::setpromotion() {
    flag = "promotion";
}

void Move::setenpassant() {
    flag = "promotion";
}

//std::istream & operator>>(std::istream & in, Move & m) {
//    std::string s;
//    std::getline(in, s);
//    if (s.size() != )
//    if (in >> type2) { // try to do an enpassant
//        m.settype2(type2);
//        m.setpromotion();
//    }
//    return in;
//}
