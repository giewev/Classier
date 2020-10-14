#include "FullEvaluator.h"
#include "Board.h"

FullEvaluator::FullEvaluator()
{
    materialEvaluator = MaterialEvaluator();
    positionalEvaluator = PositionalEvaluator();
	mobilityEvaluator = MobilityEvaluator();
}

FullEvaluator::~FullEvaluator()
{
}

double FullEvaluator::evaluate(Board boardState)
{
	double score = 0;
    score = materialEvaluator.evaluate(boardState);
    score += positionalEvaluator.evaluate(boardState);
	score += mobilityEvaluator.evaluate(boardState);

    return score;
}

double FullEvaluator::lazyEvaluate(Board boardState)
{
    double score = materialEvaluator.evaluate(boardState);
    score += positionalEvaluator.evaluate(boardState);

    return score;
}
