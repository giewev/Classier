#ifndef GAMEOVEREVALUATOR_H
#define GAMEOVEREVALUATOR_H

#include <Evaluator.h>

class GameOverEvaluator : public Evaluator
{
public:
    GameOverEvaluator();
    virtual ~GameOverEvaluator();
    double evaluate(Board);
protected:
private:
};

#endif // GAMEOVEREVALUATOR_H
