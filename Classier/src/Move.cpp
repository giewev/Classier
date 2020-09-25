#include "Move.h"
#include "Danger.h"
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

Move::Move(int x1, int y1, int x2, int y2)
{
    Board::throwIfOutOfBounds(x1, y1);
    Board::throwIfOutOfBounds(x2, y2);
    startX = x1;
    startY = y1;
    endX = x2;
    endY = y2;
    promotion = PieceType::Empty;
    score = 0;
	null = false;
}

Move::Move(int x1, int y1, int x2, int y2, PieceType promote)
{
    Board::throwIfOutOfBounds(x1, y1);
    Board::throwIfOutOfBounds(x2, y2);
    startX = x1;
    startY = y1;
    endX = x2;
    endY = y2;
    promotion = promote;
    score = 0;
	null = false;
}

Move::Move(std::string notation)
{
	startX = ((int)notation[0]) - 97;
	startY = ((int)notation[1]) - 48 - 1; // Subtract the extra one to zero index it
	endX = ((int)notation[2]) - 97;
	endY = ((int)notation[3]) - 48 - 1;
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

	Board::throwIfOutOfBounds(startX, startY);
	Board::throwIfOutOfBounds(endX, endY);
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

bool Move::isSafe(Danger safetyData)
{
    if(safetyData.movePinned(*this))
    {
        return false;
    }

    // Check for en passant capture, as this needs a manual check
    if (startY == 3 || startY == 4)
    {
        if (fabs(startX - endX) == 1 && fabs(startY - endY) == 1)
        {
            if (!safetyData.getBoard().squareIsPopulated(endX, endY))
            {
                if (safetyData.getBoard().squareIsType(endX, startY, PieceType::Pawn))
                {
                    Board newBoard = safetyData.getBoard().newCopy();
                    newBoard.makeMove(*this);
                    if (newBoard.getSquare(newBoard.getKingX(!newBoard.turn), newBoard.getKingY(!newBoard.turn)).isSafe(newBoard))
                    {
                        return true;
                    }
                    else
                    {
                        return false;
                    }
                }
            }
        }
    }

    //The king is moving, need a manual check
    if(startX == safetyData.defenderX && startY == safetyData.defenderY)
    {
        Board newBoard;
        //Castling Moves
        if(fabs(startX - endX) == 2)
        {
            //Cant castle out of check
            if(safetyData.getCheck())
            {
                return false;
            }
            //iterates over the squares next to the king for two squares
            //The absolute value division thing finds the sign of the direction
            int direction = -1*fabs(startX-endX)/(startX-endX);
            for (int i = startX + direction; i <= startX + 2 && i >= startX - 2; i+=direction)
            {
                newBoard = safetyData.getBoard().newCopy();
                newBoard.makeMove(Move(startX, startY, i, startY));
                if(!newBoard.getSquare(i, endY).isSafe(newBoard))
                {
                    return false;
                }
            }
            return true;
        }
        //Normal King moves
        newBoard = safetyData.getBoard().newCopy();
        newBoard.makeMove(*this);
        if(newBoard.getSquare(endX, endY).isSafe(newBoard))
        {
            return true;
        }
        else
        {
            return false;
        }
    }
    if(safetyData.getDoubleCheck())
    {
        //Only the King can move, and we checked if the king was moving earlier
        return false;
    }
    else if(safetyData.getCheck())
    {
        if(safetyData.inSafeSquares(endX, endY))
        {
            return true;
        }
        else
        {
            return false;
        }
    }
    return true;
}

std::string Move::basicAlg()
{
    std::string move = "";
    move = Engine::toAlg(startX) + patch::to_string(startY + 1) + Engine::toAlg(endX) + patch::to_string(endY + 1);
    return move;
}

bool Move::operator==(Move other) const
{
    if(startX != other.startX)
    {
        return false;
    }
    if(startY != other.startY)
    {
        return false;
    }
    if(endX != other.endX)
    {
        return false;
    }
    if(endY != other.endY)
    {
        return false;
    }
    if(promotion != other.promotion)
    {
        return false;
    }
    return true;
}

bool Move::operator!=(Move other) const
{
    if(startX != other.startX)
    {
        return true;
    }
    if(startY != other.startY)
    {
        return true;
    }
    if(endX != other.endX)
    {
        return true;
    }
    if(endY != other.endY)
    {
        return true;
    }
    if(promotion != other.promotion)
    {
        return true;
    }
    return false;
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

bool Move::isCapture(const Board& safeBoard)
{
    return safeBoard.squareIsPopulated(endX, endY);
}
