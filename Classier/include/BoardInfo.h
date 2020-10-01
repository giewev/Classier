#pragma once

#include "Bitwise.h"
#include "ZobristHasher.h"

struct BoardInfo
{
	bool turn;
	bitBoard allPieces;
	bitBoard pieces[7];
	char castlingRights;
	int EPdata;
	ZobristHasher hasher;

	size_t getHashCode() const;
	bool operator==(const BoardInfo& other) const;
};

namespace std
{
	template <> struct hash<BoardInfo>
	{
		size_t operator()(const BoardInfo& board) const
		{
			return board.getHashCode();
		}
	};
}