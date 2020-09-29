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
    bitBoard whitePawns = boardState.pieces[PieceType::Pawn] & boardState.pieces[0];
    bitBoard blackPawns = boardState.pieces[PieceType::Pawn] & ~boardState.pieces[0];
    score += doubledPawnValue * bitwise::countBits(whitePawns & (whitePawns << 8));
    score -= doubledPawnValue * bitwise::countBits(blackPawns & (blackPawns >> 8));

	// Reward pushing pawns, both for promotion and space reasons
	for (bitBoard i = 0; i < 8; i++)
	{
		score += pawnAdvancementValue * i * bitwise::countBits(whitePawns & bitwise::rank(i));
		score -= pawnAdvancementValue * (7 - i) * bitwise::countBits(blackPawns & bitwise::rank(i));
	}

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
			if ((i == 0 || blackPawnFiles[i - 1] == 0) && blackPawnFiles[i] == 0 && (i == 7 || blackPawnFiles[i + 1] == 0))
			{
				score += passedPawnValue;
			}
		}

		if (blackPawnFiles[i] > 0)
		{
			if ((i == 0 || whitePawnFiles[i - 1] == 0) && whitePawnFiles[i] == 0 && (i == 7 || whitePawnFiles[i + 1] == 0))
			{
				score -= passedPawnValue;
			}
		}
	}

    // Count how many pawns from each player are in the center
    bitBoard centerPawns = boardState.pieces[PieceType::Pawn] & centerBoard;
    score += centerPawnValue * bitwise::countBits(centerPawns & boardState.pieces[0]);
    score -= centerPawnValue * bitwise::countBits(centerPawns & (~boardState.pieces[0]));
    score += semiCenterPawnValue * bitwise::countBits(whiteSemiCenter & boardState.pieces[0] & boardState.pieces[PieceType::Pawn]);
    score += semiCenterPawnValue * bitwise::countBits(blackSemiCenter & (~boardState.pieces[0]) & boardState.pieces[PieceType::Pawn]);

    // Knights on the rim are grim
    bitBoard sideKnights = boardState.pieces[PieceType::Knight] & (aFile | hFile);
    score += rimKnightValue * bitwise::countBits(sideKnights & boardState.pieces[0]);
    score -= rimKnightValue * bitwise::countBits(sideKnights & (~boardState.pieces[0]));

    //Bishop and Knight development
    bitBoard knightsAndBishops = (boardState.pieces[PieceType::Bishop] | boardState.pieces[PieceType::Knight]);
    bitBoard whiteUndeveloped = knightsAndBishops & rank1 & boardState.pieces[0];
    bitBoard blackUndeveloped = knightsAndBishops & rank8 & (~boardState.pieces[0]);
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
