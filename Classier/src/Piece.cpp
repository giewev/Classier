#include "Piece.h"
#include "Move.h"
#include "Bitwise.h"
#include <iostream>
#include <string>
#include "Board.h"

Piece::Piece()
{
    xPos = -1;
    yPos = -1;
    color = false;
    type = PieceType::Empty;
    setMoved(false);
}
Piece::Piece(bool newColor)
{
    xPos = -1;
    yPos = -1;
    color = newColor;
    type = PieceType::Empty;
    setMoved(false);
}
Piece::Piece(int newX, int newY, bool newColor)
{
    xPos = newX;
    yPos = newY;
    color = newColor;
    type = PieceType::Empty;
    setMoved(false);
}
Piece::Piece(PieceType newType, int newX, int newY, bool newColor)
{
    xPos = newX;
    yPos = newY;
    color = newColor;
    type = newType;
    setMoved(false);
}

Piece::Piece(PieceType newType)
{
    xPos = -1;
    yPos = -1;
    color = true;
    type = newType;
    setMoved(false);
}

Piece::~Piece()
{
}

bool Piece::operator!=(Piece other)
{
    if(type != other.type)
    {
        return true;
    }
    if(xPos != other.xPos)
    {
        return true;
    }
    if(yPos != other.yPos)
    {
        return true;
    }
    if(color != other.color)
    {
        return true;
    }
    return false;
}

const bool Piece::isNull()
{
    return (type == PieceType::Empty);
}


bool Piece::hasMoved()
{
    return(moved);
}
void Piece::setMoved(bool x)
{
    moved = x;
    return;
}

void Piece::appendMoveArray(Move* moveList, int& moveCounter, PieceType movingType, int index, const Board& gameBoard, bool captureOnly)
{
	switch (movingType)
	{
	case (PieceType::Pawn):
		pawnMoveArray(moveList, moveCounter, index, gameBoard, captureOnly);
		break;
	case (PieceType::King):
		kingMoveArray(moveList, moveCounter, index, gameBoard, captureOnly);
		break;
	case (PieceType::Queen):
		queenMoveArray(moveList, moveCounter, index, gameBoard, captureOnly);
		break;
	case (PieceType::Bishop):
		bishopMoveArray(moveList, moveCounter, index, gameBoard, captureOnly);
		break;
	case (PieceType::Knight):
		knightMoveArray(moveList, moveCounter, index, gameBoard, captureOnly);
		break;
	case (PieceType::Rook):
		rookMoveArray(moveList, moveCounter, index, gameBoard, captureOnly);
		break;
	}
}

bitBoard Piece::kingRegularMoveBoard(int index, bool color, const Board& gameBoard, bool captureOnly)
{
	bitBoard legalMoves;
	if (color) legalMoves = ~gameBoard.facts.pieces[0] & gameBoard.facts.allPieces;
	else legalMoves = gameBoard.facts.pieces[0] & gameBoard.facts.allPieces;

	if (!captureOnly)
	{
		legalMoves |= ~gameBoard.facts.allPieces;
	}

	legalMoves &= kingMoves[index];
	return legalMoves;
}

void Piece::kingMoveArray(Move* moveList, int& moveCounter, int index, const Board& gameBoard, bool captureOnly)
{
	bool ownColor = gameBoard.getSquareColor(index);
	bitBoard legalMoves = kingRegularMoveBoard(index, ownColor, gameBoard, captureOnly);

	unsigned long legalIndex;
	while (_BitScanForward64(&legalIndex, legalMoves))
	{
		moveList[moveCounter++] = Move(index, legalIndex, PieceType::King, PieceType::Empty, gameBoard);
		legalMoves &= legalMoves - 1;
	}
	
	if (!captureOnly)
	{
		// Kingside castling
		if (gameBoard.getCastlingRights(ownColor, 0))
		{
			bitBoard castlingObstacles = kingCastlingMoves[ownColor][0] & gameBoard.facts.allPieces;
			if (!castlingObstacles)
			{
				moveList[moveCounter++] = Move(index, index - 2, PieceType::King, PieceType::Empty, gameBoard);
			}
		}

		// Queenside castling
		if (gameBoard.getCastlingRights(ownColor, 1))
		{
			bitBoard castlingObstacles = kingCastlingMoves[ownColor][1] & gameBoard.facts.allPieces;
			if (!castlingObstacles)
			{
				moveList[moveCounter++] = Move(index, index + 2, PieceType::King, PieceType::Empty, gameBoard);
			}
		}
	}
}

bitBoard Piece::queenMoveBoard(int index, bool color, const Board& gameBoard, bool captureOnly)
{
	bitBoard moves = queenMoves[index];
	bitBoard blockers = queenBlockerMask[index] & gameBoard.facts.allPieces;

	unsigned long scanIndex;
	while (_BitScanForward64(&scanIndex, blockers))
	{
		moves &= notBehind[index][scanIndex];
		blockers &= blockers - 1;
	}

	if (captureOnly)
	{
		if (color) moves &= ~gameBoard.facts.pieces[0] & gameBoard.facts.allPieces;
		else moves &= gameBoard.facts.pieces[0] & gameBoard.facts.allPieces;
	}
	else
	{
		if (color) moves &= ~gameBoard.facts.allPieces | ~gameBoard.facts.pieces[0];
		else moves &= ~gameBoard.facts.allPieces | gameBoard.facts.pieces[0];
	}

	return moves;
}

void Piece::queenMoveArray(Move* moveList, int& moveCounter, int index, const Board& gameBoard, bool captureOnly)
{
	bool ownColor = gameBoard.facts.turn;
	bitBoard moves = queenMoveBoard(index, ownColor, gameBoard, captureOnly);

	unsigned long scanIndex;
	while (_BitScanForward64(&scanIndex, moves))
	{
		moveList[moveCounter++] = Move(index, scanIndex, PieceType::Queen, PieceType::Empty, gameBoard);
		moves &= moves - 1;
	}
}

bitBoard Piece::bishopMoveBoard(int index, bool color, const Board& gameBoard, bool captureOnly)
{
	bitBoard moves = bishopMoves[index];
	bitBoard blockers = bishopBlockerMask[index] & gameBoard.facts.allPieces;

	unsigned long scanIndex;
	while (_BitScanForward64(&scanIndex, blockers))
	{
		moves &= notBehind[index][scanIndex];
		blockers &= blockers - 1;
	}

	if (captureOnly)
	{
		if (color) moves &= ~gameBoard.facts.pieces[0] & gameBoard.facts.allPieces;
		else moves &= gameBoard.facts.pieces[0] & gameBoard.facts.allPieces;
	}
	else
	{
		if (color) moves &= ~gameBoard.facts.allPieces | ~gameBoard.facts.pieces[0];
		else moves &= ~gameBoard.facts.allPieces | gameBoard.facts.pieces[0];
	}

	return moves;
}

void Piece::bishopMoveArray(Move* moveList, int& moveCounter, int index, const Board& gameBoard, bool captureOnly)
{
	bool ownColor = gameBoard.facts.turn;
	bitBoard moves = bishopMoveBoard(index, ownColor, gameBoard, captureOnly);

	unsigned long scanIndex;
	while (_BitScanForward64(&scanIndex, moves))
	{
		moveList[moveCounter++] = Move(index, scanIndex, PieceType::Bishop, PieceType::Empty, gameBoard);
		moves &= moves - 1;
	}
}

bitBoard Piece::knightMoveBoard(int index, bool color, const Board& gameBoard, bool captureOnly)
{
	bitBoard obstacles;
	if (color) obstacles = gameBoard.facts.allPieces & gameBoard.facts.pieces[0];
	else obstacles = gameBoard.facts.allPieces & ~gameBoard.facts.pieces[0];

	bitBoard legalDestinations = ~obstacles & knightMoves[index];
	if (captureOnly) legalDestinations &= gameBoard.facts.allPieces;
	return legalDestinations;
}

void Piece::knightMoveArray(Move* moveList, int& moveCounter, int index, const Board& gameBoard, bool captureOnly)
{
	bool ownColor = gameBoard.getSquareColor(index);
	bitBoard legalDestinations = knightMoveBoard(index, ownColor, gameBoard, captureOnly);
	
	unsigned long legalIndex;
	while (_BitScanForward64(&legalIndex, legalDestinations))
	{
		moveList[moveCounter++] = Move(index, legalIndex, PieceType::Knight, PieceType::Empty, gameBoard);
		legalDestinations &= legalDestinations - 1;
	}
}

bitBoard Piece::rookMoveBoard(int index, bool color, const Board& gameBoard, bool captureOnly)
{
	bitBoard moves = rookMoves[index];
	bitBoard blockers = rookBlockerMask[index] & gameBoard.facts.allPieces;

	unsigned long scanIndex;
	while (_BitScanForward64(&scanIndex, blockers))
	{
		moves &= notBehind[index][scanIndex];
		blockers &= blockers - 1;
	}

	if (captureOnly)
	{
		if (color) moves &= ~gameBoard.facts.pieces[0] & gameBoard.facts.allPieces;
		else moves &= gameBoard.facts.pieces[0] & gameBoard.facts.allPieces;
	}
	else
	{
		if (color) moves &= ~gameBoard.facts.allPieces | ~gameBoard.facts.pieces[0];
		else moves &= ~gameBoard.facts.allPieces | gameBoard.facts.pieces[0];
	}

	return moves;
}

void Piece::rookMoveArray(Move* moveList, int& moveCounter, int index, const Board& gameBoard, bool captureOnly)
{
	bool ownColor = gameBoard.facts.turn;
	bitBoard moves = rookMoveBoard(index, ownColor, gameBoard, captureOnly);

	unsigned long scanIndex;
	while (_BitScanForward64(&scanIndex, moves))
	{
		moveList[moveCounter++] = Move(index, scanIndex, PieceType::Rook, PieceType::Empty, gameBoard);
		moves &= moves - 1;
	}
}

void Piece::pawnMoveArray(Move* moveList, int& moveCounter, int index, const Board& gameBoard, bool captureOnly)
{
	bool ownColor = gameBoard.facts.turn;
	unsigned long legalIndex;

	if (!captureOnly)
	{
		bitBoard doubleMoves = pawnDoubleMoves[ownColor][index] & ~gameBoard.facts.allPieces;
		if (bitwise::countBits(doubleMoves) == 2)
		{
			while (_BitScanForward64(&legalIndex, doubleMoves))
			{
				moveList[moveCounter++] = Move(index, legalIndex, PieceType::Pawn, PieceType::Empty, gameBoard);
				doubleMoves &= doubleMoves - 1;
			}
		}
		else
		{
			bitBoard singleMove = pawnSingleMoves[ownColor][index] & ~gameBoard.facts.allPieces;
			if (_BitScanForward64(&legalIndex, singleMove))
			{
				if (promotionSquares & singleMove)
				{
					moveList[moveCounter++] = Move(index, legalIndex, PieceType::Pawn, PieceType::Queen, gameBoard);
					moveList[moveCounter++] = Move(index, legalIndex, PieceType::Pawn, PieceType::Knight, gameBoard);
					moveList[moveCounter++] = Move(index, legalIndex, PieceType::Pawn, PieceType::Rook, gameBoard);
					moveList[moveCounter++] = Move(index, legalIndex, PieceType::Pawn, PieceType::Bishop, gameBoard);
				}
				else
				{
					moveList[moveCounter++] = Move(index, legalIndex, PieceType::Pawn, PieceType::Empty, gameBoard);
				}
			}
		}
	}
	
	bitBoard enemies = gameBoard.facts.pieces[0];
	if (ownColor) enemies = ~enemies;
	bitBoard epTargets = 0ll;
	if (gameBoard.getEPData() != -1)
	{
		int ep = gameBoard.getEPData();
		if (ownColor)
		{
			epTargets |= bitwise::indexToBoard(ep + 8);
		}
		else
		{
			epTargets |= bitwise::indexToBoard(ep - 8);
		}
	}

	bitBoard captureMoves = pawnCaptureMoves[ownColor][index] & ((gameBoard.facts.allPieces & enemies) | epTargets);
	while (_BitScanForward64(&legalIndex, captureMoves))
	{
		if (promotionSquares & captureMoves)
		{
			moveList[moveCounter++] = Move(index, legalIndex, PieceType::Pawn, PieceType::Queen, gameBoard);
			moveList[moveCounter++] = Move(index, legalIndex, PieceType::Pawn, PieceType::Knight, gameBoard);
			moveList[moveCounter++] = Move(index, legalIndex, PieceType::Pawn, PieceType::Rook, gameBoard);
			moveList[moveCounter++] = Move(index, legalIndex, PieceType::Pawn, PieceType::Bishop, gameBoard);
		}
		else
		{
			moveList[moveCounter++] = Move(index, legalIndex, PieceType::Pawn, PieceType::Empty, gameBoard);
		}
		captureMoves &= captureMoves - 1;
	}
}
