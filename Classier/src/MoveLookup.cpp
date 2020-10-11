#include "MoveLookup.h"

void MoveLookup::add(Move newMove)
{
	moveTable[newMove.startIndex][newMove.endIndex] = true;
}

bool MoveLookup::contains(Move checkMove) const
{
	return moveTable[checkMove.startIndex][checkMove.endIndex];
}

void MoveLookup::reset()
{
	for (int i = 0; i < 64; i++)
	{
		for (int j = 0; j < 64; j++)
		{
			moveTable[i][j] = false;
		}
	}
}