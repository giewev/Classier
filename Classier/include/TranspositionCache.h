#ifndef TRANSPOSITIONCACHE_H
#define TRANSPOSITIONCACHE_H
#include "Move.h"

struct TranspositionCache
{
public:
    int bestHeight;
    Move bestMove;

    int cutoffHeight;
    Move cutoffMove;

    TranspositionCache();
    TranspositionCache(int bestHeight, Move newMove, int cutoffHeight, Move cutoffMove);

protected:
private:
};

#endif // TRANSPOSITIONCACHE_H
