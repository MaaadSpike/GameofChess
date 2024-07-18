#ifndef _GAMECONTROLLER_H_
#define _GAMECONTROLLER_H_
#include "board.h"
#include "player.h"
#include "graphdisplay.h"
#include "textdisplay.h"
#include "scoreboard.h"
#include "move.h"
#include <string>
#include <stack>
#include <memory>
#include "move.h"
#include <vector>

//Forward declarations
class Player;
class Algorithm;
class Computer;
class Level1;
class Level2;
class Level3;

class Gamecontroller {
    friend class Algorithm; 
    friend class Computer;
    friend class Level1;
    friend class Level2;
    friend class Level3;
    
    int curPlayerIndex; // set the intial player to first player
    int curstep; // set the intial step to 0
    bool cancheat; // this flag tells whether we can use undo/redo/opening book

    std::unique_ptr<Graphdisplay>  gd;
    std::unique_ptr<Textdisplay> td;
    std::unique_ptr<Board> bd;
    std::unique_ptr<Scoreboard> sb;
    std::vector<std::unique_ptr<Player>> players;
    
    std::string turn = "White"; // set the initial turn to white player
    std::string gamestate = "continue"; // set the initial gamestate to continue

    //doing redo and undo after we finish the core
    std::stack<std::unique_ptr<Move>> MoveHistory; 
    std::stack<std::unique_ptr<Move>> RedoS;

    //helper functions
    std::pair<int,int> findKing(); // return the position of the king of this turn
    bool VHcheck(const char& type, const int& x, const int& y); // VHcheck(char) is function tell whether the piece at postion<x,y> can be captured by piece with type type on vertical/horizontal directions.
    bool Dcheck(const char& type, const int& x, const int& y); // Dcheck(char) is function tell whether the piece at postion<x,y> can be captured by piece with type type on diagonal directions
    void VHthreats(const char& type, const int& x, const int& y, std::vector<std::unique_ptr<Piece>>& Threats); // VHcheck(char) is function stores all the piece attack king in vector Threat with type type on vertical/horizontal d
    void Dthreats(const char& type, const int& x, const int& y, std::vector<std::unique_ptr<Piece>>& Threats); // Dcheck(char) is function stores all the piece attack king in vector Threat with type type on diagonal directions
    void VHmoves(const int& x, const int& y, const std::unique_ptr<Piece>& p, std::vector<Move> & vm) const;
    void Dmoves(const int& x, const int& y, const std::unique_ptr<Piece>& p, std::vector<Move> & vm) const;
    std::vector<std::unique_ptr<Piece>> findthreats(); // find all pieces that attack the king
    std::vector<std::unique_ptr<Piece>> alivePieces(); // find all piece alives expect for king
    std::vector<Move> possiblemoves(const std::unique_ptr<Piece>& p) const; // return a vector of piece's possible move
    bool dopossiblemoves(const std::unique_ptr<Piece>& p);// do all possible moves, if there is one move makes the player non-check return true; otherwise, false
    bool dopossiblemoves2(std::pair<int,int> p1, std::pair<int,int> p2); // do one possible, if this move doesn't make the player in check return true; otherwise, return false

    public:
    Gamecontroller(bool cheat); // constructor
    void initializeGame(std::vector<std::unique_ptr<Player>>&& players); // initialize the players of the games
    std::string curTurn(); // return the white or black in this turn
    void changeTurn(); // make another player's turn
    std::string checkState(); // check the status of this game: whitewin blackwin draw continue
    void updatetd(); // update the textdisplay
    void updategd(); // update the graphdisplay
    void updatesb(); // update the scoreboard

    bool ischeck(); // return true if in currect turn the player is in check; otherwise flase
    bool ischeckmate(); // return true if in currect turn the player is in checkmate; otherwise flase
    bool makeMove(Move& m); // if move is valid, return true and change the board; otherwise, return false
    bool isdraw(); // if the player is not in check, but any movement is invalid, return true; otherwise, return false
    
    //doing redo and undo after we finish the core
    //void turnoncheat(); // turn on cheat model
    //void undo(); // undo the game
    //void redo(); //redo the game
    //void openingbook(); 

    void computermakemove(); 
    int whomakethisstep(); // In cur turn, if it is a player of computer, then return c; if it is human, return h;
    //setup model
    void setupboard(const std::string& s); // read the string and do setup
    
    // check if setupboard is in a valid state
    bool validsetupboard();
    void callprintsb();
    void makeboardblank();
    Board& getBoard() {
        return *bd;
    }
};
#endif
