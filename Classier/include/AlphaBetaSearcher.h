#ifndef ALPHABETASEARCHER_H
#define ALPHABETASEARCHER_H
#include "Engine.h"
#include <chrono>
#include <set>
#include "MoveLookup.h"
#include "Move.h";
#include "AlgorithmToggles.h"

class AlphaBetaSearcher
{
    public:
        AlphaBetaSearcher(Engine& linkedEngine, std::chrono::steady_clock::time_point);

        Move alphaBeta(Board& boardState, int depth);
        Move alphaBeta(Board& boardState, int depth, double alpha, double beta);

		long nodesVisited;
		std::vector<long> nodesAtDepths;
		std::vector<Move> lastMoveMade;
		Move variations[maxDepth][maxDepth];
		Move moveLists[maxDepth][300];
		Move qMoveLists[20][200];
    protected:
    private:
        Engine& engine;

        static int chooseBetweenEqualMoves(Move* moveList, int currentIndex, int newIndex);
        static int bestMove(Move* moveList, int bestIndex, int currentIndex, bool turn);
        static bool causesAlphaBetaBreak(double score, double alpha, double beta, bool turn);
        static void updateAlphaBeta(double newScore, bool turn, double& alpha, double& beta);
		static double deltaToAlphaBeta(const double currentScore, const bool turn, const double alpha, const double beta);
        double quiesce(Board& boardState, double alpha, double beta, Move lastCap, int maxDepth);
		int distanceToHorizon(int depth);
		bool nullMode;
		std::chrono::steady_clock::time_point dieTime;
		int horizonDepth;
		std::vector<MoveLookup> killerMoves;
};

#endif // ALPHABETASEARCHER_H
