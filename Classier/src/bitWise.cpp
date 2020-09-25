#include "Bitwise.h"
#include <iostream>

void bitwise::trimBottom(bitBoard& toTrim, int layers)
{
    for (int i = 0; i < layers; i++)
    {
        toTrim &= ~(rank1 << i);
    }
}

void bitwise::trimTop(bitBoard& toTrim, int layers)
{
    for (int i = 0; i < layers; i++)
    {
        toTrim &= ~(rank1 << (7 - i));
    }
}

void bitwise::trimLeft(bitBoard& toTrim, int layers)
{
    for (int i = 0; i < layers; i++)
    {
        toTrim &= ~(aFile << (i * 8));
    }
}

void bitwise::trimRight(bitBoard& toTrim, int layers)
{
    for (int i = 0; i < layers; i++)
    {
        toTrim &= ~(aFile << (8 * (7 - i)));
    }
}

int bitwise::countBits(bitBoard data)
{
    int count = 0;
    while (data)
    {
        data &= data - 1;
        count++;
    }

    return count;
}

bitBoard bitwise::knightSquares(int x, int y)
{
    bitBoard toReturn = knightMovement;
    //std::cout << toReturn << std::endl;
    if (x < 3)
    {
        trimLeft(toReturn, 3 - x);
    }
    else if (x > 6)
    {
        trimRight(toReturn, x - 3);
    }
    if (y < 3)
    {
        trimBottom(toReturn, 3 - y);
    }
    if (y > 6)
    {
        trimTop(toReturn, y - 3);
    }

    int toShift = -18;
    toShift += y;
    toShift += x * 8;
    if (toShift < 0)
    {
        toReturn >>= -toShift;
    }
    else
    {
        toReturn <<= toShift;
    }
    return toReturn;
}

int bitwise::firstBit(bitBoard input)
{
    int index = 0;
    while (!(input & 1) && index < 64)
    {
        input >>= 1;
        index++;
    }
    //std::cout << "info string ending first bit" << std::endl;
    if (index == 64)
    {
        return -1;
    }
    else
    {
        return index;
    }
}

int bitwise::lastBit(bitBoard input)
{
    bitBoard save = input;
    while (input)
    {
        input &= (input - 1);
        if (input)
        {
            save = input;
        }
    }

    return firstBit(save);
}

bitBoard bitwise::file(int i)
{
	if (i < 8 && i >= 0)
	{
		return files[i];
	}

	return 0;
}

bitBoard bitwise::rank(int i)
{
	if (i < 8 && i >= 0)
	{
		return ranks[i];
	}

	return 0;
}

int bitwise::getX(int raw)
{
    return (raw >> 3) + 1;
}

int bitwise::getY(int raw)
{
    return (raw % 8) + 1;
}
