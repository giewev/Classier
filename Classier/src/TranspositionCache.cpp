#include "TranspositionCache.h"

TranspositionCache::TranspositionCache()
{
    this->bestDepth = -1;
    this->bestMove = Move();
    this->cutoffDepth = -1;
    this->cutoffMove = Move();
}

TranspositionCache::TranspositionCache(int bestDepth, Move newMove, int cutoffDepth, Move cutoffMove)
{
    this->bestDepth = bestDepth;
    this->bestMove = newMove;
    this->cutoffDepth = cutoffDepth;
    this->cutoffMove = cutoffMove;
}
