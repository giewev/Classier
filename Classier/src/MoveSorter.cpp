#include <algorithm>
#include "MaterialEvaluator.h"
#include "MoveSorter.h"
#include "MoveLookup.h"
#include <set>

const double MoveSorter::piecePriorities[] = { 1, 2, 6, 7, 4, 3, 5 };

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
	moveIndex = 0;
}

void MoveSorter::sortNext()
{
	int bestIndex = moveIndex;
	for (int i = moveIndex + 1; i < moveCount; i++)
	{
		if (moveList[i].score > moveList[bestIndex].score) {
			bestIndex = i;
		}
	}

	std::swap(moveList[bestIndex], moveList[moveIndex]);
	moveIndex++;
}

void MoveSorter::sortMoves()
{
    std::sort(this->moveList, this->moveList + this->moveCount, moveBetter);
}

void MoveSorter::assignOrderingScores()
{
    for (int i = 0; i < this->moveCount; i++)
    {
		if (this->moveList[i] == this->pvMove)
		{
			this->moveList[i].score = 1000;
		}
        else if (this->moveList[i] == this->transposition.bestMove)
        {
            this->moveList[i].score = 999;
        }
        else if (this->moveList[i] == this->transposition.cutoffMove)
        {
            this->moveList[i].score = 998;
        }
		else if (moveList[i].promotion != PieceType::Empty)
		{
			moveList[i].score = 900 + MaterialEvaluator::pieceValue(moveList[i].promotion);
		}
		else if (this->moveList[i].endIndex == lastMove.endIndex)
		{
			PieceType attackerType = moveList[i].movingPiece;
			double attackerValue = MaterialEvaluator::pieceValue(attackerType);
			this->moveList[i].score = 500 - attackerValue;
		}
		else {
			PieceType victimType = moveList[i].pieceCaptured;
			double victimValue = MaterialEvaluator::pieceValue(victimType);
			PieceType attackerType = moveList[i].movingPiece;
			double attackerValue = MaterialEvaluator::pieceValue(attackerType);

			if (victimType != PieceType::Empty)
			{
				this->moveList[i].score = boardState.captureSEE(moveList[i]);
			}
			else if (killers.contains(moveList[i]))
			{
				this->moveList[i].score = 0;
			}
			else 
			{
				this->moveList[i].score = -1;
			}
		}
    }
}
