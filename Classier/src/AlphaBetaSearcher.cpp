#include "AlphaBetaSearcher.h"
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
	bool inCheck = boardState.squareAttacked(boardState.getKingIndex(boardState.facts.turn), !boardState.facts.turn);

	// Null move heuristic
	// If we are searching a significant depth, we check to see if not making move at all would already cause a cutoff
	if (null_move_enabled && !nullMode && distanceToHorizon(depth) > 3)
	{
		if (!inCheck)
		{
			nullMode = true;
			Move nullMove = Move(boardState);
			boardState.makeMove(nullMove);
			lastMoveMade[depth] = nullMove;
			Move nullResponse = alphaBeta(boardState, depth + 3, alpha, beta);
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
    boardState.generatePseudoMoveArray(moveLists[depth], moveCount, false);

    unsigned int bestIndex = -1;
	int legalMoves = 0;
    Move returnedMove;

	MoveSorter sorter = MoveSorter(moveLists[depth], moveCount, boardState, transposition, killerMoves[depth], lastMoveMade[depth - 1], this->variations[0][depth]);
	if (internal_search_ordering && distanceToHorizon(depth) > 4 && transposition.bestMove.null)
	{
		for (int i = 0; i < moveCount; i++)
		{
			lastMoveMade[depth] = moveLists[depth][i];
			boardState.makeMove(moveLists[depth][i]);
			returnedMove = alphaBeta(boardState, depth + 2, alpha, beta);

			moveLists[depth][i].setScore(returnedMove.score);
			if (returnedMove.getGameOverDepth() != -1)
			{
				moveLists[depth][i].setGameOverDepth(returnedMove.getGameOverDepth() + 1);
			}

			boardState.unmakeMove(moveLists[depth][i]);
		}
	}
	else
	{
		sorter.assignOrderingScores();
	}

    for(int i=0; i<moveCount; i++)
    {
		sorter.sortNext();
		if (!moveLists[depth][i].isSafe(boardState))
		{
			continue;
		}

		legalMoves++;
		lastMoveMade[depth] = moveLists[depth][i];
        boardState.makeMove(moveLists[depth][i]);
		variations[depth][depth] = moveLists[depth][i];

        if(distanceToHorizon(depth) == 0)
        {
            if (quiescence_enabled && moveLists[depth][i].pieceCaptured != PieceType::Empty)
            {
				moveLists[depth][i].score = quiesce(boardState, alpha, beta, moveLists[depth][i], 7);
            }
            else
            {
                engine.evaluateMove(boardState, moveLists[depth], i);
            }
        }
        else
        {
            returnedMove = alphaBeta(boardState, depth + 1, alpha, beta);

			moveLists[depth][i].setScore(returnedMove.score);
            if(returnedMove.getGameOverDepth() != -1)
            {
				moveLists[depth][i].setGameOverDepth(returnedMove.getGameOverDepth() + 1);
            }
        }

		boardState.unmakeMove(moveLists[depth][i]);
        bestIndex = bestMove(moveLists[depth], bestIndex, i, boardState.facts.turn);
        updateAlphaBeta(moveLists[depth][bestIndex].score, boardState.facts.turn, alpha, beta);
		if (bestIndex == i)
		{
			for (int j = horizonDepth; j >= depth; j--)
			{
				variations[depth - 1][j] = variations[depth][j];
			}
		}

        if (causesAlphaBetaBreak(moveLists[depth][i].score, alpha, beta, boardState.facts.turn))
        {
            engine.updateTranspositionCutoffIfDeeper(boardState, distanceToHorizon(depth), moveLists[depth][i]);
			if (moveLists[depth][i].pieceCaptured == PieceType::Empty)
			{
				killerMoves[depth].add(moveLists[depth][i]);
			}

            return(moveLists[depth][i]);
        }

		if (depth == 1 && std::chrono::steady_clock::now() > dieTime)
		{
			return moveLists[depth][bestIndex];
		}
    }

	//Checkmate or StaleMate
	if (legalMoves == 0)
	{
		returnedMove = Move();
		if (inCheck)
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

    engine.updateTranspositionBestIfDeeper(boardState, distanceToHorizon(depth), moveLists[depth][bestIndex]);
    return moveLists[depth][bestIndex];
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
    Move moveList[300];
    boardState.generatePseudoMoveArray(moveList, moveCount, true);
	MoveSorter sorter = MoveSorter(moveList, moveCount, boardState, TranspositionCache(), MoveLookup(), lastCap, Move());
	
    int bestIndex = -1;
    for (int i = 0; i < moveCount; i++)
    {
		sorter.sortNext();
		PieceType pieceToCapture = moveList[i].pieceCaptured;
		if ((pieceToCapture == PieceType::Pawn && pawnValue < minDelta) ||
			(pieceToCapture == PieceType::Bishop && bishopValue < minDelta) ||
			(pieceToCapture == PieceType::Knight && knightValue < minDelta) ||
			(pieceToCapture == PieceType::Rook && rookValue < minDelta) ||
			(pieceToCapture == PieceType::Queen && queenValue < minDelta))
		{
			continue;
		}

		if (!moveList[i].isSafe(boardState))
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