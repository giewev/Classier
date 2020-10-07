#include "TranspositionCache.h"

TranspositionCache::TranspositionCache()
{
    this->bestHeight = -1;
    this->bestMove = Move();
    this->cutoffHeight = -1;
    this->cutoffMove = Move();
}

TranspositionCache::TranspositionCache(int bestHeight, Move newMove, int cutoffHeight, Move cutoffMove)
{
    this->bestHeight = bestHeight;
    this->bestMove = newMove;
    this->cutoffHeight = cutoffHeight;
    this->cutoffMove = cutoffMove;
}
