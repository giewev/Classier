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

void Piece::appendMoveArray(Move* moveList, int& moveCounter, PieceType movingType, int x, int y, const Board& gameBoard, bool captureOnly)
{
	switch (movingType)
	{
	case (PieceType::Pawn):
		pawnMoveArray(moveList, moveCounter, x, y, gameBoard, captureOnly);
		break;
	case (PieceType::King):
		kingMoveArray(moveList, moveCounter, x, y, gameBoard, captureOnly);
		break;
	case (PieceType::Queen):
		queenMoveArray(moveList, moveCounter, x, y, gameBoard, captureOnly);
		break;
	case (PieceType::Bishop):
		bishopMoveArray(moveList, moveCounter, x, y, gameBoard, captureOnly);
		break;
	case (PieceType::Knight):
		knightMoveArray(moveList, moveCounter, x, y, gameBoard, captureOnly);
		break;
	case (PieceType::Rook):
		rookMoveArray(moveList, moveCounter, x, y, gameBoard, captureOnly);
		break;
	}
}

bitBoard Piece::kingRegularMoveBoard(int x, int y, bool color, const Board& gameBoard, bool captureOnly)
{
	bitBoard legalMoves;
	if (color) legalMoves = ~gameBoard.facts.pieces[0] & gameBoard.facts.allPieces;
	else legalMoves = gameBoard.facts.pieces[0] & gameBoard.facts.allPieces;

	if (!captureOnly)
	{
		legalMoves |= ~gameBoard.facts.allPieces;
	}

	legalMoves &= kingMoves[x][y];
	return legalMoves;
}

void Piece::kingMoveArray(Move* moveList, int& moveCounter, int xPos, int yPos, const Board& gameBoard, bool captureOnly)
{
	bool ownColor = gameBoard.getSquareColor(xPos, yPos);
	bitBoard legalMoves = kingRegularMoveBoard(xPos, yPos, ownColor, gameBoard, captureOnly);

	unsigned long legalIndex;
	while (_BitScanForward64(&legalIndex, legalMoves))
	{
		int x = bitwise::bitBoardX(legalIndex);
		int y = bitwise::bitBoardY(legalIndex);
		moveList[moveCounter++] = Move(xPos, yPos, x, y, PieceType::Empty, gameBoard);
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
				moveList[moveCounter++] = Move(xPos, yPos, xPos - 2, yPos, PieceType::Empty, gameBoard);
			}
		}

		// Queenside castling
		if (gameBoard.getCastlingRights(ownColor, 1))
		{
			bitBoard castlingObstacles = kingCastlingMoves[ownColor][1] & gameBoard.facts.allPieces;
			if (!castlingObstacles)
			{
				moveList[moveCounter++] = Move(xPos, yPos, xPos + 2, yPos, PieceType::Empty, gameBoard);
			}
		}
	}
}

bitBoard Piece::queenMoveBoard(int x, int y, bool color, const Board& gameBoard, bool captureOnly)
{
	bitBoard moves = queenMoves[x][y];
	bitBoard blockers = queenBlockerMask[x][y] & gameBoard.facts.allPieces;
	int queenIndex = bitwise::coordToIndex(x, y);

	unsigned long scanIndex;
	while (_BitScanForward64(&scanIndex, blockers))
	{
		moves &= notBehind[queenIndex][scanIndex];
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

void Piece::queenMoveArray(Move* moveList, int& moveCounter, int xPos, int yPos, const Board& gameBoard, bool captureOnly)
{
	bool ownColor = gameBoard.facts.turn;
	bitBoard moves = queenMoveBoard(xPos, yPos, ownColor, gameBoard, captureOnly);

	unsigned long scanIndex;
	while (_BitScanForward64(&scanIndex, moves))
	{
		int x = bitwise::bitBoardX(scanIndex);
		int y = bitwise::bitBoardY(scanIndex);
		moveList[moveCounter++] = Move(xPos, yPos, x, y, PieceType::Empty, gameBoard);
		moves &= moves - 1;
	}
}

bitBoard Piece::bishopMoveBoard(int x, int y, bool color, const Board& gameBoard, bool captureOnly)
{
	bitBoard moves = bishopMoves[x][y];
	bitBoard blockers = bishopBlockerMask[x][y] & gameBoard.facts.allPieces;
	int bishopIndex = bitwise::coordToIndex(x, y);

	unsigned long scanIndex;
	while (_BitScanForward64(&scanIndex, blockers))
	{
		moves &= notBehind[bishopIndex][scanIndex];
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

void Piece::bishopMoveArray(Move* moveList, int& moveCounter, int xPos, int yPos, const Board& gameBoard, bool captureOnly)
{
	bool ownColor = gameBoard.facts.turn;
	bitBoard moves = bishopMoveBoard(xPos, yPos, ownColor, gameBoard, captureOnly);

	unsigned long scanIndex;
	while (_BitScanForward64(&scanIndex, moves))
	{
		int x = bitwise::bitBoardX(scanIndex);
		int y = bitwise::bitBoardY(scanIndex);
		moveList[moveCounter++] = Move(xPos, yPos, x, y, PieceType::Empty, gameBoard);
		moves &= moves - 1;
	}
}

bitBoard Piece::knightMoveBoard(int x, int y, bool color, const Board& gameBoard, bool captureOnly)
{
	bitBoard obstacles;
	if (color) obstacles = gameBoard.facts.allPieces & gameBoard.facts.pieces[0];
	else obstacles = gameBoard.facts.allPieces & ~gameBoard.facts.pieces[0];

	bitBoard legalDestinations = ~obstacles & knightMoves[x][y];
	if (captureOnly) legalDestinations &= gameBoard.facts.allPieces;
	return legalDestinations;
}

void Piece::knightMoveArray(Move* moveList, int& moveCounter, int xPos, int yPos, const Board& gameBoard, bool captureOnly)
{
	bool ownColor = gameBoard.getSquareColor(xPos, yPos);
	bitBoard legalDestinations = knightMoveBoard(xPos, yPos, ownColor, gameBoard, captureOnly);
	
	unsigned long legalIndex;
	while (_BitScanForward64(&legalIndex, legalDestinations))
	{
		int x = bitwise::bitBoardX(legalIndex);
		int y = bitwise::bitBoardY(legalIndex);
		moveList[moveCounter++] = Move(xPos, yPos, x, y, PieceType::Empty, gameBoard);
		legalDestinations &= legalDestinations - 1;
	}
}

bitBoard Piece::rookMoveBoard(int x, int y, bool color, const Board& gameBoard, bool captureOnly)
{
	bitBoard moves = rookMoves[x][y];
	bitBoard blockers = rookBlockerMask[x][y] & gameBoard.facts.allPieces;
	int rookIndex = bitwise::coordToIndex(x, y);

	unsigned long scanIndex;
	while (_BitScanForward64(&scanIndex, blockers))
	{
		moves &= notBehind[rookIndex][scanIndex];
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

void Piece::rookMoveArray(Move* moveList, int& moveCounter, int xPos, int yPos, const Board& gameBoard, bool captureOnly)
{
	bool ownColor = gameBoard.facts.turn;
	bitBoard moves = rookMoveBoard(xPos, yPos, ownColor, gameBoard, captureOnly);

	unsigned long scanIndex;
	while (_BitScanForward64(&scanIndex, moves))
	{
		int x = bitwise::bitBoardX(scanIndex);
		int y = bitwise::bitBoardY(scanIndex);
		moveList[moveCounter++] = Move(xPos, yPos, x, y, PieceType::Empty, gameBoard);
		moves &= moves - 1;
	}
}

void Piece::pawnMoveArray(Move* moveList, int& moveCounter, int xPos, int yPos, const Board& gameBoard, bool captureOnly)
{
	bool ownColor = gameBoard.facts.turn;
	unsigned long legalIndex;

	if (!captureOnly)
	{
		bitBoard doubleMoves = pawnDoubleMoves[ownColor][xPos][yPos] & ~gameBoard.facts.allPieces;
		if (bitwise::countBits(doubleMoves) == 2)
		{
			while (_BitScanForward64(&legalIndex, doubleMoves))
			{
				int x = bitwise::bitBoardX(legalIndex);
				int y = bitwise::bitBoardY(legalIndex);
				moveList[moveCounter++] = Move(xPos, yPos, x, y, PieceType::Empty, gameBoard);
				doubleMoves &= doubleMoves - 1;
			}
		}
		else
		{
			bitBoard singleMove = pawnSingleMoves[ownColor][xPos][yPos] & ~gameBoard.facts.allPieces;
			if (_BitScanForward64(&legalIndex, singleMove))
			{
				int x = bitwise::bitBoardX(legalIndex);
				int y = bitwise::bitBoardY(legalIndex);

				if (promotionSquares & singleMove)
				{
					moveList[moveCounter++] = Move(xPos, yPos, x, y, PieceType::Queen, gameBoard);
					moveList[moveCounter++] = Move(xPos, yPos, x, y, PieceType::Knight, gameBoard);
					moveList[moveCounter++] = Move(xPos, yPos, x, y, PieceType::Rook, gameBoard);
					moveList[moveCounter++] = Move(xPos, yPos, x, y, PieceType::Bishop, gameBoard);
				}
				else
				{
					moveList[moveCounter++] = Move(xPos, yPos, x, y, PieceType::Empty, gameBoard);
				}
			}
		}
	}
	
	bitBoard enemies = gameBoard.facts.pieces[0];
	if (ownColor) enemies = ~enemies;
	bitBoard epTargets = 0ll;
	if (gameBoard.getEPData() != -1)
	{
		Piece ep = gameBoard.getEP();
		if (ep.color)
		{
			epTargets |= bitwise::coordToBoard(ep.xPos, ep.yPos - 1);
		}
		else
		{
			epTargets |= bitwise::coordToBoard(ep.xPos, ep.yPos + 1);
		}
	}

	bitBoard captureMoves = pawnCaptureMoves[ownColor][xPos][yPos] & ((gameBoard.facts.allPieces & enemies) | epTargets);
	while (_BitScanForward64(&legalIndex, captureMoves))
	{
		int x = bitwise::bitBoardX(legalIndex);
		int y = bitwise::bitBoardY(legalIndex);
		if (promotionSquares & captureMoves)
		{
			moveList[moveCounter++] = Move(xPos, yPos, x, y, PieceType::Queen, gameBoard);
			moveList[moveCounter++] = Move(xPos, yPos, x, y, PieceType::Knight, gameBoard);
			moveList[moveCounter++] = Move(xPos, yPos, x, y, PieceType::Rook, gameBoard);
			moveList[moveCounter++] = Move(xPos, yPos, x, y, PieceType::Bishop, gameBoard);
		}
		else
		{
			moveList[moveCounter++] = Move(xPos, yPos, x, y, PieceType::Empty, gameBoard);
		}
		captureMoves &= captureMoves - 1;
	}
}

//Returns false if the Piece is in check, true otherwise
bool Piece::isSafe(const Board& gameBoard)
{
    //Check for Bishop or Queen
	bitBoard bishopMap = Piece::bishopMoveBoard(xPos, yPos, color, gameBoard, false);
	bishopMap &= gameBoard.facts.pieces[PieceType::Bishop] | gameBoard.facts.pieces[PieceType::Queen];
	if (bishopMap)
	{
		return false;
	}

    //Check for Rook or Queen
	bitBoard rookMap = Piece::rookMoveBoard(xPos, yPos, color, gameBoard, false);
	rookMap &= gameBoard.facts.pieces[PieceType::Rook] | gameBoard.facts.pieces[PieceType::Queen];
	if (rookMap)
	{
		return false;
	}

    //Check for Knight
	bitBoard knightMap = Piece::knightMoveBoard(xPos, yPos, color, gameBoard, false);
	knightMap &= gameBoard.facts.pieces[PieceType::Knight];
	if (knightMap)
	{
		return false;
	}

	//Check if next to a king
	bitBoard kingMap = Piece::kingRegularMoveBoard(xPos, yPos, color, gameBoard, false);
	kingMap &= gameBoard.facts.pieces[PieceType::King];
	if (kingMap)
	{
		return false;
	}

    //Check if Pawn ahead
    int direction = -1;
    if(color) direction = 1;

    for(int x=-1; x<=1; x+=2)
    {
        int targetX = xPos + x;
        int targetY = yPos+direction;
        if(targetX >= 8 || targetX < 0 || targetY >= 8 || targetY < 0)
        {
            continue;
        }
        if (gameBoard.squareIsPopulated(targetX, targetY))
        {
            if(gameBoard.getSquareType(targetX, targetY) == PieceType::Pawn)
            {
                if(gameBoard.getSquareColor(targetX, targetY) != color)
                {
                    return(false);
                }
            }
        }
    }

    return true;
}

