#ifndef EVALUATOR_H
#define EVALUATOR_H

class Board;

class Evaluator
{
public:
    Evaluator();
    virtual ~Evaluator();
    virtual double evaluate(Board) = 0;
protected:
private:
};

#endif // EVALUATOR_H
