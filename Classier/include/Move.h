#ifndef MOVE_H
#define MOVE_H

#include <string>
#include "Bitwise.h"
class Danger;
class Board;

struct Move
{
public:
    int startX, startY;
    int endX, endY;
    PieceType promotion;
    double score;
	bool null;

    Move();
    Move(int, int, int, int);
    Move(int, int, int, int, PieceType);
	Move(std::string);

    void setScore(double);
    void setGameOverDepth(int);

    double getScore();
    int getGameOverDepth();

    bool operator==(Move) const;
    bool operator!=(Move) const;

    bool operator<(Move) const;
    bool operator>(Move) const;
    static bool bigger(Move, Move);

    bool isSafe(Danger);
    std::string basicAlg();
    bool isCapture(const Board&);

    template <class Archive>
    void serialize(Archive& ar)
    {
        ar(startX, startY, endX, endY, promotion, score);
    }
};

#endif
