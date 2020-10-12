#include <random>
#include "ZobristHasher.h"
#include "Board.h"
#include "Move.h"
#include "Bitwise.h"
#include "Piece.h"

std::random_device rd;
std::mt19937_64 ZobristHasher::hashGenerator(rd());
long long ZobristHasher::whitePieceHashCodes[64][6];
long long ZobristHasher::blackPieceHashCodes[64][6];
long long ZobristHasher::castlingHashCodes[4];
long long ZobristHasher::enPassantHashCodes[64];
long long ZobristHasher::turnHashCode;
bool ZobristHasher::alreadySeeded;

ZobristHasher::ZobristHasher()
{
    this->hashValue = 0;
    if (!alreadySeeded)
    {
        seed();
        alreadySeeded = true;
    }
}

ZobristHasher::ZobristHasher(const Board& board) : ZobristHasher()
{
    this->load(board);
}

ZobristHasher::ZobristHasher(const ZobristHasher& original) : ZobristHasher()
{
    this->hashValue = original.hashValue;
}

void ZobristHasher::seed()
{
    for (int i = 0; i < 64; i++)
    {
        enPassantHashCodes[i] = generateHashCode();
        for (int k = 0; k < 6; k++)
        {
            whitePieceHashCodes[i][k] = generateHashCode();
            blackPieceHashCodes[i][k] = generateHashCode();
        }
    }

    for (int i = 0; i < 4; i++)
    {
        castlingHashCodes[i] = generateHashCode();
    }

    turnHashCode = generateHashCode();
}

void ZobristHasher::load(const Board& board)
{
    this->hashValue = 0ll;
    this->loadPieces(board);
    this->loadCastlingRights(board);
    this->loadEnPassant(board);
    if (board.facts.turn)
    {
        this->hashValue ^= this->turnHashCode;
    }
}

void ZobristHasher::loadPieces(const Board& board)
{
    for (int i = 0; i < 64; i ++)
    {
        PieceType pieceType = board.getSquareType(i);
        if (pieceType != PieceType::Empty)
        {
            this->togglePiece(i, pieceType, board.getSquareColor(i));
        }
    }
}

void ZobristHasher::loadCastlingRights(const Board& board)
{
    bool boolValues[2] = { true, false };
    for (int i = 0; i < 2; i++)
    {
        for (int j = 0; j < 2; j++)
        {
            if (board.getCastlingRights(boolValues[i], boolValues[j]))
            {
                this->toggleCastlingRights(boolValues[i], boolValues[j]);
            }
        }
    }
}

void ZobristHasher::loadEnPassant(const Board& board)
{
	int ep = board.getEPData();
    if (ep != -1)
    {
        this->toggleEnPassant(ep);
    }
}

void ZobristHasher::togglePiece(int index, PieceType pieceType, bool color)
{
    if (color)
    {
        this->hashValue ^= whitePieceHashCodes[index][pieceType - 1];
    }
    else
    {
        this->hashValue ^= blackPieceHashCodes[index][pieceType - 1];
    }
}

void ZobristHasher::toggleTurn()
{
    this->hashValue ^= turnHashCode;
}

void ZobristHasher::toggleEnPassant(int file)
{
    this->hashValue ^= enPassantHashCodes[file];
}

void ZobristHasher::toggleCastlingRights(bool color, bool side)
{
    int castlingIndex = 0;
    if (color)
    {
        castlingIndex += 2;
    }

    if (side)
    {
        castlingIndex++;
    }

    this->hashValue ^= castlingHashCodes[castlingIndex];
}

void ZobristHasher::update(const Board& prevBoard, Move nextMove)
{
    this->toggleTurn();
    this->updateEnPassant(prevBoard, nextMove);
	if (nextMove.null) return;
    this->updateCastling(prevBoard, nextMove);
    this->updatePieces(prevBoard, nextMove);
}

void ZobristHasher::updateEnPassant(const Board& prevBoard, Move nextMove)
{
	int prevEp = prevBoard.getEPData();
    if (prevEp != -1)
    {
        this->toggleEnPassant(prevEp);
    }

	if (fabs(nextMove.endIndex - nextMove.startIndex) == 16)
	{
		this->toggleEnPassant(nextMove.endIndex);
	}
}

void ZobristHasher::updateCastling(const Board& prevBoard, Move nextMove)
{
	bool kingColor = prevBoard.facts.turn;
	if (prevBoard.getKingIndex(prevBoard.getKingIndex(kingColor)) == nextMove.startIndex)
    {
        if (prevBoard.getCastlingRights(kingColor, true))
        {
            this->toggleCastlingRights(kingColor, true);
        }

        if (prevBoard.getCastlingRights(kingColor, false))
        {
            this->toggleCastlingRights(kingColor, false);
        }
    }
    else if (nextMove.movingPiece == PieceType::Rook)
    {

		switch (nextMove.startIndex)
		{
		case(0):
			if (prevBoard.getCastlingRights(true, false)) this->toggleCastlingRights(true, false);
			break;
		case(7):
			if (prevBoard.getCastlingRights(true, true)) this->toggleCastlingRights(true, true);
			break;
		case(56):
			if (prevBoard.getCastlingRights(false, false)) this->toggleCastlingRights(false, false);
			break;
		case(63):
			if (prevBoard.getCastlingRights(false, true)) this->toggleCastlingRights(false, true);
		}

		switch (nextMove.endIndex)
		{
		case(0):
			if (prevBoard.getCastlingRights(true, true)) this->toggleCastlingRights(true, true);
			break;
		case(7):
			if (prevBoard.getCastlingRights(true, true)) this->toggleCastlingRights(true, true);
			break;
		case(56):
			if (prevBoard.getCastlingRights(false, false)) this->toggleCastlingRights(false, false);
			break;
		case(63):
			if (prevBoard.getCastlingRights(false, true)) this->toggleCastlingRights(false, true);
		}
    }
}

void ZobristHasher::updatePieces(const Board& prevBoard, Move nextMove)
{
	PieceType movingPieceType = nextMove.movingPiece;
	bool movingPieceColor = prevBoard.facts.turn;
    this->togglePiece(nextMove.startIndex, movingPieceType, movingPieceColor);
    this->togglePiece(nextMove.endIndex, movingPieceType, movingPieceColor);

    if (nextMove.pieceCaptured != PieceType::Empty)
    {
        this->togglePiece(nextMove.endIndex, nextMove.pieceCaptured, !movingPieceColor);
    }

    if (movingPieceType == PieceType::Pawn)
    {
		int epIndex = prevBoard.getEPData();
		if (epIndex != -1)
		{
			if ((movingPieceColor && nextMove.endIndex == epIndex + 8)
				|| (!movingPieceColor && nextMove.endIndex == epIndex - 8))
			{
				this->togglePiece(epIndex, PieceType::Pawn, !movingPieceColor);
			}
		}

        if (nextMove.promotion != PieceType::Empty)
        {
            this->togglePiece(nextMove.endIndex, movingPieceType, movingPieceColor);
            this->togglePiece(nextMove.endIndex, nextMove.promotion, movingPieceColor);
        }
    }

    if (movingPieceType == PieceType::King)
    {
		if (fabs(nextMove.startIndex - nextMove.endIndex) == 2)
		{
			// Castling to the right
			if (nextMove.startIndex > nextMove.endIndex)
			{
				this->togglePiece(nextMove.endIndex - 1, PieceType::Rook, movingPieceColor);
				this->togglePiece(nextMove.endIndex + 1, PieceType::Empty, false);
			}
			else // Castling to the left
			{
				this->togglePiece(nextMove.endIndex + 1, PieceType::Rook, movingPieceColor);
				this->togglePiece(nextMove.endIndex - 2, PieceType::Empty, false);
			}
		}
    }
}

long long ZobristHasher::generateHashCode()
{
    std::uniform_int_distribution<long long> distribution;
    return distribution(ZobristHasher::hashGenerator);
}
