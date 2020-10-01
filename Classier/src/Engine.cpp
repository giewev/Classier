#include "Danger.h"
#include "Engine.h"
#include "Evaluation Constants.h"
#include "Piece.h"
#include "Bitwise.h"
#include "FullEvaluator.h"
#include "Logger.h"
#include "MoveSorter.h"
#include "AlphaBetaSearcher.h"

#include <iostream>
#include <math.h>
#include <random>
#include <time.h>
#include <unordered_map>
#include <vector>
#include <algorithm>
#include <fstream>
#include <chrono>

using namespace std;

Engine::Engine()
{
    gameBoard = Board();
    evaluator = FullEvaluator();
}

Engine::Engine(Board loadBoard)
{
    gameBoard = loadBoard;
    evaluator = FullEvaluator();
}

Engine::~Engine()
{
}

Board Engine::getBoard()
{
    return gameBoard;
}

void Engine::setBoard(Board loadBoard)
{
    gameBoard = loadBoard;
}

Move Engine::searchToDepth(int depth)
{
	chrono::steady_clock::time_point end = chrono::steady_clock::now() + chrono::hours(99);
	return searchToDepth(depth, end);
}

Move Engine::searchToDepth(int depth, chrono::steady_clock::time_point cancelTime)
{
	AlphaBetaSearcher searcher = AlphaBetaSearcher(*this, cancelTime);
	Move bestMove = searcher.alphaBeta(gameBoard, depth);

	for (int i = depth - 1; i > 0; i--)
	{
		std::cout << (double)searcher.nodesAtDepths[i] / searcher.nodesAtDepths[i + 1] << " ";
	}
	std::cout << std::endl;

	if (bestMove.getMateDistance())
	{
		printf("info depth %d nodes %d score mate %d pv ", depth, searcher.nodesVisited, bestMove.getMateDistance());
		std::cout << bestMove.basicAlg() << std::endl;
	}
	else {
		int centipawnScore = (int)(bestMove.getScore() * 100);
		if (!gameBoard.facts.turn) centipawnScore *= -1;
		printf("info depth %d nodes %d score cp %d pv ", depth, searcher.nodesVisited, centipawnScore);
		std::cout << bestMove.basicAlg() << std::endl;
	}
	
	return bestMove;
}

Move Engine::searchForTime(int milliseconds)
{
	chrono::steady_clock::time_point begin = chrono::steady_clock::now();
	chrono::steady_clock::time_point end = begin + chrono::milliseconds(milliseconds);
	resetTransTable();
	Move lastBest;
    Move bestMove;
    int depth = 1;
    while (chrono::steady_clock::now() < end)
    {
		if (depth != 1) lastBest = bestMove;
        bestMove = searchToDepth(depth++, end);
    }

	//bestMove.setScore(lastBest.score);
    return bestMove;
}

void Engine::evaluateMove(const Board& evaluationBoard, Move* moveList, int index)
{
    double moveScore = evaluator.evaluate(evaluationBoard);
    if(moveScore != 1000)
    {
        moveList[index].setScore(moveScore);
    }
    else
    {
        moveList[index].setScore(0);
    }

    if(moveScore == 999 || moveScore == -999)
    {
        moveList[index].setGameOverDepth(1);
    }
}

double Engine::evaluatePosition(const Board& evaluationBoard)
{
    double rawScore = evaluator.evaluate(evaluationBoard);
    if(rawScore == 1000)
    {
        rawScore = 0;
    }

    return rawScore;
}

double Engine::lazyEvaluatePosition(const Board& evaluationBoard)
{
    return evaluator.lazyEvaluate(evaluationBoard);
}

std::string Engine::toAlg(int val)
{
    if(val < 0 || val > 7)
    {
        return "z";
    }

    std::string alpha[8] = {"a", "b", "c", "d", "e", "f", "g", "h"};
    return(alpha[val]);
}

void Engine::updateTranspositionBestIfDeeper(const Board& newBoard, int depth, Move newMove)
{
    this->clearTranspositionIfFull();
    if (this->transpositionTable[newBoard].bestDepth < depth)
    {
        this->transpositionTable[newBoard].bestDepth = depth;
        this->transpositionTable[newBoard].bestMove = newMove;
    }
}

void Engine::updateTranspositionCutoffIfDeeper(const Board& newBoard, int depth, Move newMove)
{
    this->clearTranspositionIfFull();
    if (this->transpositionTable[newBoard].cutoffDepth < depth)
    {
        this->transpositionTable[newBoard].cutoffDepth = depth;
        this->transpositionTable[newBoard].cutoffMove = newMove;
    }
}

void Engine::clearTranspositionIfFull()
{
	return;

	if (this->transpositionTable.size() > this->transTableMax)
	{
		resetTransTable();
		return;
	}

    int eraseDepth = 1;
    while (this->transpositionTable.size() > this->transTableMax)
    {
        for (auto entry = this->transpositionTable.begin(); entry != this->transpositionTable.end();)
        {
            if (entry->second.bestDepth <= eraseDepth && entry->second.cutoffDepth <= eraseDepth)
            {
                entry = this->transpositionTable.erase(entry);
            }
            else
            {
                entry++;
            }
        }

        eraseDepth++;
    }
}

TranspositionCache Engine::getTransposition(const Board& lookupBoard)
{
    return this->transpositionTable[lookupBoard];
}

void Engine::exportTransTable(std::string path)
{
    //std::ofstream outputStream(path, std::ios::binary);
    //cereal::BinaryOutputArchive archive(outputStream);
    //archive(this->transpositionTable);
}

void Engine::importTransTable(std::string path)
{
    //std::ifstream inputStream(path, std::ios::binary);
    //cereal::BinaryInputArchive archive(inputStream);
    //archive(this->transpositionTable);
}

void Engine::resetTransTable()
{
	this->transpositionTable.clear();
}