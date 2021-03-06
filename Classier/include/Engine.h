#ifndef ENGINE_H
#define ENGINE_H

#include <inttypes.h>
#include <time.h>
#include <iostream>
#include <unordered_map>
#include <chrono>
#include "FullEvaluator.h"
#include "TranspositionCache.h"
#include "Board.h"
#include "BoardInfo.h"
struct Move;
class HashData;
class HashTable;

#define killTableSize 3
#define bitBoard uint64_t

class Engine
{
public:
    Engine();
    Engine(Board);
    ~Engine();

    Board getBoard();
    void setBoard(Board);

    Move searchToDepth(int depth);
	Move searchToDepth(int depth, std::chrono::steady_clock::time_point);
    Move searchForTime(int milliseconds);

    void updateTranspositionBestIfDeeper(const Board& searchBoard, int depth, Move newMove);
    void updateTranspositionCutoffIfDeeper(const Board& searchBoard, int depth, Move newMove);
    TranspositionCache getTransposition(const Board& lookupBoard);
    void evaluateMove(const Board& evaluationBoard, Move* moveList, int index);
    double evaluatePosition(const Board& evaluationBoard);
    double lazyEvaluatePosition(const Board& evaluationBoard);
    void exportTransTable(std::string path);
    void importTransTable(std::string path);
	void resetTransTable();

    static std::string toAlg(int);
private:
    Board gameBoard;
    FullEvaluator evaluator;
    std::unordered_map<BoardInfo, TranspositionCache> transpositionTable;
    unsigned long transTableMax = 1024 * 1024 * 64;

    void clearTranspositionIfFull();

	unsigned long transChecks = 0;
	unsigned long transHits = 0;
};

#endif
