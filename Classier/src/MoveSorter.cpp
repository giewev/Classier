#include <algorithm>
#include "MaterialEvaluator.h"
#include "MoveSorter.h"
#include "MoveLookup.h"
#include <set>

const double MoveSorter::piecePriorities[] = { 1, 2, 6, 7, 4, 3, 5 };
long MoveSorter::transHits = 0;
long MoveSorter::sortAttempts = 0;
long MoveSorter::hashMoveExistsCount = 0;

bool moveBetter(const Move& left, const Move& right)
{
	return left.score > right.score;
}

MoveSorter::MoveSorter(Move* moveList, int moveCount, Board boardState, TranspositionCache transposition, const MoveLookup& killers, const Move& lastMove, const Move& pvMove) : killers(killers), lastMove(lastMove), pvMove(pvMove)
{
    this->moveList = moveList;
    this->moveCount = moveCount;
    this->boardState = boardState;
    this->transposition = transposition;
    this->assignOrderingScores();
}

void MoveSorter::sortMoves()
{
    std::sort(this->moveList, this->moveList + this->moveCount, moveBetter);
}

void MoveSorter::assignOrderingScores()
{
	if (!lastMove.null && (!transposition.bestMove.null || !transposition.cutoffMove.null))
	{
		hashMoveExistsCount++;
	}

	sortAttempts++;
    for (int i = 0; i < this->moveCount; i++)
    {
		if (this->moveList[i] == this->pvMove)
		{
			this->moveList[i].score = 1000;
		}
        else if (this->moveList[i] == this->transposition.bestMove)
        {
			transHits++;
            this->moveList[i].score = 999;
        }
        else if (this->moveList[i] == this->transposition.cutoffMove)
        {
			transHits++;
            this->moveList[i].score = 998;
        }
		else if (this->moveList[i].endX == lastMove.endX && this->moveList[i].endY == lastMove.endY)
		{
			PieceType attackerType = boardState.getSquareType(moveList[i].startX, moveList[i].startY);
			double attackerValue = MaterialEvaluator::pieceValue(attackerType);
			this->moveList[i].score = 800 - attackerValue;
		}
		else {
			PieceType victimType = boardState.getSquareType(moveList[i].endX, moveList[i].endY);
			//double victimValue = piecePriorities[(int)victimType];
			double victimValue = MaterialEvaluator::pieceValue(victimType);
			PieceType attackerType = boardState.getSquareType(moveList[i].startX, moveList[i].startY);
			//double attackerValue = piecePriorities[(int)attackerType];
			double attackerValue = MaterialEvaluator::pieceValue(attackerType);

			if (victimType != PieceType::Empty)
			{
				//this->moveList[i].score = victimValue * MaterialEvaluator::pieceValue(PieceType::Queen);
				//this->moveList[i].score -= attackerValue;
				this->moveList[i].score = 10 * victimValue - attackerValue;
			}
			else if (killers.contains(moveList[i]))
			{
				this->moveList[i].score = 1;
			}
			else 
			{
				this->moveList[i].score = -100;
			}
		}
    }
}
