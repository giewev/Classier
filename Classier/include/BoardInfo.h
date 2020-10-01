#pragma once

#include "Bitwise.h"

struct BoardInfo
{
	bool turn;
	bitBoard allPieces;
	bitBoard pieces[7];
	char castlingRights;
	int EPdata;
};