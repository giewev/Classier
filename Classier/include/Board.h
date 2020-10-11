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

	bool hashingEnabled;
	bool positionalScoresEnabled;

    Board();
    Board(int);
    ~Board();
    Board newCopy() const;

    void countPieces() const;
    int pieceCount() const;
    int pieceCount(bool) const;
    int pieceCount(PieceType) const;
    int pieceCount(PieceType, bool) const;

	void setEP(int);
	void clearEP();
	int getEPData() const;

	int getKingIndex(bool) const;
	int getCastlingData() const;

    PieceType getSquareType(const int& index) const;
    bool getSquareColor(int index) const;

	void setSquare(PieceType type, bool color, int index);
	void setSquare(PieceType type, PieceType prevType, bool color, int index);
    void setKingLocation(bool, int);

    void loadFEN(std::string);
    std::string outputFEN() const;
    double perft(int);
    void generateMoveArray(Move*, int&);
    void generateCaptureMoves(Move* moveList, int& moveCounter);

    void makeMove(Move);
	void unmakeMove(Move);

    bool operator==(const Board&) const;

    bool squareIsPopulated(const int& index) const;
    bool squareIsType(int index, int type) const;
    double dividePerft(int);
    bool getCastlingRights(bool, bool) const;
    char getCastlingRights() const;
    void setCastlingRights(bool, bool, bool);
    void setCastlingRights(char);

	bool squareAttacked(int index, bool byColor);

private:
	int whiteKingIndex;
	int blackKingIndex;
    int moveCounter;
    int halfMoveCounter;

	void updatePositionalScore(PieceType type, bool color, int index);
	void updatePositionalScore(PieceType type, bool color, PieceType deadPieceType, bool deadPieceColor, int index);
};



#endif
