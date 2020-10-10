#ifndef MOVESORTER_H
#define MOVESORTER_H

#include "Board.h"
#include "TranspositionCache.h"
#include "MoveLookup.h"
#include <set>
struct Move;

class MoveSorter
{
public:
    static const double piecePriorities[];

    MoveSorter(Move* moveList, int moveCount, Board boardState, TranspositionCache transposition, const MoveLookup& killers, const Move& lastMove, const Move& pvMove);
    void sortMoves();
	void assignOrderingScores();
protected:
private:
    Move* moveList;
    int moveCount;
    Board boardState;
    TranspositionCache transposition;
	const MoveLookup& killers;
	const Move& lastMove;
	const Move& pvMove;
};

#endif // MOVESORTER_H
