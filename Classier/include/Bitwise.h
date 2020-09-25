#ifndef BITWISE_H
#define BITWISE_H

#include <inttypes.h>
#define bitBoard uint64_t

enum PieceType : char
{
    Empty,
    Pawn,
    Queen,
    King,
    Bishop,
    Knight,
    Rook
};

const bitBoard rank1 = 0x00000000000000FF;
const bitBoard rank2 = rank1 << (8 * 1);
const bitBoard rank3 = rank1 << (8 * 2);
const bitBoard rank4 = rank1 << (8 * 3);
const bitBoard rank5 = rank1 << (8 * 4);
const bitBoard rank6 = rank1 << (8 * 5);
const bitBoard rank7 = rank1 << (8 * 6);
const bitBoard rank8 = rank1 << (8 * 7);

const bitBoard aFile = 0x0101010101010101;
const bitBoard bFile = aFile << 1;
const bitBoard cFile = aFile << 2;
const bitBoard dFile = aFile << 3;
const bitBoard eFile = aFile << 4;
const bitBoard fFile = aFile << 5;
const bitBoard gFile = aFile << 6;
const bitBoard hFile = aFile << 7;

const bitBoard ranks[8] =
{
	rank1,
	rank2,
	rank3,
	rank4,
	rank5,
	rank6,
	rank7,
	rank8
};

const bitBoard files[8] =
{
	aFile,
	bFile,
	cFile,
	dFile,
	eFile,
	fFile,
	gFile,
	hFile
};

const bitBoard centerBoard = 0x0000001818000000;
const bitBoard outsideRim  = 0xFF818181818181FF;

const bitBoard whitePawnChains[6] =
{
    0x0408000000000000,
    0x0004080000000000,
    0x0000040800000000,

    0x0000000000000804,
    0x0000000000080400,
    0x0000000008040000
};
const bitBoard blackPawnChains[6] =
{
    0x2010000000000000,
    0x0020100000000000,
    0x0000201000000000,

    0x0000000000001020,
    0x0000000000102000,
    0x0000000010200000
};


const bitBoard whiteSemiCenter = 0x00000000243c0000;
const bitBoard blackSemiCenter = 0x00003c2400000000;
const bitBoard a8h1 = 0x0102040810204080;
const bitBoard a1h8 = 0x8040201008040201;


const bitBoard knightMovement = 0x0000000A1100110A;
const bitBoard pawnAttack = 0x0000000000010001;

const bitBoard BishopSquareTable[4] =
{
    1800008181000018,
    2418814242811824,
    0x42A55A24245AA542,
    0x8142241818244281
};

namespace bitwise
{
void trimBottom(bitBoard& toTrim, int layers);
void trimTop(bitBoard& toTrim, int layers);
void trimLeft(bitBoard& toTrim, int layers);
void trimRight(bitBoard& toTrim, int layers);

int countBits(bitBoard data);

bitBoard knightSquares(int x, int y);

int firstBit(bitBoard input);
int lastBit(bitBoard input);

int getX(int raw);
int getY(int raw);

bitBoard file(int i);
bitBoard rank(int i);
}

#endif // BITWISE_H
