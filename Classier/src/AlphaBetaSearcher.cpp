#include "AlphaBetaSearcher.h"

AlphaBetaSearcher::AlphaBetaSearcher(Engine& linkedEngine) : engine(linkedEngine)
{
	nullMode == false;
}

Move AlphaBetaSearcher::alphaBeta(const Board& boardState, int depth)
{
    return this->alphaBeta(boardState, depth, -1000, 1000);
}

Move AlphaBetaSearcher::alphaBeta(const Board& boardState, int depth, double alpha, double beta)
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

	if (!nullMode && depth > 2)
	{
		nullMode = true;
		Move nullMove = Move();
		Board nullBoard = boardState.newCopy();
		nullBoard.makeMove(nullMove);
		Move nullResponse = alphaBeta(nullBoard, 2, alpha, beta);
		nullMode = false;
		nullMove.score = nullResponse.score;
		if (causesAlphaBetaBreak(nullMove.score, alpha, beta, boardState.turn))
		{
			return nullMove;
		}
	}

    int moveCount = 0;
    Move moveList[220];
    boardState.generateMoveArray(moveList, moveCount);
    engine.sortMoveList(moveList, moveCount, boardState, transposition);

    Board newBoard;
    unsigned int bestIndex = 0;
    Move returnedMove;

    //Checkmate or StaleMate
    if(moveCount == 0)
    {
        returnedMove = Move();
        returnedMove.setScore(engine.evaluatePosition(boardState));
        returnedMove.setGameOverDepth(0);

        if (returnedMove.score == 1000)
        {
            returnedMove.setScore(0);
        }

        engine.updateTranspositionBestIfDeeper(boardState, depth, returnedMove);
        return returnedMove;
    }

    for(int i=0; i<moveCount; i++)
    {
        newBoard = boardState.newCopy();
        newBoard.makeMove(moveList[i]);

        if(depth == 1)
        {
            if (moveList[i].isCapture(boardState))
            {
                moveList[i].score = quiesce(newBoard, alpha, beta);
            }
            else
            {
                engine.evaluateMove(newBoard, moveList, i);
            }
        }
        else
        {
            returnedMove = alphaBeta(newBoard, depth - 1, alpha, beta);

            moveList[i].setScore(returnedMove.score);
            if(returnedMove.getGameOverDepth() != -1)
            {
                moveList[i].setGameOverDepth(returnedMove.getGameOverDepth() + 1);
            }
        }

        bestIndex = bestMove(moveList, bestIndex, i, boardState.turn);
        updateAlphaBeta(moveList[bestIndex].score, boardState.turn, alpha, beta);

        if (causesAlphaBetaBreak(moveList[i].score, alpha, beta, boardState.turn))
        {
            engine.updateTranspositionCutoffIfDeeper(boardState, depth, moveList[i]);
            return(moveList[i]);
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

double AlphaBetaSearcher::quiesce(const Board& boardState, double alpha, double beta)
{
    double staticScore = engine.lazyEvaluatePosition(boardState);
    if (causesAlphaBetaBreak(staticScore, alpha, beta, boardState.turn))
    {
        return staticScore;
    }

    updateAlphaBeta(staticScore, boardState.turn, alpha, beta);

    int moveCount = 0;
    Move moveList[220];
    boardState.generateCaptureMoves(moveList, moveCount);
    engine.sortMoveList(moveList, moveCount, boardState, TranspositionCache());
    int bestIndex = -1;
    for (int i = 0; i < moveCount; i++)
    {
        Board moveBoard = boardState.newCopy();
        moveBoard.makeMove(moveList[i]);
        double score = quiesce(moveBoard, alpha, beta);
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
