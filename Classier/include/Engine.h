#ifndef ENGINE_H
#define ENGINE_H

#include <inttypes.h>
#include <time.h>
#include <iostream>
#include <unordered_map>
#include "FullEvaluator.h"
#include "TranspositionCache.h"
#include "Board.h"
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

    static void sortMoveList(Move*, int, const Board&, const TranspositionCache&);
    static std::string toAlg(int);
private:
    Board gameBoard;
    FullEvaluator evaluator;
    std::unordered_map<Board, TranspositionCache> transpositionTable;
    unsigned long transTableMax = 1024 * 1024 * 64;

    void clearTranspositionIfFull();
};

#endif
