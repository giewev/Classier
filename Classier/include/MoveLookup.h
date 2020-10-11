#pragma once

#include "Move.h"

struct MoveLookup
{
public:
	bool moveTable[64][64];

	void add(Move);
	bool contains(Move) const;
	void reset();
};