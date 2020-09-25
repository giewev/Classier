#ifndef POSITIONALEVALUATOR_H
#define POSITIONALEVALUATOR_H

#include <Evaluator.h>

class PositionalEvaluator : public Evaluator
{
public:
    PositionalEvaluator();
    virtual ~PositionalEvaluator();
    double evaluate(Board);
protected:
private:
    static double manhattenDistance(const int x1, const int y1, const double x2, const double y2);
};

#endif // POSITIONALEVALUATOR_H
