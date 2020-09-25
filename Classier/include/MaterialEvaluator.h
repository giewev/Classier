#ifndef MATERIALEVALUATOR_H
#define MATERIALEVALUATOR_H

#include <Evaluator.h>
#include "Bitwise.h"

class MaterialEvaluator : public Evaluator
{
public:
    MaterialEvaluator();
    virtual ~MaterialEvaluator();
    double evaluate(Board);

    static double pieceValue(PieceType type);
protected:
private:
};

#endif // MATERIALEVALUATOR_H
