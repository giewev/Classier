#ifndef FULLEVALUATOR_H
#define FULLEVALUATOR_H

#include <Evaluator.h>
#include <MaterialEvaluator.h>
#include <PositionalEvaluator.h>
#include <MobilityEvaluator.h>

class FullEvaluator : public Evaluator
{
public:
    FullEvaluator();
    virtual ~FullEvaluator();
    double evaluate(Board);
    double lazyEvaluate(Board);
protected:
private:
    MaterialEvaluator materialEvaluator;
    PositionalEvaluator positionalEvaluator;
	MobilityEvaluator mobilityEvaluator;
};

#endif // FULLEVALUATOR_H
