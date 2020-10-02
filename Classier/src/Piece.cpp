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

void Piece::generateMoves(std::vector<Move>& moveList, int x, int y, const Board& gameBoard)
{
    Move moveArray[220];
    int moveCounter = 0;
    appendMoveArray(moveArray, moveCounter, x, y, gameBoard);
    for (int i = 0; i < moveCounter; i++)
    {
        moveList.push_back(moveArray[i]);
    }
}

void Piece::appendMoveArray(Move* moveList, int& moveCounter, int x, int y, const Board& gameBoard)
{
    PieceType type = gameBoard.getSquareType(x, y);
    if(type == PieceType::Empty)
    {
    }
    else if(type == PieceType::Pawn)
    {
        pawnMoveArray(moveList, moveCounter, x, y, gameBoard);
    }
    else if(type == PieceType::King)
    {
        kingMoveArray(moveList, moveCounter, x, y, gameBoard);
    }
    else if(type == PieceType::Queen)
    {
        queenMoveArray(moveList, moveCounter, x, y, gameBoard);
    }
    else if(type == PieceType::Bishop)
    {
        bishopMoveArray(moveList, moveCounter, x, y, gameBoard);
    }
    else if(type == PieceType::Knight)
    {
        knightMoveArray(moveList, moveCounter, x, y, gameBoard);
    }
    else if(type == PieceType::Rook)
    {
        rookMoveArray(moveList, moveCounter, x, y, gameBoard);
    }
    else
    {
        std::cout << "No matching piece in appendMoveArray" << std::endl;
    }
    return;
}

void Piece::kingMoveArray(Move* moveList, int& moveCounter, int xPos, int yPos, const Board& gameBoard)
{
    bool ownColor = gameBoard.getSquareColor(xPos, yPos);
    for (int j=-1; j<=1; j++)
    {
        for (int k=-1; k<=1; k++)
        {
            if(j==0 && k==0) //if not moving
            {
                continue;
            }

            if(xPos+j >= 8 || xPos+j < 0) //if moving horizontally off the board
            {
                continue;
            }
            if(yPos+k >= 8 || yPos+k < 0) //if moving vertically off the board
            {
                continue;
            }

            //Check to see if Friendly Piece is there
            if(gameBoard.squareIsPopulated(xPos + j, yPos + k))
            {
                if(gameBoard.getSquareColor(xPos + j, yPos + k) == ownColor)
                {
                    continue;
                }
            }
            //Everything is good, so make a move and add it to the moveList
            moveList[moveCounter++] = Move(xPos, yPos, xPos+j, yPos+k, PieceType::Empty, gameBoard);
        }
    }

    //CastlingMoves
    for(int i=0; i<=1; i++)
    {
        if(gameBoard.getCastlingRights(ownColor, i==1))
        {
            if(i)
            {
                if(gameBoard.squareIsPopulated(xPos+1, yPos))
                {
                    continue;
                }
                if(gameBoard.squareIsPopulated(xPos+2, yPos))
                {
                    continue;
                }

                moveList[moveCounter++] = Move(xPos, yPos, xPos + 2, yPos, PieceType::Empty, gameBoard);
            }
            else
            {
                if(gameBoard.squareIsPopulated(xPos-1, yPos))
                {
                    continue;
                }
                if(gameBoard.squareIsPopulated(xPos-2, yPos))
                {
                    continue;
                }

                if(gameBoard.squareIsPopulated(xPos-3, yPos))
                {
                    continue;
                }
                moveList[moveCounter++] = Move(xPos, yPos, xPos - 2, yPos, PieceType::Empty, gameBoard);
            }
        }
    }
}

void Piece::queenMoveArray(Move* moveList, int& moveCounter, int xPos, int yPos, const Board& gameBoard)
{
    rookMoveArray(moveList, moveCounter, xPos, yPos, gameBoard);
    bishopMoveArray(moveList, moveCounter, xPos, yPos, gameBoard);
}

void Piece::bishopMoveArray(Move* moveList, int& moveCounter, int xPos, int yPos, const Board& gameBoard)
{
	bool ownColor = gameBoard.facts.turn;
    for(int i=-1; i<=1; i+=2)
    {
        for(int j=-1; j<=1; j+=2)
        {
            if(i==0 || j==0)
            {
                continue;
            }
            for(int k=1; k<=7; k++)
            {

                if(xPos+(k*i) < 0 || xPos+(k*i) >= 8 || yPos+(k*j) < 0 || yPos+(k*j) >= 8) //Too Far
                {
                    break;
                }

                if(gameBoard.squareIsPopulated(xPos+(k*i), yPos+(k*j)))
                {
                    if(gameBoard.getSquareColor(xPos+(k*i), yPos+(k*j)) == ownColor)
                    {
                        break;
                    }
                    else
                    {
                        moveList[moveCounter++] = Move(xPos, yPos, xPos+(k*i), yPos+(k*j), PieceType::Empty, gameBoard);
                        break;
                    }
                }

                moveList[moveCounter++] = Move(xPos, yPos, xPos+(k*i), yPos+(k*j), PieceType::Empty, gameBoard);

            }
        }
    }
}

void Piece::knightMoveArray(Move* moveList, int& moveCounter, int xPos, int yPos, const Board& gameBoard)
{
	bool ownColor = gameBoard.getSquareColor(xPos, yPos);
	bitBoard obstacles;
	if (ownColor) obstacles = gameBoard.facts.allPieces & gameBoard.facts.pieces[0];
	else obstacles = gameBoard.facts.allPieces & ~gameBoard.facts.pieces[0];

	bitBoard legalDestinations = ~obstacles & knightMoves[xPos][yPos];
	unsigned long legalIndex;
	while (_BitScanForward64(&legalIndex, legalDestinations))
	{
		int x = bitwise::bitBoardX(legalIndex);
		int y = bitwise::bitBoardY(legalIndex);
		moveList[moveCounter++] = Move(xPos, yPos, x, y, PieceType::Empty, gameBoard);
		legalDestinations &= legalDestinations - 1;
	}
}

void Piece::rookMoveArray(Move* moveList, int& moveCounter, int xPos, int yPos, const Board& gameBoard)
{
    bool ownColor = gameBoard.getSquareColor(xPos, yPos);
    for(int i=-1; i<=1; i++)
    {
        for(int j=-1; j<=1; j++)
        {
            if(!i == !j) // diagonal or no motion
            {
                continue;
            }
            for(int k = 1; k < 8; k++)
            {

                if(xPos+(i*k) < 0 || xPos+(i*k) >= 8) //out of bounds X
                {
                    continue;
                }
                if(yPos+(j*k) < 0 || yPos+(j*k) >= 8) //out of Bounds Y
                {
                    continue;
                }

                if(gameBoard.squareIsPopulated(xPos+(i*k), yPos+(j*k)))
                {
                    if(gameBoard.getSquareColor(xPos+(i*k), yPos+(j*k)) == ownColor)
                    {
                        break;
                    }
                    else
                    {
                        moveList[moveCounter++] = Move(xPos, yPos, xPos+(k*i), yPos+(k*j), PieceType::Empty, gameBoard);
                        break;
                    }
                }

                moveList[moveCounter++] = Move(xPos, yPos, xPos+(k*i), yPos+(k*j), PieceType::Empty, gameBoard);
            }
        }
    }
}

void Piece::pawnMoveArray(Move* moveList, int& moveCounter, int xPos, int yPos, const Board& gameBoard)
{
    bool ownColor = gameBoard.getSquareColor(xPos, yPos);
    int direction = -1;
    if(ownColor)
    {
        direction = 1;
    }

    if(!gameBoard.squareIsPopulated(xPos, yPos + direction)) //Normal Moves
    {
        if(yPos+direction == 7 || yPos+direction == 0)
        {
            moveList[moveCounter++] = Move(xPos, yPos, xPos, yPos+direction, PieceType::Queen, gameBoard);
            moveList[moveCounter++] = Move(xPos, yPos, xPos, yPos+direction, PieceType::Knight, gameBoard);
            moveList[moveCounter++] = Move(xPos, yPos, xPos, yPos+direction, PieceType::Rook, gameBoard);
            moveList[moveCounter++] = Move(xPos, yPos, xPos, yPos+direction, PieceType::Bishop, gameBoard);
        }
        else
        {
            moveList[moveCounter++] = Move(xPos, yPos, xPos, yPos+direction, PieceType::Empty, gameBoard);
        }

        if((yPos == 1 && ownColor) || (yPos == 6 && !ownColor)) //Double Moves
        {
            if(!gameBoard.squareIsPopulated(xPos, yPos + 2 * direction))
            {
                moveList[moveCounter++] = Move(xPos, yPos, xPos, yPos+2*direction, PieceType::Empty, gameBoard);
            }
        }
    }

    //Capture Moves
    for(int i = -1; i<=1; i+=2)
    {
        if(xPos+i >= 8 || xPos+i < 0)
        {
            continue;
        }

        if(!gameBoard.squareIsPopulated(xPos + i, yPos + direction))
        {
            if(yPos == gameBoard.getEP().getY())
            {
                if(xPos + i == gameBoard.getEP().getX())
                {
                    if(gameBoard.getEP().getColor() != ownColor)
                    {
                        moveList[moveCounter++] = Move(xPos, yPos, xPos+i, yPos+direction, PieceType::Empty, gameBoard);
                    }
                }
            }
            continue;
        }

        if(gameBoard.getSquareColor(xPos + i, yPos + direction) == ownColor)
        {
            continue;
        }

        if(yPos+direction == 7 || yPos+direction == 0)
        {
            moveList[moveCounter++] = Move(xPos, yPos, xPos+i, yPos+direction, PieceType::Queen, gameBoard);
            moveList[moveCounter++] = Move(xPos, yPos, xPos+i, yPos+direction, PieceType::Knight, gameBoard);
            moveList[moveCounter++] = Move(xPos, yPos, xPos+i, yPos+direction, PieceType::Rook, gameBoard);
            moveList[moveCounter++] = Move(xPos, yPos, xPos+i, yPos+direction, PieceType::Bishop, gameBoard);
        }
        else
        {
            moveList[moveCounter++] = Move(xPos, yPos, xPos+i, yPos+direction, PieceType::Empty, gameBoard);
        }
    }
}

//Returns false if the Piece is in check, true otherwise
bool Piece::isSafe(const Board& gameBoard)
{
    //Check for Bishop or Queen
    for(int i=-1; i<=1; i+=2)
    {
        for(int j=-1; j<=1; j+=2)
        {
            for(int k=1; k<=7; k++)
            {
                int targetX = xPos+(i*k);
                int targetY = yPos+(j*k);
                //Out of bounds
                if(targetX >= 8 || targetX < 0 || targetY >= 8 || targetY < 0)
                {
                    break;
                }

                //Found a piece
                if(gameBoard.squareIsPopulated(targetX, targetY))
                {
                    bool targetColor = gameBoard.getSquareColor(targetX, targetY);
                    if(targetColor != color)
                    {
                        PieceType targetType = gameBoard.getSquareType(targetX, targetY);
                        if(targetType == PieceType::Bishop || targetType == PieceType::Queen)
                        {
                            return(false);
                        }
                        break;
                    }
                    break;
                }
            }
        }
    }

    //Check for Rook or Queen
    for(int i=-1; i<=1; i++)
    {
        for(int j=-1; j<=1; j++)
        {
            if(!i == !j)
            {
                continue;
            }

            for(int k=1; k<=7; k++)
            {
                int targetX = xPos+(i*k);
                int targetY = yPos+(j*k);
                //Out of bounds
                if(targetX >= 8 || targetX < 0 || targetY >= 8 || targetY < 0)
                {
                    break;
                }

                //Found a piece
                if(gameBoard.squareIsPopulated(targetX, targetY))
                {
                    int targetColor = gameBoard.getSquareColor(targetX, targetY);
                    if(targetColor != color)
                    {
                        PieceType targetType = gameBoard.getSquareType(targetX, targetY);
                        if(targetType == PieceType::Rook || targetType == PieceType::Queen)
                        {
                            return(false);
                        }
                        break;
                    }
                    break;
                }
            }
        }
    }

    //Check for Knight
    for(int i=-2; i<=2; i+= 4)
    {
        for(int j=-1; j<=1; j+=2)
        {
            int targetX = xPos+i;
            int targetY = yPos+j;
            if(targetX >= 0 && targetX < 8 && targetY >= 0 && targetY < 8) // Within bounds
            {
                if(gameBoard.squareIsPopulated(targetX, targetY))
                {
                    bool targetColor = gameBoard.getSquareColor(targetX, targetY);
                    if(targetColor != color)
                    {
                        PieceType targetType = gameBoard.getSquareType(targetX, targetY);
                        if(targetType == PieceType::Knight)
                        {
                            return false;
                        }
                    }
                }
            }

            targetX = xPos+j;
            targetY = yPos+i;
            if(targetX >= 0 && targetX < 8 && targetY >= 0 && targetY < 8) // Within bounds
            {
                if(gameBoard.squareIsPopulated(targetX, targetY))
                {
                    bool targetColor = gameBoard.getSquareColor(targetX, targetY);
                    if(targetColor != color)
                    {
                        PieceType targetType = gameBoard.getSquareType(targetX, targetY);
                        if(targetType == PieceType::Knight)
                        {
                            return false;
                        }
                    }
                }
            }
        }
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

    //Check if next to a king
    for(int x=-1; x<=1; x++)
    {
        for(int y=-1; y<=1; y++)
        {

            if(!x && !y) //No motion
            {
                continue;
            }

            int targetX = xPos + x;
            int targetY = yPos + y;
            if(targetX >= 8 || targetX < 0) //if checking horizontally off the board
            {
                continue;
            }
            if(targetY >= 8 || targetY < 0) //if checking vertically off the board
            {
                continue;
            }

            if (gameBoard.squareIsPopulated(targetX, targetY))
            {
                if(gameBoard.getSquareType(targetX, targetY) == PieceType::King)
                {
                    if (gameBoard.getSquareColor(targetX, targetY) != color)
                    {
                        return(false);
                    }
                }
            }
        }
    }

    return true;
}

