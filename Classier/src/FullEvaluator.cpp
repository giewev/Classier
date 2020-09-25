#include "FullEvaluator.h"
#include "Board.h"

FullEvaluator::FullEvaluator()
{
    gameOverEvaluator = GameOverEvaluator();
    materialEvaluator = MaterialEvaluator();
    positionalEvaluator = PositionalEvaluator();
}

FullEvaluator::~FullEvaluator()
{
}

double FullEvaluator::evaluate(Board boardState)
{
    double score = gameOverEvaluator.evaluate(boardState);
    if (score == 1000)
    {
        return 0;
    }
    else if (score != 0)
    {
        return score;
    }

    score = materialEvaluator.evaluate(boardState);
    score += positionalEvaluator.evaluate(boardState);

    return score;
}

double FullEvaluator::lazyEvaluate(Board boardState)
{
    double score = materialEvaluator.evaluate(boardState);
    score += positionalEvaluator.evaluate(boardState);

    return score;
}
