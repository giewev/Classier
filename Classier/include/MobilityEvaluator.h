#pragma once
#include <Evaluator.h>
#include <Bitwise.h>

class MobilityEvaluator : public Evaluator
{
public:
	MobilityEvaluator();
    virtual ~MobilityEvaluator();
    double evaluate(Board);
protected:
private:
	static int mobilityScore(const Board& board, bitBoard pieceMap);
};

