#include "Move.h"
#include "Board.h"
#include "Piece.h"
#include "Engine.h"
#include "Bitwise.h"
#include <iostream>
#include <sstream>
#include <math.h>

namespace patch
{
template < typename T > std::string to_string( const T& n )
{
    std::ostringstream stm ;
    stm << n ;
    return stm.str() ;
}
}

Move::Move(int sIndex, int eIndex, PieceType promote, const Board& board)
{
	startIndex = sIndex;
	endIndex = eIndex;
	promotion = promote;
	score = 0;
	null = false;
	pieceCaptured = getPieceCaptured(board);
	oldEPData = board.getEPData();
	oldCastlingRights = board.getCastlingData();
}

Move::Move(std::string notation, const Board& board)
{
	int startX = ((int)notation[0]) - 97;
	int startY = ((int)notation[1]) - 48 - 1; // Subtract the extra one to zero index it
	int endX = ((int)notation[2]) - 97;
	int endY = ((int)notation[3]) - 48 - 1;

	startIndex = bitwise::coordToIndex(startX, startY);
	endIndex = bitwise::coordToIndex(endX, endY);
	promotion = PieceType::Empty;
	if (notation.length() > 4)
	{
		char p = notation[4];
		if (p == 'q') promotion = PieceType::Queen;
		else if (p == 'n') promotion = PieceType::Knight;
		else if (p == 'b') promotion = PieceType::Bishop;
		else if (p == 'r') promotion = PieceType::Rook;
	}
	score = 0;
	null = false;

	pieceCaptured = getPieceCaptured(board);
	oldEPData = board.getEPData();
	oldCastlingRights = board.getCastlingData();
}

Move::Move(const Board& board)
{
	null = true;
	score = 0;
	pieceCaptured = getPieceCaptured(board);
	oldEPData = board.getEPData();
	oldCastlingRights = board.getCastlingData();
	startIndex = -1;
	endIndex = -1;
}

Move::Move()
{
	null = true;
	score = 0;
}

void Move::setScore(double scoreVal)
{
    score = scoreVal;
}

void Move::setGameOverDepth(int depthVal)
{
    if (score > 0)
    {
        score = 999 - depthVal;
    }
    if (score < 0)
    {
        score = -999 + depthVal;
    }
}

double Move::getScore()
{
    return score;
}

int Move::getGameOverDepth()
{
    double absScore = fabs(score);
    if (absScore > 900)
    {
        return 999 - absScore;
    }

    return -1;
}

int Move::getMateDistance()
{
	int depth = getGameOverDepth();
	if (depth != -1)
	{
		if (score > 0)
		{
			return (depth + 1) / 2;
		}
		else 
		{
			return -(depth + 1) / 2;
		}
	}
	
	return 0;
}

bool Move::isSafe(Board& gameBoard)
{
	bool kingColor = gameBoard.facts.turn;
	gameBoard.makeMove(*this);
	int kingIndex = gameBoard.getKingIndex(kingColor);
	//bool safe = gameBoard.getSquare(kingX, kingY).isSafe(gameBoard);
	bool safe = !gameBoard.squareAttacked(kingIndex, !kingColor);
	gameBoard.unmakeMove(*this);

	kingIndex = gameBoard.getKingIndex(kingColor);
	if (safe)
	{
		// Special check if the in between square is being attacked when you castle
		if (startIndex == kingIndex)
		{
			if (fabs(startIndex - endIndex) == 2)
			{
				int betweenIndex = (startIndex + endIndex) / 2;
				return Move(startIndex, betweenIndex, PieceType::Empty, gameBoard).isSafe(gameBoard) &&
					Move(gameBoard).isSafe(gameBoard);
			}
		}
	}

	
	return safe;
}

std::string Move::basicAlg()
{
    std::string move = "";
	int startX = startIndex % 8;
	int startY = startIndex / 8;
	int endX = endIndex % 8;
	int endY = endIndex / 8;
    move = Engine::toAlg(startX) + patch::to_string(startY + 1) + Engine::toAlg(endX) + patch::to_string(endY + 1);
	switch (promotion)
	{
	case Queen:
		move += "q";
		break;
	case Bishop:
		move += "b";
		break;
	case Knight:
		move += "n";
		break;
	case Rook:
		move += "r";
		break;
	}
    return move;
}

bool Move::operator==(Move other) const
{
	return startIndex == other.startIndex &&
		endIndex == other.endIndex &&
		promotion == other.promotion;
}

bool Move::operator!=(Move other) const
{
	return !(this->operator==(other));
}

bool Move::operator<(Move other) const
{
    if (score < other.score)
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool Move::operator>(Move other) const
{
    if (score > other.score)
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool Move::bigger(Move left, Move right)
{
    if (left > right)
    {
        return true;
    }
    else
    {
        return false;
    }
}

PieceType Move::getPieceCaptured(const Board& gameBoard)
{
	return gameBoard.getSquareType(endIndex);
}
