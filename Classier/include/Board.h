#ifndef BOARD_H
#define BOARD_H

#include <iostream>
#include <vector>
#include <inttypes.h>
#include "ZobristHasher.h"
#include "Move.h"
#include "BoardInfo.h"

struct Move;
class Piece;
enum PieceType : char;

#define bitBoard uint64_t

class Board
{
public:
	BoardInfo facts;

	double pawnPositionalValue;
	double knightPositionalValue;
	double bishopPositionalValue;

    Board();
    Board(int);
    ~Board();
    Board newCopy() const;

    void countPieces() const;
    int pieceCount() const;
    int pieceCount(bool) const;
    int pieceCount(PieceType) const;
    int pieceCount(PieceType, bool) const;

    Piece getEP() const;
    Piece getSquare(int, int) const;
    Piece findKing(bool) const;
    int getKingX(bool) const;
    int getKingY(bool) const;
	int getEPData() const;
	int getCastlingData() const;

    PieceType getSquareType(const int&, const int&) const;
    bool getSquareColor(int, int) const;

    void setSquare(Piece, int, int);
    void setSquare(PieceType type, bool color, int x, int y);
    void setEP(Piece);
    void setEP(int, int, bool);
    void setKingLocation(bool, int, int);

    size_t getHashCode() const;
    void loadFEN(std::string);
    std::string outputFEN() const;
    double perft(int);
    void generateMoveArray(Move*, int&) const;
    void generateCaptureMoves(Move* moveList, int& moveCounter) const;
    int gameOverCheck() const;
    void makeMove(Move);
	void unmakeMove(Move);

    bool operator==(const Board&) const;

    bool squareIsPopulated(const int& x, const int& y) const;
    bool squareIsType(int, int, int) const;
    double dividePerft(int) const;
    bool getCastlingRights(bool, bool) const;
    char getCastlingRights() const;
    void setCastlingRights(bool, bool, bool);
    void setCastlingRights(char);
    static void throwIfOutOfBounds(int x, int y);

private:
    int kingCoordinates;
    int moveCounter;
    int halfMoveCounter;

	void updatePositionalScore(PieceType type, bool color, int x, int y);
};



#endif
