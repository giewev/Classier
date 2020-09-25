#ifndef ALPHABETASEARCHER_H
#define ALPHABETASEARCHER_H
#include "Engine.h"

class Move;

class AlphaBetaSearcher
{
    public:
        AlphaBetaSearcher(Engine& linkedEngine);

        Move alphaBeta(const Board& boardState, int depth);
        Move alphaBeta(const Board& boardState, int depth, double alpha, double beta);
    protected:
    private:
        Engine& engine;

        static int chooseBetweenEqualMoves(Move* moveList, int currentIndex, int newIndex);
        static int bestMove(Move* moveList, int bestIndex, int currentIndex, bool turn);
        static bool causesAlphaBetaBreak(double score, double alpha, double beta, bool turn);
        static void updateAlphaBeta(double newScore, bool turn, double& alpha, double& beta);
        double quiesce(const Board& boardState, double alpha, double beta);
		bool nullMode;
};

#endif // ALPHABETASEARCHER_H
