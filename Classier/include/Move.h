#ifndef MOVE_H
#define MOVE_H

#include <string>
#include "Bitwise.h"
class Danger;
class Board;

struct Move
{
public:
    /*int startX, startY;
    int endX, endY;*/
	int startIndex;
	int endIndex;
    PieceType promotion;
    double score;
	bool null;

	int oldEPData;
	PieceType pieceCaptured;
	PieceType movingPiece;
	char oldCastlingRights;

	Move();
    Move(const Board&);
	Move(int, int, PieceType, const Board&);
	Move(int f, int t, PieceType movingType, PieceType promotion, const Board& gameBoard);
	Move(std::string, const Board&);

    void setScore(double);
    void setGameOverDepth(int);

    double getScore();
    int getGameOverDepth();
	int getMateDistance();

    bool operator==(Move) const;
    bool operator!=(Move) const;

    bool operator<(Move) const;
    bool operator>(Move) const;
    static bool bigger(Move, Move);

	bool isSafe(Board& gameBoard);
	bool makeIfSafe(Board& gameBoard);
    std::string basicAlg();
private:
	PieceType getPieceCaptured(const Board&);
};

#endif
