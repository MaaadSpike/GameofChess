#include "algorithm.h"
#include "gamecontroller.h"
#include <ctime>
#include <random>


std::vector<std::unique_ptr<Piece>> alivePieces(bool iswhite, const Board& bd) {
    std::vector<std::unique_ptr<Piece>> alives;
    if (iswhite) {
        for (int i = 0; i < 8; i++) {
            for (int j = 0; j < 8; j++) {
                char c = bd.getPiece(i, j)->key();
                if ('A' <= c && c <= 'Z' && c != 'K') {
                    alives.emplace_back(bd.getPiece(i, j));
                }
            }
        }
        //std::cout << "I reach line 696" << std::endl;
        return alives;
    } else {
        for (int i = 0; i < 8; i++) {
            for (int j = 0; j < 8; j++) {
                char c = bd.getPiece(i, j)->key();
                if ('a' <= c && c <= 'z' && c != 'e' && c != 'k') {
                    alives.emplace_back(bd.getPiece(i, j));
                }
            }
        }
        //std::cout << "I reach line 707 I have "<< alives.size() << " lives"<< std::endl;
        return alives;
    }
}

vector<Move> Algorithm::generateCheckResponseMoves(const vector<unique_ptr<Piece>>& pieces, const Gamecontroller& gc, bool isWhite) {
    vector<Move> moves;

    Board board{*gc.bd};
    // Find the king and the threatening pieces
    Piece* king = board.getPiece(findKing(board, isWhite).first, findKing(board, isWhite).second).get();
    vector<pair<int, int>> threateningPositions = getThreateningPieces(board, king);
    vector<Piece*> threateningPieces;

    // Convert positions to Piece pointers
    for (const auto& pos : threateningPositions) {
        Piece* piece = board.getPiece(pos.first, pos.second).get();
        if (piece != nullptr) {
            threateningPieces.push_back(piece);
        }
    }

    // Try to capture the threatening piece
    // this only triggers under when threatenignPieces is one singel peice
    if (threateningPieces.size() == 1) {
        Piece* enemy = threateningPieces.front();
        vector<Move> captureMoves = canPieceBeCaptured(pieces, enemy, gc, isWhite);
        moves.insert(moves.end(), captureMoves.begin(), captureMoves.end());
    }

    // Try to block the check
    if (threateningPieces.size() == 1) { // Check can only be blocked if there is a single threat
        vector<Move> blockMoves;
        std::vector<std::unique_ptr<Piece>> va = std::move(alivePieces(isWhite, board));
        for (auto &it1:va) {
            vector<Move> vp = gc.possiblemoves(it1);
            for (auto it:vp) {
                Board board1{*gc.bd};
                board1.movePiece(it.getNpo().first, it.getNpo().second, 
                board1.getPiece(it.getOpo().first, it.getOpo().second));
                if (getThreateningPieces(board1, king).size() == 0) {
                    blockMoves.emplace_back(it);
                } 
            }
        }
        moves.insert(moves.end(), blockMoves.begin(), blockMoves.end());
    }

    // Try to move the king to a safe square
    vector<Move> kingMoves;
    vector<Move> vk = gc.possiblemoves(board.getPiece(findKing(board, isWhite).first, 
    findKing(board, isWhite).second));
    for (auto it:vk) {
        Board board1{*gc.bd};
        board1.movePiece(it.getNpo().first, it.getNpo().second, 
        board1.getPiece(it.getOpo().first, it.getOpo().second));
        if (getThreateningPieces(board1, king).size() == 0) {
            kingMoves.emplace_back(it);
        } 
    }
    moves.insert(moves.end(), kingMoves.begin(), kingMoves.end());

    return moves;
}

Move Algorithm::selectRandomMove(const vector<Move>& moves) {
    if (moves.empty()) {
        return Move(); // Handle no-move scenario (checkmate or stalemate)
    }

    std::mt19937 rng(std::random_device{}()); // Create a random number generator
    std::vector<Move> mutableMoves = moves; // Make a copy of moves since we need to modify it
    std::shuffle(mutableMoves.begin(), mutableMoves.end(), rng); // Shuffle the copy

    return mutableMoves.front(); // Return the first move after shuffling
}


pair<int, int> Algorithm::findKing(const Board& bd, bool isWhite) {
    if (isWhite) {
        for (int i = 0;i < 8; i++) {
            for (int j = 0; j < 8; j++) {
                if (bd.getPiece(i,j)->key() == 'K' &&
                bd.getPiece(i,j)->isWhiteTurn()) {
                    return std::pair<int,int>{i,j};
                }
            }
        }
    } else {
         for (int i = 0;i < 8; i++) {
            for (int j = 0; j < 8; j++) {
                if (bd.getPiece(i,j)->key() == 'k' &&
                !bd.getPiece(i,j)->isWhiteTurn()) {
                    return std::pair<int,int>{i,j};
                }
            }
        }
    }
    return std::pair<int,int>{-1,-1};
}

vector<pair<int, int>> Algorithm::getThreateningPieces(const Board& bd, bool isWhite) {
    std::vector<std::pair<int, int>> threateningPieces;

    // Find the king's position first
    std::pair<int, int> kingPos = findKing(bd, isWhite);

    // Check if the king's position is valid
    if (kingPos.first == -1 && kingPos.second == -1) {
        return threateningPieces; // King not found or invalid position
    }

    // Iterate over the board to find threatening pieces
    for (int x = 0; x < 8; ++x) {
        for (int y = 0; y < 8; ++y) {
            if (isThreateningPiece(bd, x, y, kingPos, isWhite)) {
                threateningPieces.emplace_back(x, y);
            }
        }
    }

    return threateningPieces;
}

bool Algorithm::isThreateningPiece(const Board& bd, int x, int y, std::pair<int, int> kingPos, bool isWhite) {
    std::unique_ptr<Piece> piece = bd.getPiece(x, y);

    // Skip empty squares or pieces of the same color as the current player
    if (piece->isEmptyPiece() || (isWhite == piece->isWhiteTurn())) {
        return false;
    }

    char pieceKey = piece->key();
    bool isPieceWhite = isupper(pieceKey);

    // Check for each piece type
    if (tolower(pieceKey) == 'q') {
        // Queen can move diagonally, vertically, horizontally
        if (canReachKing(bd, x, y, kingPos, true, true)) {
            return true;
        }
    } else if (tolower(pieceKey) == 'b') {
        // Bishop moves diagonally
        if (canReachKing(bd, x, y, kingPos, true, false)) {
            return true;
        }
    } else if (tolower(pieceKey) == 'r') {
        // Rook moves vertically, horizontally
        if (canReachKing(bd, x, y, kingPos, false, true)) {
            return true;
        }
    } else if (tolower(pieceKey) == 'n') {
        // Knight's potential moves
        std::vector<std::pair<int, int>> knightMoves = {
            {x + 2, y + 1}, {x + 2, y - 1},
            {x - 2, y + 1}, {x - 2, y - 1},
            {x + 1, y + 2}, {x + 1, y - 2},
            {x - 1, y + 2}, {x - 1, y - 2}
        };

        for (const auto& move : knightMoves) {
            if (move == kingPos) {
                return true;
            }
        }
    } else if (tolower(pieceKey) == 'p') {
        //need to reconsider through pawn later with grou p meeting
        // Pawn's capture moves
        std::vector<std::pair<int, int>> pawnAttacks;
        if (isPieceWhite) { // Assuming white pawns are at the bottom
            pawnAttacks = {{x - 1, y - 1}, {x - 1, y + 1}};
        } else {
            pawnAttacks = {{x + 1, y - 1}, {x + 1, y + 1}};
        }

        for (const auto& attack : pawnAttacks) {
            if (attack == kingPos) {
                return true;
            }
        }
    }

    return false;
}

//cehck can reach king
bool Algorithm::canReachKing(const Board& bd, int x, int y, std::pair<int, int> kingPos, bool checkDiagonal, bool checkStraight) {
    // Check straight lines (for Rooks and Queens)
    if (checkStraight) {
        if (canReachKingStraight(bd, x, y, kingPos)) {
            return true;
        }
    }

    // Check diagonals (for Bishops and Queens)
    if (checkDiagonal) {
        if (canReachKingDiagonal(bd, x, y, kingPos)) {
            return true;
        }
    }

    return false;
}
//helper fucntions to canreachkign straight and horizontal
bool Algorithm::canReachKingStraight(const Board& bd, int x, int y, std::pair<int, int> kingPos) {
    // Check horizontally
    if (x == kingPos.first) {
        int min_y = std::min(y, kingPos.second);
        int max_y = std::max(y, kingPos.second);
        for (int i = min_y + 1; i < max_y; ++i) {
            if (bd.getPiece(x, i)) { // Check if there is an obstruction
                return false;
            }
        }
        return true;
    }

    // Check vertically
    if (y == kingPos.second) {
        int min_x = std::min(x, kingPos.first);
        int max_x = std::max(x, kingPos.first);
        for (int i = min_x + 1; i < max_x; ++i) {
            if (bd.getPiece(i, y)) { // Check if there is an obstruction
                return false;
            }
        }
        return true;
    }

    return false;
}

bool Algorithm::canReachKingDiagonal(const Board& bd, int x, int y, std::pair<int, int> kingPos) {
    // Calculate differences
    int dx = kingPos.first - x;
    int dy = kingPos.second - y;

    // Check if the move is diagonal
    if (abs(dx) == abs(dy)) {
        int stepX = (dx > 0) ? 1 : -1;
        int stepY = (dy > 0) ? 1 : -1;

        // Check each square along the diagonal for obstructions
        int curX = x + stepX;
        int curY = y + stepY;
        while (curX != kingPos.first && curY != kingPos.second) {
            if (bd.getPiece(curX, curY)) { // Check if there is an obstruction
                return false;
            }
            curX += stepX;
            curY += stepY;
        }
        return true;
    }

    return false;
}

vector<Move> Algorithm::canPieceBeCaptured(const vector<unique_ptr<Piece>>& pieces, Piece* threateningPiece, const Gamecontroller& gameController, bool isWhite) {
    vector<Move> captureMoves;

    for (const auto& piece : pieces) {
        // Skip if it's the king or a piece of the opposite color
        if (tolower(piece->isWhiteTurn()) == 'k' || piece->isWhiteTurn() != isWhite) {
            continue;
        }

        // Get all possible capturing moves for the piece
        vector<Move> potentialCaptureMoves = captureMove(piece, threateningPiece, gameController);

        // Check each potential capture move for validity
        for (const auto& move : potentialCaptureMoves) {
            captureMoves.push_back(move); // Add to the list of capture moves
        }
    }

    return captureMoves;
}

vector<Move> Algorithm::captureMove(const unique_ptr<Piece>& capturingPiece, const Piece* targetPiece, const Gamecontroller& gameController) {
    vector<Move> captureMoves;

    // Get all possible moves for the capturing piece
    vector<Move> possibleMoves = gameController.possiblemoves(capturingPiece);

    // Filter moves to find those that capture the target piece
    for (const auto& move : possibleMoves) {
        if (move.getNpo() == targetPiece->getCoord()) {
            //idk if getposition function is valid
            captureMoves.push_back(move);
        }
    }

    return captureMoves;
}






