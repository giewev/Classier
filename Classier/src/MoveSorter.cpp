#include <algorithm>
#include "MaterialEvaluator.h"
#include "MoveSorter.h"
#include <set>

const double MoveSorter::piecePriorities[] = { 1, 2, 6, 7, 4, 3, 5 };

MoveSorter::MoveSorter(Move* moveList, int moveCount, Board boardState, TranspositionCache transposition, std::set<Move> killers)
{
    this->moveList = moveList;
    this->moveCount = moveCount;
    this->boardState = boardState;
    this->transposition = transposition;
	this->killers = killers;
    this->assignOrderingScores();
}

void MoveSorter::sortMoves()
{
    std::sort(this->moveList, this->moveList + this->moveCount,
              [this] (const Move& left, const Move& right)
    {
        return this->moveBetter(left, right);
    });
}

bool MoveSorter::moveBetter(const Move& left, const Move& right)
{
    return left.score > right.score;
}

void MoveSorter::assignOrderingScores()
{
    for (int i = 0; i < this->moveCount; i++)
    {
        if (this->moveList[i] == this->transposition.bestMove)
        {
            this->moveList[i].score = 999;
        }
        else if (this->moveList[i] == this->transposition.cutoffMove)
        {
            this->moveList[i].score = 998;
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
				/*this->moveList[i].score = victimValue * piecePriorities[(int)PieceType::King];
				this->moveList[i].score -= attackerValue;*/
				this->moveList[i].score = victimValue - attackerValue;
			}
			else if (killers.find(moveList[i]) != killers.end())
			{
				this->moveList[i].score = 1;
			}
			else 
			{
				this->moveList[i].score = -1;
			}
		}
    }
}
