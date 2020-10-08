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
	static long transHits;
	static long sortAttempts;
	static long hashMoveExistsCount;

    MoveSorter(Move* moveList, int moveCount, Board boardState, TranspositionCache transposition, const MoveLookup& killers, const Move& lastMove, const Move& pvMove);
    void sortMoves();
protected:
private:
    Move* moveList;
    int moveCount;
    Board boardState;
    TranspositionCache transposition;
	const MoveLookup& killers;
	const Move& lastMove;
	const Move& pvMove;

    void assignOrderingScores();
};

#endif // MOVESORTER_H
