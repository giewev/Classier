#include "Piece.h"
#include "Board.h"
#include "Move.h"
#include "Danger.h"
#include "Engine.h"
#include "Bitwise.h"
#include <vector>
#include <math.h>
#include <iostream>
#include <string.h>
#include <algorithm>

Danger::Danger(const Board& gameBoard) : dataBoard(gameBoard)
{
    for(int i=0; i<8; i++)
    {
        pinCoordinates[i] = -1;
        pinDirections[i] = -1;
    }
    for(int i=0; i<7; i++)
    {
        safeSquares[i] = -1;
    }

    check = false;
    doubleCheck = false;
    pinCount = 0;
    safeSquareCount = 0;
    Piece King = gameBoard.findKing(gameBoard.turn);
    defenderX = King.getX();
    defenderY = King.getY();

    loadData(gameBoard, gameBoard.findKing(gameBoard.turn));
}

Danger::Danger(const Board& gameBoard, Piece defender) : dataBoard(gameBoard)
{
    for(int i=0; i<8; i++)
    {
        pinCoordinates[i] = -1;
        pinDirections[i] = -1;
    }
    for(int i=0; i<7; i++)
    {
        safeSquares[i] = -1;
    }

    check = false;
    doubleCheck = false;
    pinCount = 0;
    safeSquareCount = 0;
    defenderX = defender.getX();
    defenderY = defender.getY();

    loadData(gameBoard, defender);
}

Danger::~Danger()
{
}

int Danger::getPinCount()
{
    return pinCount;
}

int Danger::getSafeSquareCount()
{
    return safeSquareCount;
}

int Danger::getPinX(int index)
{
    return (pinCoordinates[index] / 8) & 7;
}

int Danger::getPinY(int index)
{
    return (pinCoordinates[index] % 8) & 7;
}

bool Danger::getCheck()
{
    return check;
}

bool Danger::getDoubleCheck()
{
    return doubleCheck;
}

Board Danger::getBoard()
{
    return dataBoard;
}

bool Danger::inSafeSquares(int x, int y)
{
    int safeX, safeY;
    for(int i=0; i<safeSquareCount; i++)
    {
        safeX = getSafeX(i);
        safeY = getSafeY(i);
        if(safeY == y && safeX == x)
        {
            return true;
        }
    }
    return false;
}
bool Danger::inPinCoordinates(int x, int y)
{
    int pinX, pinY;
    for(int i=0; i < pinCount; i++)
    {
        pinX = getPinX(i);
        pinY = getPinY(i);
        if(pinY == -1)
        {
            return false;
        }
        if(pinY == y && pinX == x)
        {
            return true;
        }
    }
    return false;
}
bool Danger::movePinned(Move pinCheck)
{
    int toPieceX;
    int toPieceY;
    int targetX;
    int targetY;

    toPieceX = pinCheck.startX - defenderX;
    toPieceY = pinCheck.startY - defenderY;
    targetX = pinCheck.endX - pinCheck.startX;
    targetY = pinCheck.endY - pinCheck.startY;

    int pinX, pinY;
    int i;
    for (i = 0; i < pinCount; i++)
    {
        pinX = getPinX(i);
        pinY = getPinY(i);
        if (pinY == pinCheck.startY && pinX == pinCheck.startX)
        {
            break;
        }
    }
    if (i == pinCount)
    {
        return false;
    }

    if (toPieceX == 0 || targetX == 0)
    {
        if (toPieceX == targetX)
        {
            return false;
        }
        return true;
    }
    else if (toPieceY == 0 || targetY == 0)
    {
        if (toPieceY == targetY)
        {
            return false;
        }
        return true;
    }
    else if (toPieceY / toPieceX == targetY / targetX)
    {
        return false;
    }
    return true;
}

void Danger::addAttacker(int x, int y)
{
    int modifier = 1;
    if(check)
    {
        doubleCheck = true;
        return;
    }
    else
    {
        check = true;
    }
    Piece target;

    //Vertical
    if(x == defenderX)
    {
        if(y < defenderY) //Down
        {
            modifier = -1;
        }

        for(int i=1; i<8; i++)
        {
            if(defenderY + i*modifier >= 8 || defenderY + i*modifier < 0)
            {
                break;
            }
            target = dataBoard.getSquare(x, defenderY + i * modifier);
            setSafeSquare(i-1, defenderX, defenderY+i*modifier);
            if(!target.isNull())
            {
                safeSquareCount = i;
                break;
            }
        }
    }

    //Horizontal
    else if(y == defenderY)
    {

        //std::cout << "hor" << std::endl;
        if(x < defenderX) //Left
        {
            modifier = - 1;
        }

        for(int i=1; i<8; i++)
        {
            if(defenderX + i*modifier >= 8 || defenderX + i*modifier < 0)
            {
                break;
            }
            target = dataBoard.getSquare(defenderX + i * (modifier), y);
            setSafeSquare(i-1, defenderX+i*modifier, defenderY);
            if(!target.isNull())
            {
                //std::cout << defenderX+i*modifier << " " << defenderY << std::endl;
                safeSquareCount = i;
                break;
            }
        }
    }

    //Diagonal
    else if(fabs((defenderX - x) / (defenderY - y)) == 1)
    {
        int altModifier = 1;
        if(y > defenderY)
        {
            altModifier = -1;
        }
        if(x > defenderX)
        {
            modifier = -1;
        }

        for(int i=0; i<8; i++)
        {
            //Out of bounds
            if(x + i*modifier >= 8 || x + i*modifier < 0 || y + i*altModifier >= 8 || y + i*altModifier < 0)
            {
                break;
            }
            target = dataBoard.getSquare(x + i*modifier, y + i*altModifier);
            if(target.type == PieceType::King)
            {
                safeSquareCount = i;
                break;
            }
            setSafeSquare(i, x+i*modifier, y+i*altModifier);
        }
    }

    //Knight
    else
    {
        setSafeSquare(0, x, y);
        safeSquareCount = 1;
    }
}
void Danger::addPin(int x, int y)
{
    pinCoordinates[pinCount] = x*8 + y;
    pinCount++;
}
void Danger::addPin(int data)
{
    pinCoordinates[pinCount] = data;
    pinCount++;
}

void Danger::loadData(const Board& gameBoard, Piece defender)
{
    bool color = gameBoard.turn;
    int direction = -1;
    if(color)
    {
        direction = 1;
    }
    int x = defender.getX();
    int y = defender.getY();

    //UP
    bool trace = false;
    int traceX = 0;
    int traceY = 0;
    for (int i = 1; i <= 7-y; i++)
    {
        PieceType targetType = gameBoard.getSquareType(x, y+i);
        bool targetColor = gameBoard.getSquareColor(x, y+i);
        if(targetType)
        {
            if(color == targetColor)
            {
                if(!trace)
                {
                    trace = true;
                    traceX = x;
                    traceY = y + i;
                }
                else
                {
                    break;
                }
            }
            else if(targetType == PieceType::Rook || targetType == PieceType::Queen)
            {
                if(trace)
                {
                    addPin(traceX, traceY);
                    pinDirections[pinCount - 1] = 9;
                    break;
                }
                else
                {
                    addAttacker(x, y+i);
                    break;
                }
            }
            else
            {
                break;
            }
        }
    }

    //DOWN
    trace = false;
    traceX = 0;
    traceY = 0;
    for (int i = 1; i <= y; i++)
    {
        PieceType targetType = gameBoard.getSquareType(x, y-i);
        bool targetColor = gameBoard.getSquareColor(x, y-i);
        if(targetType)
        {
            if(color == targetColor)
            {
                if(!trace)
                {
                    trace = true;
                    traceX = x;
                    traceY = y - i;
                }
                else
                {
                    break;
                }
            }
            else if(targetType == PieceType::Rook || targetType == PieceType::Queen)
            {
                if(trace)
                {
                    addPin(traceX, traceY);
                    pinDirections[pinCount - 1] = 1;
                    break;
                }
                else
                {
                    addAttacker(x, y-i);
                    break;
                }
            }
            else
            {
                break;
            }
        }
    }

    //LEFT
    trace = false;
    traceX = 0;
    traceY = 0;
    for (int i = 1; i <= x; i++)
    {
        PieceType targetType = gameBoard.getSquareType(x-i, y);
        bool targetColor = gameBoard.getSquareColor(x-i, y);
        if(targetType)
        {
            if(color == targetColor)
            {
                if(!trace)
                {
                    trace = true;
                    traceX = x-i;
                    traceY = y;
                }
                else
                {
                    break;
                }
            }
            else if(targetType == PieceType::Rook || targetType == PieceType::Queen)
            {
                if(trace)
                {
                    addPin(traceX, traceY);
                    pinDirections[pinCount - 1] = 4;
                    break;
                }
                else
                {
                    addAttacker(x-i, y);
                    break;
                }
            }
            else
            {
                break;
            }
        }
    }

    //RIGHT
    trace = false;
    traceX = 0;
    traceY = 0;
    for (int i = 1; i <= 7-x; i++)
    {
        PieceType targetType = gameBoard.getSquareType(x+i, y);
        bool targetColor = gameBoard.getSquareColor(x+i, y);
        if(targetType)
        {
            if(color == targetColor)
            {
                if(!trace)
                {
                    trace = true;
                    traceX = x+i;
                    traceY = y;
                }
                else
                {
                    break;
                }
            }
            else if(targetType == PieceType::Rook || targetType == PieceType::Queen)
            {
                if(trace)
                {
                    addPin(traceX, traceY);
                    pinDirections[pinCount - 1] = 6;
                    break;
                }
                else
                {
                    addAttacker(x+i, y);
                    break;
                }
            }
            else
            {
                break;
            }
        }
    }

    //NE
    trace = false;
    traceX = 0;
    traceY = 0;
    for (int i = 1; i <= std::min(7-x, 7-y); i++)
    {
        PieceType targetType = gameBoard.getSquareType(x+i, y+i);
        bool targetColor = gameBoard.getSquareColor(x+i, y+i);
        if(targetType)
        {
            if(color == targetColor)
            {
                if(!trace)
                {
                    trace = true;
                    traceX = x+i;
                    traceY = y+i;
                }
                else
                {
                    break;
                }
            }
            else if(targetType == PieceType::Bishop || targetType == PieceType::Queen)
            {
                if(trace)
                {
                    addPin(traceX, traceY);
                    pinDirections[pinCount - 1] = 10;
                    break;
                }
                else
                {
                    addAttacker(x+i, y+i);
                    break;
                }
            }
            else
            {
                break;
            }
        }
    }

    //SW
    trace = false;
    traceX = 0;
    traceY = 0;
    for (int i = 1; i <= std::min(x, y); i++)
    {
        PieceType targetType = gameBoard.getSquareType(x-i, y-i);
        bool targetColor = gameBoard.getSquareColor(x-i, y-i);
        if(targetType)
        {
            if(color == targetColor)
            {
                if(!trace)
                {
                    trace = true;
                    traceX = x-i;
                    traceY = y-i;
                }
                else
                {
                    break;
                }
            }
            else if(targetType == PieceType::Bishop || targetType == PieceType::Queen)
            {
                if(trace)
                {
                    addPin(traceX, traceY);
                    pinDirections[pinCount - 1] = 0;
                    break;
                }
                else
                {
                    addAttacker(x-i, y-i);
                    break;
                }
            }
            else
            {
                break;
            }
        }
    }

    //NW
    trace = false;
    traceX = 0;
    traceY = 0;
    for (int i = 1; i <= std::min(x, 7-y); i++)
    {
        PieceType targetType = gameBoard.getSquareType(x-i, y+i);
        bool targetColor = gameBoard.getSquareColor(x-i, y+i);
        if(targetType != PieceType::Empty)
        {
            if(color == targetColor)
            {
                if(!trace)
                {
                    trace = true;
                    traceX = x-i;
                    traceY = y+i;
                }
                else
                {
                    break;
                }
            }
            else if(targetType == PieceType::Bishop || targetType == PieceType::Queen)
            {
                if(trace)
                {
                    addPin(traceX, traceY);
                    pinDirections[pinCount - 1] = 8;
                    break;
                }
                else
                {
                    addAttacker(x-i, y+i);
                    break;
                }
            }
            else
            {
                break;
            }
        }
    }

    //SE
    trace = false;
    traceX = 0;
    traceY = 0;
    for (int i = 1; i <= std::min(7-x, y); i++)
    {
        PieceType targetType = gameBoard.getSquareType(x+i, y-i);
        bool targetColor = gameBoard.getSquareColor(x+i, y-i);
        if(targetType)
        {
            if(color == targetColor)
            {
                if(!trace)
                {
                    trace = true;
                    traceX = x+i;
                    traceY = y-i;
                }
                else
                {
                    break;
                }
            }
            else if(targetType == PieceType::Bishop || targetType == PieceType::Queen)
            {
                if(trace)
                {
                    addPin(traceX, traceY);
                    pinDirections[pinCount - 1] = 2;
                    break;
                }
                else
                {
                    addAttacker(x+i, y-i);
                    break;
                }
            }
            else
            {
                break;
            }
        }
    }

    //PieceType::Pawn
    if(y + direction <= 7 && y + direction >= 0)
    {
        for(int i = -1; i <= 1; i+=2)
        {
            if(x + i <= 7 && x + i >= 0)
            {
                PieceType targetType = gameBoard.getSquareType(x + i, y + direction);
                int targetColor = gameBoard.getSquareColor(x + i, y + direction);

                if(targetType == PieceType::Pawn && targetColor != color)
                {
                    addAttacker(x + i, y + direction);
                }
            }
        }
    }

    //PieceType::Knight
    PieceType targetType;
    bool targetColor;
    for(int i = -2; i <= 2; i++)
    {
        for(int j = -2; j<= 2; j++)
        {
            if(fabs(i) == fabs(j) || i == 0 || j == 0)
            {
                continue;
            }

            if(x + i < 8 && x + i >= 0)
            {
                if(y + j < 8 && y + j >= 0)
                {
                    targetType = gameBoard.getSquareType(x+i, y+j);
                    targetColor = gameBoard.getSquareColor(x+i, y+j);

                    if(targetType == PieceType::Knight && targetColor != color)
                    {
                        addAttacker(x+i, y+j);
                    }
                }
            }
        }
    }
}



void Danger::setSafeSquare(int index, int x, int y)
{
    safeSquares[index] = 0;
    safeSquares[index] = x;
    safeSquares[index] += y << 3;
}

int Danger::getSafeX(int index)
{
    return safeSquares[index] & 7;
}
int Danger::getSafeY(int index)
{
    return safeSquares[index] >> 3;
}

void Danger::printSafeSquares()
{
    std::cout << "There are " << safeSquareCount << " safe squares" << std::endl;
    for (int i = 0; i < safeSquareCount; i++)
    {
        std::cout << Engine::toAlg(getSafeX(i)) << getSafeY(i) << std::endl;
    }
}
