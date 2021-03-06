#include "Bitwise.h"
#include <iostream>
#include <bitset>
#include <intrin.h>

bitBoard knightMoves[64];
bitBoard pawnSingleMoves[2][64];
bitBoard pawnDoubleMoves[2][64];
bitBoard pawnCaptureMoves[2][64];
bitBoard kingMoves[64];
bitBoard notBehind[64][64];
bitBoard rookMoves[64];
bitBoard bishopMoves[64];
bitBoard queenMoves[64];
bitBoard rookBlockerMask[64];
bitBoard bishopBlockerMask[64];
bitBoard queenBlockerMask[64];
bitBoard passedPawnBlockers[2][64];
bitBoard adjacentFilesByIndex[64];

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
	return 1ull << coordToIndex(x, y);
}

bitBoard bitwise::indexToBoard(int index)
{
	return 1ull << index;
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
	bitBoard pawnPosition = 1ull << bitwise::coordToIndex(x, y);
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
	bitBoard pawnPosition = 1ull << bitwise::coordToIndex(x, y);
	if (color)
	{
		if (y != 1) {
			return 0ull;
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
			mask |= 1ull << (baseIndex + 7);
		}
		else 
		{
			mask |= 1ull << (baseIndex - 9);
		}
	}

	// Right attack
	if (x < 7)
	{
		if (color)
		{
			mask |= 1ull << (baseIndex + 9);
		}
		else
		{
			mask |= 1ull << (baseIndex - 7);
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

bool rayExists(int fx, int fy, int tx, int ty)
{
	if (fx == tx && fy == ty) return false;
	if (fx == tx || fy == ty) return true;
	if (abs(fx - tx) == abs(fy - ty)) return true;

	return false;
}

void iterateTowards(int fx, int fy, int tx, int ty, int& i, int& j)
{
	if (tx > fx) i++;
	else if (tx < fx) i--;

	if (ty > fy) j++;
	else if (ty < fy) j--;
}

bitBoard bitwise::genNotBehind(unsigned long f, unsigned long t)
{
	bitBoard base = allSquares;

	int fx = bitwise::bitBoardX(f);
	int fy = bitwise::bitBoardY(f);
	int tx = bitwise::bitBoardX(t);
	int ty = bitwise::bitBoardY(t);

	if (!rayExists(fx, fy, tx, ty)) return base;
	int i = tx;
	int j = ty;
	iterateTowards(fx, fy, tx, ty, i, j);

	while (i >= 0 && i <= 7 && j >= 0 && j <= 7)
	{
		base &= ~bitwise::coordToBoard(i, j);
		iterateTowards(fx, fy, tx, ty, i, j);
	}

	return base;
}

bitBoard bitwise::genRookMovement(int x, int y)
{
	bitBoard base = 0;

	base |= file(x);
	base |= rank(y);
	base &= ~bitwise::coordToBoard(x, y);

	return base;
}

bitBoard bitwise::genBishopMovement(int x, int y)
{
	bitBoard base = 0;
	for (int i = -1; i < 2; i += 2)
	{
		for (int j = -1; j < 2; j += 2)
		{
			for (int k = 1; k < 8; k++)
			{
				int targetX = x + i * k;
				int targetY = y + j * k;
				if (targetX < 0 || targetX > 7 || targetY < 0 || targetY > 7) break;
				base |= bitwise::coordToBoard(targetX, targetY);
			}
		}
	}

	return base;
}

bitBoard bitwise::genQueenMovement(int x, int y)
{
	return genBishopMovement(x, y) | genRookMovement(x, y);
}

void bitwise::populateBlockerMasks(int x, int y)
{
	bishopBlockerMask[bitwise::coordToIndex(x, y)] = bishopMoves[bitwise::coordToIndex(x, y)] & ~outsideRim;

	rookBlockerMask[bitwise::coordToIndex(x, y)] = rookMoves[bitwise::coordToIndex(x, y)];
	if (x != 0) rookBlockerMask[bitwise::coordToIndex(x, y)] &= ~aFile;
	if (x != 7) rookBlockerMask[bitwise::coordToIndex(x, y)] &= ~hFile;
	if (y != 0) rookBlockerMask[bitwise::coordToIndex(x, y)] &= ~rank1;
	if (y != 7) rookBlockerMask[bitwise::coordToIndex(x, y)] &= ~rank8;

	queenBlockerMask[bitwise::coordToIndex(x, y)] = bishopBlockerMask[bitwise::coordToIndex(x, y)] | rookBlockerMask[bitwise::coordToIndex(x, y)];
}

bitBoard bitwise::genPassedPawnBlockers(int x, int y, bool color)
{
	bitBoard blocks = files[x] | adjacentFiles[x];

	if (color)
	{
		for (int i = 0; i <= y; i++)
		{
			blocks &= ~ranks[i];
		}
	}
	else
	{
		for (int i = 7; i >= y; i--)
		{
			blocks &= ~ranks[i];
		}
	}

	return blocks;
}

void bitwise::initializeBitboards()
{
	for (int x = 0; x < 8; x++)
	{
		for (int y = 0; y < 8; y++)
		{
			knightMoves[bitwise::coordToIndex(x, y)] = genKnightMovement(x, y);
			pawnSingleMoves[0][bitwise::coordToIndex(x, y)] = genSinglePawnMovement(false, x, y);
			pawnSingleMoves[1][bitwise::coordToIndex(x, y)] = genSinglePawnMovement(true, x, y);
			pawnDoubleMoves[0][bitwise::coordToIndex(x, y)] = genDoublePawnMovement(false, x, y);
			pawnDoubleMoves[1][bitwise::coordToIndex(x, y)] = genDoublePawnMovement(true, x, y);
			pawnCaptureMoves[0][bitwise::coordToIndex(x, y)] = genPawnCaptureMovement(false, x, y);
			pawnCaptureMoves[1][bitwise::coordToIndex(x, y)] = genPawnCaptureMovement(true, x, y);
			kingMoves[bitwise::coordToIndex(x, y)] = genKingMovement(x, y);
			rookMoves[bitwise::coordToIndex(x, y)] = genRookMovement(x, y);
			bishopMoves[bitwise::coordToIndex(x, y)] = genBishopMovement(x, y);
			queenMoves[bitwise::coordToIndex(x, y)] = genQueenMovement(x, y);
			populateBlockerMasks(x, y);
			passedPawnBlockers[true][bitwise::coordToIndex(x, y)] = genPassedPawnBlockers(x, y, true);
			passedPawnBlockers[false][bitwise::coordToIndex(x, y)] = genPassedPawnBlockers(x, y, false);
			adjacentFilesByIndex[bitwise::coordToIndex(x, y)] = adjacentFiles[x];
		}
	}

	for (unsigned long f = 0; f < 64; f++)
	{
		for (unsigned long t = 0; t < 64; t++)
		{
			notBehind[f][t] = genNotBehind(f, t);
		}
	}
}