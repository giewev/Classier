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

    // Count how many pawns are doubled up on a friendly pawn
    bitBoard whitePawns = boardState.facts.pieces[PieceType::Pawn] & boardState.facts.pieces[0];
    bitBoard blackPawns = boardState.facts.pieces[PieceType::Pawn] & ~boardState.facts.pieces[0];
    score += doubledPawnValue * bitwise::countBits(whitePawns & (whitePawns << 8));
    score -= doubledPawnValue * bitwise::countBits(blackPawns & (blackPawns >> 8));

	score += boardState.pawnPositionalValue;

	// Count the number of passed pawns for each player
	int whitePawnFiles[8];
	int blackPawnFiles[8];
	for (int i = 0; i < 8; i++)
	{
		whitePawnFiles[i] = bitwise::countBits(whitePawns & bitwise::file(i));
		blackPawnFiles[i] = bitwise::countBits(blackPawns & bitwise::file(i));
	}

	for (int i = 0; i < 8; i++)
	{
		if (whitePawnFiles[i] > 0)
		{
			// Check for white passed pawns
			if ((i == 0 || blackPawnFiles[i - 1] == 0) && blackPawnFiles[i] == 0 && (i == 7 || blackPawnFiles[i + 1] == 0))
			{
				score += passedPawnValue;
			}

			if ((i == 0 || whitePawnFiles[i - 1] == 0) && (i == 7 || whitePawnFiles[i + 1] == 0))
			{
				score += whitePawnFiles[i] * isolatedPawnValue;
			}
		}

		if (blackPawnFiles[i] > 0)
		{
			// Black passed pawns
			if ((i == 0 || whitePawnFiles[i - 1] == 0) && whitePawnFiles[i] == 0 && (i == 7 || whitePawnFiles[i + 1] == 0))
			{
				score -= passedPawnValue;
			}

			if ((i == 0 || blackPawnFiles[i - 1] == 0) && (i == 7 || blackPawnFiles[i + 1] == 0))
			{
				score -= blackPawnFiles[i] * isolatedPawnValue;
			}
		}
	}

    // Knights on the rim are grim
    bitBoard sideKnights = boardState.facts.pieces[PieceType::Knight] & (aFile | hFile);
    score += rimKnightValue * bitwise::countBits(sideKnights & boardState.facts.pieces[0]);
    score -= rimKnightValue * bitwise::countBits(sideKnights & (~boardState.facts.pieces[0]));

    //Bishop and Knight development
    bitBoard knightsAndBishops = (boardState.facts.pieces[PieceType::Bishop] | boardState.facts.pieces[PieceType::Knight]);
    bitBoard whiteUndeveloped = knightsAndBishops & rank1 & boardState.facts.pieces[0];
    bitBoard blackUndeveloped = knightsAndBishops & rank8 & (~boardState.facts.pieces[0]);
    score -= developmentValue * bitwise::countBits(whiteUndeveloped);
    score += developmentValue * bitwise::countBits(blackUndeveloped);

    //Kings far from center during the main game, but near the center when there are few pieces left
    double kingLocationMultiplier = 1;
    if (boardState.pieceCount() < 12)
    {
        kingLocationMultiplier *= -1;
    }

    score += kingCornerValue * kingLocationMultiplier * manhattenDistance(boardState.getKingX(true), boardState.getKingY(true), 3.5, 3.5);
    score -= kingCornerValue * kingLocationMultiplier * manhattenDistance(boardState.getKingX(false), boardState.getKingY(false), 3.5, 3.5);

    return score;
}

double PositionalEvaluator::manhattenDistance(const int x1, const int y1, const double x2, const double y2)
{
    return fabs(x1 - x2) + fabs(y1 - y2);
}
