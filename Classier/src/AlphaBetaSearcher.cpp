#include "AlphaBetaSearcher.h"
#include "Danger.h"
#include "Evaluation Constants.h"
#include <chrono>
#include <MoveSorter.h>
#include "AlgorithmToggles.h"

AlphaBetaSearcher::AlphaBetaSearcher(Engine& linkedEngine, std::chrono::steady_clock::time_point end) : engine(linkedEngine)
{
	nullMode = false;
	dieTime = end;
	horizonDepth = 0;
	nodesVisited = 0;
}

Move AlphaBetaSearcher::alphaBeta(Board& boardState, int depth)
{
	horizonDepth = depth;
	nodesVisited = 0;
	nodesAtDepths.clear();
	nodesAtDepths.resize(depth + 1);
	killerMoves.clear();
	killerMoves.resize(depth + 1);
	lastMoveMade.clear();
	lastMoveMade.resize(depth + 2);
    return this->alphaBeta(boardState, 1, -1000, 1000);
}

Move AlphaBetaSearcher::alphaBeta(Board& boardState, int depth, double alpha, double beta)
{
    TranspositionCache transposition = engine.getTransposition(boardState);
    if (transposition.bestHeight >= distanceToHorizon(depth))
    {
        return transposition.bestMove;
    }
    else if (transposition.bestHeight >= distanceToHorizon(depth) && // Cutoff info is good enough
        causesAlphaBetaBreak(transposition.cutoffMove.score, alpha, beta, boardState.facts.turn))
    {
        return transposition.cutoffMove;
    }

	nodesVisited += 1;
	nodesAtDepths[depth] += 1;
	Danger safetyinfo = Danger(boardState);

	// Null move heuristic
	// If we are searching a significant depth, we check to see if not making move at all would already cause a cutoff
	if (null_move_enabled && !nullMode && distanceToHorizon(depth) > 3)
	{
		if (!safetyinfo.getCheck())
		{
			nullMode = true;
			Move nullMove = Move(boardState);
			boardState.makeMove(nullMove);
			lastMoveMade[depth] = nullMove;
			Move nullResponse = alphaBeta(boardState, depth + 2, alpha, beta);
			boardState.unmakeMove(nullMove);
			nullMode = false;
			nullMove.score = nullResponse.score;
			if (causesAlphaBetaBreak(nullMove.score, alpha, beta, boardState.facts.turn))
			{
				return nullMove;
			}
		}
	}

    int moveCount = 0;
    Move moveList[220];
    boardState.generateMoveArray(moveList, moveCount);

    unsigned int bestIndex = 0;
    Move returnedMove;

    //Checkmate or StaleMate
    if(moveCount == 0)
    {
        returnedMove = Move();
		if (Danger(boardState).getCheck())
		{
			if (boardState.facts.turn) returnedMove.setScore(-999);
			else returnedMove.setScore(999);
			returnedMove.setGameOverDepth(0);
		}
		else
		{
			returnedMove.setScore(0);
		}
		
        return returnedMove;
    }

	MoveSorter sorter = MoveSorter(moveList, moveCount, boardState, transposition, killerMoves[depth], lastMoveMade[depth - 1], this->variations[0][depth]);
	if (distanceToHorizon(depth) > 4 && transposition.bestMove.null)
	{
		for (int i = 0; i < moveCount; i++)
		{
			lastMoveMade[depth] = moveList[i];
			boardState.makeMove(moveList[i]);
			returnedMove = alphaBeta(boardState, depth + 2, alpha, beta);

			moveList[i].setScore(returnedMove.score);
			if (returnedMove.getGameOverDepth() != -1)
			{
				moveList[i].setGameOverDepth(returnedMove.getGameOverDepth() + 1);
			}

			boardState.unmakeMove(moveList[i]);
		}
	}
	else
	{
		sorter.assignOrderingScores();
	}
	sorter.sortMoves();

    for(int i=0; i<moveCount; i++)
    {
		lastMoveMade[depth] = moveList[i];
        boardState.makeMove(moveList[i]);
		variations[depth][depth] = moveList[i];

        if(distanceToHorizon(depth) == 0)
        {
            if (quiescence_enabled && moveList[i].pieceCaptured != PieceType::Empty)
            {
                moveList[i].score = quiesce(boardState, alpha, beta, moveList[i], 3);
            }
            else
            {
                engine.evaluateMove(boardState, moveList, i);
            }
        }
        else
        {
            returnedMove = alphaBeta(boardState, depth + 1, alpha, beta);

            moveList[i].setScore(returnedMove.score);
            if(returnedMove.getGameOverDepth() != -1)
            {
                moveList[i].setGameOverDepth(returnedMove.getGameOverDepth() + 1);
            }
        }

		boardState.unmakeMove(moveList[i]);
        bestIndex = bestMove(moveList, bestIndex, i, boardState.facts.turn);
        updateAlphaBeta(moveList[bestIndex].score, boardState.facts.turn, alpha, beta);
		if (bestIndex == i)
		{
			for (int j = horizonDepth; j >= depth; j--)
			{
				variations[depth - 1][j] = variations[depth][j];
			}
		}

        if (causesAlphaBetaBreak(moveList[i].score, alpha, beta, boardState.facts.turn))
        {
            engine.updateTranspositionCutoffIfDeeper(boardState, distanceToHorizon(depth), moveList[i]);
			if (moveList[i].pieceCaptured == PieceType::Empty)
			{
				killerMoves[depth].add(moveList[i]);
			}

            return(moveList[i]);
        }

		if ((depth <= 1 && depth < 4) && std::chrono::steady_clock::now() > dieTime)
		{
			return moveList[bestIndex];
		}
    }

    engine.updateTranspositionBestIfDeeper(boardState, distanceToHorizon(depth), moveList[bestIndex]);
    return moveList[bestIndex];
}

bool AlphaBetaSearcher::causesAlphaBetaBreak(double score, double alpha, double beta, bool turn)
{
    return (turn && score > beta) ||
            (!turn && score < alpha);
}

double AlphaBetaSearcher::deltaToAlphaBeta(const double currentScore, const bool turn, const double alpha, const double beta)
{
	if (turn)
	{
		return beta - currentScore;
	}
	else 
	{
		return currentScore - alpha;
	}
}

void AlphaBetaSearcher::updateAlphaBeta(double score, bool turn, double& alpha, double& beta)
{
    if (turn)
    {
        if (score > alpha)
        {
            alpha = score;
        }
    }
    else
    {
        if (score < beta)
        {
            beta = score;
        }
    }
}

double AlphaBetaSearcher::quiesce(Board& boardState, double alpha, double beta, Move lastCap, int depth)
{
    double staticScore = engine.lazyEvaluatePosition(boardState);
    if (depth == 0 || causesAlphaBetaBreak(staticScore, alpha, beta, boardState.facts.turn))
    {
        return staticScore;
    }

    updateAlphaBeta(staticScore, boardState.facts.turn, alpha, beta);
	double minDelta = deltaToAlphaBeta(staticScore, boardState.facts.turn, alpha, beta) - 2;

    int moveCount = 0;
    Move moveList[220];
    boardState.generateCaptureMoves(moveList, moveCount); 
	MoveSorter sorter = MoveSorter(moveList, moveCount, boardState, TranspositionCache(), MoveLookup(), lastCap, Move());
	sorter.sortMoves();

    int bestIndex = -1;
    for (int i = 0; i < moveCount; i++)
    {
		PieceType pieceToCapture = moveList[i].pieceCaptured;
		if ((pieceToCapture == PieceType::Pawn && pawnValue < minDelta) ||
			(pieceToCapture == PieceType::Bishop && bishopValue < minDelta) ||
			(pieceToCapture == PieceType::Knight && knightValue < minDelta) ||
			(pieceToCapture == PieceType::Rook && rookValue < minDelta) ||
			(pieceToCapture == PieceType::Queen && queenValue < minDelta))
		{
			continue;
		}

		boardState.makeMove(moveList[i]);
        double score = quiesce(boardState, alpha, beta, moveList[i], depth - 1);
		boardState.unmakeMove(moveList[i]);

        if (causesAlphaBetaBreak(score, alpha, beta, boardState.facts.turn))
        {
            return score;
        }

		moveList[i].setScore(score);
        updateAlphaBeta(score, boardState.facts.turn, alpha, beta);
        bestIndex = bestMove(moveList, bestIndex, i, boardState.facts.turn);
    }

    if (bestIndex == -1)
    {
        return staticScore;
    }
    else if ((boardState.facts.turn && staticScore > moveList[bestIndex].score) ||
        (!boardState.facts.turn && staticScore < moveList[bestIndex].score))
    {
        return staticScore;
    }
    else
    {
        return moveList[bestIndex].score;
    }
}

int AlphaBetaSearcher::bestMove(Move* moveList, int bestIndex, int currentIndex, bool turn)
{
    if (bestIndex == -1)
    {
        return currentIndex;
    }

    if(turn)
    {
        if (currentIndex == 0 || moveList[currentIndex].score > moveList[bestIndex].score)
        {
            return currentIndex;
        }
    }
    else
    {
        if (currentIndex == 0 || moveList[currentIndex].score < moveList[bestIndex].score)
        {
            return currentIndex;
        }
    }

    if(moveList[currentIndex].score == moveList[bestIndex].score)
    {
        return chooseBetweenEqualMoves(moveList, bestIndex, currentIndex);
    }

    return bestIndex;
}

int AlphaBetaSearcher::chooseBetweenEqualMoves(Move* moveList, int bestIndex, int newIndex)
{
    if(rand() % 2)
    {
        return newIndex;
    }
    else
    {
        return bestIndex;
    }
}

int AlphaBetaSearcher::distanceToHorizon(int depth)
{
	return horizonDepth - depth;
}