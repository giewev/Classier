#ifndef ALPHABETASEARCHER_H
#define ALPHABETASEARCHER_H
#include "Engine.h"
#include <chrono>

class Move;

class AlphaBetaSearcher
{
    public:
        AlphaBetaSearcher(Engine& linkedEngine, std::chrono::steady_clock::time_point);

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
		std::chrono::steady_clock::time_point dieTime;
		int topDepth;
};

#endif // ALPHABETASEARCHER_H
