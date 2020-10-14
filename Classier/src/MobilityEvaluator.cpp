#include "MobilityEvaluator.h"
#include "Evaluation Constants.h"
#include "Board.h"
#include "Bitwise.h"
#include "Engine.h"
#include "Piece.h"

MobilityEvaluator::MobilityEvaluator()
{
}

MobilityEvaluator::~MobilityEvaluator()
{
}

double MobilityEvaluator::evaluate(Board boardState)
{
    double score = 0;

	bitBoard whiteMap = boardState.facts.pieces[0];
	bitBoard blackMap = ~boardState.facts.pieces[0] & boardState.facts.allPieces;

	score += mobilityValue * mobilityScore(boardState, whiteMap);
	score -= mobilityValue * mobilityScore(boardState, blackMap);
    return score;
}

int MobilityEvaluator::mobilityScore(const Board& board, bitBoard pieceMap)
{
	unsigned long scanIndex;
	int count = 0;
	for (char pType = 2; pType < 7; pType++)
	{
		bitBoard moveables = board.facts.pieces[pType] & pieceMap;
		while (_BitScanForward64(&scanIndex, moveables))
		{
			bitBoard moves;
			switch (pType)
			{
			case(PieceType::Bishop):
				moves = Piece::bishopMoveBoard(scanIndex, true, board, false);
			case(PieceType::Knight):
				moves = Piece::knightMoveBoard(scanIndex, true, board, false);
			case(PieceType::Queen):
				moves = Piece::queenMoveBoard(scanIndex, true, board, false);
			case(PieceType::Rook):
				moves = Piece::rookMoveBoard(scanIndex, true, board, false);
			};

			count += bitwise::countBits(moves);
			moveables &= moveables - 1;
		}
	}

	return count;
}