#include "MoveLookup.h"

void MoveLookup::add(Move newMove)
{
	moveTable[newMove.startX][newMove.startY][newMove.endX][newMove.endY] = true;
}

bool MoveLookup::contains(Move checkMove) const
{
	return moveTable[checkMove.startX][checkMove.startY][checkMove.endX][checkMove.endY];
}

void MoveLookup::reset()
{
	for (int i = 0; i < 8; i++)
	{
		for (int j = 0; j < 8; j++)
		{
			for (int k = 0; k < 8; k++)
			{
				for (int l = 0; l < 8; l++)
				{
					moveTable[i][j][k][l] = false;
				}
			}
		}
	}
}