#include "BoardInfo.h"

size_t BoardInfo::getHashCode() const
{
	return hasher.hashValue;
}

bool BoardInfo::operator==(const BoardInfo& other) const
{
	for (int i = 1; i < 7; i++)
	{
		if (this->pieces[i] != other.pieces[i])
		{
			return false;
		}
	}

	if ((this->pieces[0] & this->allPieces) != (other.pieces[0] & other.allPieces))
	{
		return false;
	}

	if ((~this->pieces[0] & this->allPieces) != (~other.pieces[0] & other.allPieces))
	{
		return false;
	}

	return this->turn == other.turn &&
		this->EPdata == other.EPdata &&
		this->castlingRights == other.castlingRights;
}