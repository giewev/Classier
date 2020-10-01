#include "MaterialEvaluator.h"
#include "Board.h"
#include "Bitwise.h"
#include "Evaluation Constants.h"

MaterialEvaluator::MaterialEvaluator()
{
}

MaterialEvaluator::~MaterialEvaluator()
{
}

double MaterialEvaluator::evaluate(Board boardState)
{
    double score = 0;

    double knightBishopMod = knightBishopWeight * (bitwise::countBits(boardState.facts.pieces[PieceType::Pawn] & centerBoard) - 2);
    score += (knightValue + knightBishopMod) * boardState.pieceCount(PieceType::Knight, true);
    score -= (knightValue + knightBishopMod) * boardState.pieceCount(PieceType::Knight, false);
    score += (bishopValue - knightBishopMod) * boardState.pieceCount(PieceType::Bishop, true);
    score -= (bishopValue - knightBishopMod) * boardState.pieceCount(PieceType::Bishop, false);

    score += pawnValue * boardState.pieceCount(PieceType::Pawn, true);
    score -= pawnValue * boardState.pieceCount(PieceType::Pawn, false);
    score += rookValue * boardState.pieceCount(PieceType::Rook, true);
    score -= rookValue * boardState.pieceCount(PieceType::Rook, false);
    score += queenValue * boardState.pieceCount(PieceType::Queen, true);
    score -= queenValue * boardState.pieceCount(PieceType::Queen, false);

    return score;
}

double MaterialEvaluator::pieceValue(PieceType type)
{
    switch (type)
    {
    case PieceType::Empty:
        return 0;
    case PieceType::Pawn:
        return pawnValue;
    case PieceType::Knight:
        return knightValue;
    case PieceType::Bishop:
        return bishopValue;
    case PieceType::Rook:
        return rookValue;
    case PieceType::Queen:
        return queenValue;
    case PieceType::King:
        return 999;
    }

    throw "Unkown piece type";
}
