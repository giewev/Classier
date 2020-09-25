#ifndef FULLEVALUATOR_H
#define FULLEVALUATOR_H

#include <Evaluator.h>
#include <GameOverEvaluator.h>
#include <MaterialEvaluator.h>
#include <PositionalEvaluator.h>

class FullEvaluator : public Evaluator
{
public:
    FullEvaluator();
    virtual ~FullEvaluator();
    double evaluate(Board);
    double lazyEvaluate(Board);
protected:
private:
    GameOverEvaluator gameOverEvaluator;
    MaterialEvaluator materialEvaluator;
    PositionalEvaluator positionalEvaluator;
};

#endif // FULLEVALUATOR_H
