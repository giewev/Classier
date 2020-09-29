#pragma once

#include "Move.h"

struct MoveLookup
{
public:
	bool moveTable[8][8][8][8];

	void add(Move);
	bool contains(Move) const;
	void reset();
};