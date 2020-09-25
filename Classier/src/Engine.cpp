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
    //time_t timer = time(NULL);
    AlphaBetaSearcher searcher = AlphaBetaSearcher(*this);
    Move bestMove = searcher.alphaBeta(gameBoard, depth);
	
	int centipawnScore = (int)(bestMove.getScore() * 100);
	if (!gameBoard.turn) centipawnScore *= -1;
	printf("info depth %d score cp %d pv ", depth, centipawnScore);
	std::cout << bestMove.basicAlg() << std::endl;
    //Logger::mainLog()->info("Alpha-beta search on position [{0}] to a depth of {1} chose: {2} with score: {3} after {4} seconds",
    //                        gameBoard.outputFEN(), depth, bestMove.basicAlg(), bestMove.score, difftime(time(NULL), timer));
    
	return bestMove;
}

Move Engine::searchForTime(int milliseconds)
{
	chrono::steady_clock::time_point begin = chrono::steady_clock::now();
	resetTransTable();
    Move bestMove;
    int depth = 1;
    while (chrono::duration_cast<chrono::milliseconds>(chrono::steady_clock::now() - begin).count() < milliseconds)
    {
        bestMove = searchToDepth(depth++);
        //Logger::mainLog()->flush();
    }

    //Logger::mainLog()->info("Iterative search on position [{0}] to a depth of {1} chose: {2} with score: {3} after {4} seconds",
    //                        gameBoard.outputFEN(), depth - 1, bestMove.basicAlg(), bestMove.score, difftime(time(NULL), timer));

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

void Engine::sortMoveList(Move* rawList, int moveCount, const Board& sortBoard, const TranspositionCache& transposition)
{
    MoveSorter sorter = MoveSorter(rawList, moveCount, sortBoard, transposition);
    sorter.sortMoves();
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