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
	topDepth = 0;
}

Move AlphaBetaSearcher::alphaBeta(Board& boardState, int depth)
{
	topDepth = depth;
	killerMoves.clear();
	killerMoves.resize(depth + 1);
    return this->alphaBeta(boardState, depth, -1000, 1000);
}

Move AlphaBetaSearcher::alphaBeta(Board& boardState, int depth, double alpha, double beta)
{
    TranspositionCache transposition = engine.getTransposition(boardState);
    if (transposition.bestDepth >= depth)
    {
        return transposition.bestMove;
    }
    else if (transposition.cutoffDepth >= depth && // Cutoff info is good enough
        causesAlphaBetaBreak(transposition.cutoffMove.score, alpha, beta, boardState.turn))
    {
        return transposition.cutoffMove;
    }

	Danger safetyinfo = Danger(boardState);

	// Null move heuristic
	// If we are searching a significant depth, we check to see if not making move at all would already cause a cutoff
	if (null_move_enabled && !nullMode && depth > 3)
	{
		if (!safetyinfo.getCheck())
		{
			nullMode = true;
			Move nullMove = Move(boardState);
			boardState.makeMove(nullMove);
			Move nullResponse = alphaBeta(boardState, depth - 2, alpha, beta);
			boardState.unmakeMove(nullMove);
			nullMode = false;
			nullMove.score = nullResponse.score;
			if (causesAlphaBetaBreak(nullMove.score, alpha, beta, boardState.turn))
			{
				return nullMove;
			}
		}
	}

    int moveCount = 0;
    Move moveList[220];
    boardState.generateMoveArray(moveList, moveCount);
	MoveSorter sorter = MoveSorter(moveList, moveCount, boardState, transposition, killerMoves[depth]);
	sorter.sortMoves();

    unsigned int bestIndex = 0;
    Move returnedMove;

    //Checkmate or StaleMate
    if(moveCount == 0)
    {
        returnedMove = Move();
		double gameOverScore = GameOverEvaluator().evaluate(boardState);
		returnedMove.setScore(gameOverScore);

        if (returnedMove.score == 1000)
        {
            returnedMove.setScore(0);
        }

		returnedMove.setGameOverDepth(0);

        engine.updateTranspositionBestIfDeeper(boardState, depth, returnedMove);
        return returnedMove;
    }

    for(int i=0; i<moveCount; i++)
    {
        boardState.makeMove(moveList[i]);

        if(depth == 1)
        {
            if (quiescence_enabled && moveList[i].pieceCaptured != PieceType::Empty)
            {
                moveList[i].score = quiesce(boardState, alpha, beta);
            }
            else
            {
                engine.evaluateMove(boardState, moveList, i);
            }
        }
        else
        {
            returnedMove = alphaBeta(boardState, depth - 1, alpha, beta);

            moveList[i].setScore(returnedMove.score);
            if(returnedMove.getGameOverDepth() != -1)
            {
                moveList[i].setGameOverDepth(returnedMove.getGameOverDepth() + 1);
            }
        }

		boardState.unmakeMove(moveList[i]);
        bestIndex = bestMove(moveList, bestIndex, i, boardState.turn);
        updateAlphaBeta(moveList[bestIndex].score, boardState.turn, alpha, beta);

        if (causesAlphaBetaBreak(moveList[i].score, alpha, beta, boardState.turn))
        {
            engine.updateTranspositionCutoffIfDeeper(boardState, depth, moveList[i]);
			if (moveList[i].pieceCaptured == PieceType::Empty)
			{
				killerMoves[depth].add(moveList[i]);
			}

            return(moveList[i]);
        }

		if ((depth >= topDepth && depth > 4) && std::chrono::steady_clock::now() > dieTime)
		{
			return moveList[bestIndex];
		}
    }

    engine.updateTranspositionBestIfDeeper(boardState, depth, moveList[bestIndex]);
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

double AlphaBetaSearcher::quiesce(Board& boardState, double alpha, double beta)
{
    double staticScore = engine.lazyEvaluatePosition(boardState);
    if (causesAlphaBetaBreak(staticScore, alpha, beta, boardState.turn))
    {
        return staticScore;
    }

    updateAlphaBeta(staticScore, boardState.turn, alpha, beta);
	double minDelta = deltaToAlphaBeta(staticScore, boardState.turn, alpha, beta) - 2;

    int moveCount = 0;
    Move moveList[220];
    boardState.generateCaptureMoves(moveList, moveCount); 
	MoveSorter sorter = MoveSorter(moveList, moveCount, boardState, TranspositionCache(), MoveLookup());
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
        double score = quiesce(boardState, alpha, beta);
		boardState.unmakeMove(moveList[i]);

        if (causesAlphaBetaBreak(score, alpha, beta, boardState.turn))
        {
            return score;
        }

        updateAlphaBeta(score, boardState.turn, alpha, beta);
        bestIndex = bestMove(moveList, bestIndex, i, boardState.turn);
    }

    if (bestIndex == -1)
    {
        return staticScore;
    }
    else if ((boardState.turn && staticScore > moveList[bestIndex].score) ||
        (!boardState.turn && staticScore < moveList[bestIndex].score))
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
