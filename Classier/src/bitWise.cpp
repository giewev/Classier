#include "Bitwise.h"
#include <iostream>
#include <bitset>
#include <intrin.h>

bitBoard knightMoves[8][8];
bitBoard pawnSingleMoves[2][8][8];
bitBoard pawnDoubleMoves[2][8][8];
bitBoard pawnCaptureMoves[2][8][8];
bitBoard kingMoves[8][8];

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

int bitwise::coordToIndex(int x, int y)
{
	return x + (8 * y);
}

bitBoard bitwise::coordToBoard(int x, int y)
{
	return 1ll << coordToIndex(x, y);
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

bitBoard bitwise::genSinglePawnMovement(bool color, int x, int y)
{
	bitBoard pawnPosition = 1ll << bitwise::coordToIndex(x, y);
	if (color)
	{
		return (pawnPosition << 8);
	}
	else
	{
		return (pawnPosition >> 8);
	}
}

bitBoard bitwise::genDoublePawnMovement(bool color, int x, int y)
{
	bitBoard pawnPosition = 1ll << bitwise::coordToIndex(x, y);
	if (color)
	{
		if (y != 1) {
			return 0ll;
		}

		return (pawnPosition << 8) | (pawnPosition << 16);
	}
	else
	{
		if (y != 6) {
			return 0ll;
		}

		return (pawnPosition >> 8) | (pawnPosition >> 16);
	}
}

bitBoard bitwise::genPawnCaptureMovement(bool color, int x, int y)
{
	int baseIndex = coordToIndex(x, y);
	bitBoard mask = 0;

	// Left attack
	if (x > 0)
	{
		if (color)
		{
			mask |= 1ll << (baseIndex + 7);
		}
		else 
		{
			mask |= 1ll << (baseIndex - 9);
		}
	}

	// Right attack
	if (x < 7)
	{
		if (color)
		{
			mask |= 1ll << (baseIndex + 9);
		}
		else
		{
			mask |= 1ll << (baseIndex - 7);
		}
	}

	return mask;
}

bitBoard bitwise::genKingMovement(int x, int y)
{
	bitBoard base = kingPatternB2;

	if (x == 0)
	{
		base &= ~aFile;
	}

	if (x == 7)
	{
		base &= ~cFile;
	}

	if (y == 0)
	{
		base &= ~rank1;
	}

	if (y == 7)
	{
		base &= ~rank3;
	}

	int toShift = -9 + bitwise::coordToIndex(x, y);
	if (toShift < 0)
	{
		return base >> -toShift;
	}
	else
	{
		return base << toShift;
	}
}

void bitwise::initializeBitboards()
{
	for (int x = 0; x < 8; x++)
	{
		for (int y = 0; y < 8; y++)
		{
			knightMoves[x][y] = genKnightMovement(x, y);
			pawnSingleMoves[0][x][y] = genSinglePawnMovement(false, x, y);
			pawnSingleMoves[1][x][y] = genSinglePawnMovement(true, x, y);
			pawnDoubleMoves[0][x][y] = genDoublePawnMovement(false, x, y);
			pawnDoubleMoves[1][x][y] = genDoublePawnMovement(true, x, y);
			pawnCaptureMoves[0][x][y] = genPawnCaptureMovement(false, x, y);
			pawnCaptureMoves[1][x][y] = genPawnCaptureMovement(true, x, y);
			kingMoves[x][y] = genKingMovement(x, y);
		}
	}
}