#include "Bitwise.h"
#include <iostream>
#include <bitset>
#include <intrin.h>

bitBoard knightMoves[8][8];

void bitwise::trimBottom(bitBoard& toTrim, int layers)
{
    for (int i = 0; i < layers; i++)
    {
		toTrim &= ~rank(i);
    }
}

void bitwise::trimTop(bitBoard& toTrim, int layers)
{
    for (int i = 0; i < layers; i++)
    {
		toTrim &= ~rank(7 - i);
    }
}

void bitwise::trimLeft(bitBoard& toTrim, int layers)
{
    for (int i = 0; i < layers; i++)
    {
		toTrim &= ~file(i);
    }
}

void bitwise::trimRight(bitBoard& toTrim, int layers)
{
    for (int i = 0; i < layers; i++)
    {
		toTrim &= ~file(7-i);
    }
}

int bitwise::countBits(bitBoard data)
{
	return __popcnt64(data);
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

int bitwise::bitBoardX(unsigned long& index)
{
	return index % 8;
}

int bitwise::bitBoardY(unsigned long& index)
{
	return index / 8;
}

bitBoard bitwise::genKnightMovement(int x, int y)
{
	bitBoard base = knightPatternC3;

	switch (x)
	{
	case(0):
		base &= ~file(1);
	case(1):
		base &= ~file(0);
		break;
	case(7):
		base &= ~file(3);
	case(6):
		base &= ~file(4);
		break;
	}

	switch (y)
	{
	case(0):
		base &= ~rank(1);
	case(1):
		base &= ~rank(0);
		break;
	case(7):
		base &= ~rank(3);
	case(6):
		base &= ~rank(4);
		break;
	}

	int toShift = -18;
	toShift += x;
	toShift += y * 8;
	if (toShift < 0)
	{
		base >>= -toShift;
	}
	else
	{
		base <<= toShift;
	}
	return base;
}

void bitwise::initializeBitboards()
{
	for (int x = 0; x < 8; x++)
	{
		for (int y = 0; y < 8; y++)
		{
			knightMoves[x][y] = genKnightMovement(x, y);
		}
	}
}