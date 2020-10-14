#include "PositionalEvaluator.h"
#include "Evaluation Constants.h"
#include "Board.h"
#include "Bitwise.h"
#include "Engine.h"

PositionalEvaluator::PositionalEvaluator()
{
}

PositionalEvaluator::~PositionalEvaluator()
{
}

double PositionalEvaluator::evaluate(Board boardState)
{
    double score = 0;

	score += boardState.pawnPositionalValue;
	score += boardState.knightPositionalValue;
	score += boardState.bishopPositionalValue;

    // Count how many pawns are doubled up on a friendly pawn
    bitBoard whitePawns = boardState.facts.pieces[PieceType::Pawn] & boardState.facts.pieces[0];
    bitBoard blackPawns = boardState.facts.pieces[PieceType::Pawn] & ~boardState.facts.pieces[0];
    score += doubledPawnValue * bitwise::countBits(whitePawns & (whitePawns << 8));
    score -= doubledPawnValue * bitwise::countBits(blackPawns & (blackPawns >> 8));

	bitBoard whitePawnScan = whitePawns;
	bitBoard blackPawnScan = blackPawns;
	unsigned long scanIndex;
	while (_BitScanForward64(&scanIndex, whitePawnScan))
	{
		if (!(passedPawnBlockers[true][scanIndex] & blackPawns))
		{
			score += passedPawnValue;
		}

		if (!(adjacentFilesByIndex[scanIndex] & whitePawns))
		{
			score += isolatedPawnValue;
		}

		whitePawnScan &= whitePawnScan - 1;
	}

	while (_BitScanForward64(&scanIndex, blackPawnScan))
	{
		if (!(passedPawnBlockers[false][scanIndex] & whitePawns))
		{
			score -= passedPawnValue;
		}

		if (!(adjacentFilesByIndex[scanIndex] & blackPawns))
		{
			score -= isolatedPawnValue;
		}

		blackPawnScan &= blackPawnScan - 1;
	}

	if (hasBishopPair(boardState, true)) score += bishopPairValue;
	if (hasBishopPair(boardState, false)) score -= bishopPairValue;

    return score;
}

double PositionalEvaluator::manhattenDistance(const int x1, const int y1, const double x2, const double y2)
{
    return fabs(x1 - x2) + fabs(y1 - y2);
}

bool PositionalEvaluator::hasBishopPair(const Board& board, bool color)
{
	if (color)
	{
		bitBoard whiteBishops = board.facts.pieces[0] & board.facts.pieces[PieceType::Bishop];
		return (whiteSquares & whiteBishops) && (blackSquares & whiteBishops);
	}
	else
	{
		bitBoard blackBishops = ~board.facts.pieces[0] & board.facts.pieces[PieceType::Bishop];
		return (whiteSquares & blackBishops) && (blackSquares & blackBishops);
	}
}