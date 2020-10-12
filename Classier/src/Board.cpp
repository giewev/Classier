#include <vector>
#include <iostream>
#include <inttypes.h>
#include <math.h>
#include <assert.h>

#include "Piece.h"
#include "Board.h"
#include "Move.h"
#include "Engine.h"
#include "Bitwise.h"
#include "ZobristHasher.h"
#include "Evaluation Constants.h"

using namespace std;

#define bitBoard uint64_t

Board::Board()
{
    facts.turn = true;
    facts.castlingRights = 15;
    facts.EPdata = -1;
    facts.allPieces = 0ull;
    for(int i=0; i<7; i++)
    {
        facts.pieces[i] = 0ull;
    }

	halfMoveCounter = 0;
	pawnPositionalValue = 0;
	knightPositionalValue = 0;
	bishopPositionalValue = 0;

	hashingEnabled = true;
	positionalScoresEnabled = true;
}

Board::Board(int null)
{
}

Board::~Board()
{
}

void Board::loadFEN(std::string fenFile)
{
    unsigned int bookmark = 0;

    //Blanking out the Board
	pawnPositionalValue = 0;
	knightPositionalValue = 0;
	bishopPositionalValue = 0;
    for(int i=0; i<7; i++)
    {
        facts.pieces[i] = 0ull;
    }

    int x = 0;
    int y = 7;

    for(unsigned int i=0; i<fenFile.length(); i++)
    {
		int index = bitwise::coordToIndex(x, y);

        //checking for every value that represents a piece
        if     (fenFile[i] == 'r')
        {
            setSquare(PieceType::Rook, false, index);
        }
        else if(fenFile[i] == 'R')
        {
			setSquare(PieceType::Rook, true, index);
        }
        else if(fenFile[i] == 'n')
        {
			setSquare(PieceType::Knight, false, index);
        }
        else if(fenFile[i] == 'N')
        {
			setSquare(PieceType::Knight, true, index);
        }
        else if(fenFile[i] == 'b')
        {
			setSquare(PieceType::Bishop, false, index);
        }
        else if(fenFile[i] == 'B')
        {
			setSquare(PieceType::Bishop, true, index);
        }
        else if(fenFile[i] == 'q')
        {
			setSquare(PieceType::Queen, false, index);
        }
        else if(fenFile[i] == 'Q')
        {
			setSquare(PieceType::Queen, true, index);
        }
        else if(fenFile[i] == 'k')
        {
			setKingLocation(false, index);
			setSquare(PieceType::King, false, index);
        }
        else if(fenFile[i] == 'K')
        {
			setKingLocation(true, index);
			setSquare(PieceType::King, true, index);
        }
        else if(fenFile[i] == 'p')
        {
			setSquare(PieceType::Pawn, false, index);
        }
        else if(fenFile[i] == 'P')
        {
			setSquare(PieceType::Pawn, true, index);
        }

        //Checking if the value was a number
        if(fenFile[i] != '/')
        {
            if(fenFile[i] >= 49 && fenFile[i] <= 56)
            {
                //If it is jump ahead some squares
                x += fenFile[i] - 48;
            }
            else
            {
                //If it isnt we just go to the next one
                x += 1;
            }
        }

        //next Rank
        if(x >= 8)
        {
            x = 0;
            y -= 1;
        }

        //Done setting facts.pieces up
        if(y == -1)
        {
            bookmark = i+1;
            break;
        }
    }

    //setting facts.turn
    for(; bookmark < fenFile.length(); bookmark++)
    {
        if(fenFile[bookmark] == 'w')
        {
            facts.turn = true;
            bookmark++;
            break;
        }
        else if(fenFile[bookmark] == 'b')
        {
            facts.turn = false;
            bookmark++;
            break;
        }
        else
        {
            continue;
        }
    }

    //Setting Castling Rights
    setCastlingRights(0);
    bool breakOut = false;
    bool startedCastling = false;
    while (bookmark < fenFile.length())
    {
        switch (fenFile[bookmark])
        {
        case('K') :
            setCastlingRights(true, true, true);
            startedCastling = true;
            breakOut = false;
            break;
        case('Q') :
            setCastlingRights(true, false, true);
            startedCastling = true;
            breakOut = false;
            break;
        case('k') :
            setCastlingRights(false, true, true);
            startedCastling = true;
            breakOut = false;
            break;
        case('q') :
            setCastlingRights(false, false, true);
            startedCastling = true;
            breakOut = false;
            break;
        case('-') :
            breakOut = true;
            startedCastling = true;
            break;
        default:
            breakOut = true;
        }
        bookmark++;
        if (breakOut && startedCastling)
        {
            break;
        }
    }

    //Setting EnPassent
    while (bookmark < fenFile.length())
    {
        if (fenFile[bookmark] == ' ')
        {
            bookmark++;
            continue;
        }
        if (fenFile[bookmark] == '-')
        {
            bookmark++;
            break;
        }
        else if (bookmark + 1 < fenFile.length())
        {
            int rank;
            if (fenFile[bookmark + 1] == '3')
            {
                rank = 3;
            }
            else
            {
                rank = 4;
            }
            setEP(bitwise::coordToIndex(fenFile[bookmark]-97, rank));
            break;
        }
        bookmark += 2;
    }

    //setting halfmove clock
    while (bookmark < fenFile.length())
    {
        if (fenFile[bookmark] >= 48 && fenFile[bookmark] <= 57)
        {
            if (bookmark + 1 < fenFile.length() &&
                    fenFile[bookmark + 1] >= 48 && fenFile[bookmark + 1] <= 57)
            {
                halfMoveCounter = 10 * (fenFile[bookmark] - 48) + fenFile[bookmark + 1] - 48;
                bookmark += 2;
                break;
            }
            else
            {
                halfMoveCounter = fenFile[bookmark] - 48;
                bookmark++;
                break;
            }
        }
        else
        {
            bookmark++;
        }
    }

    //setting fullMove clock
    while (bookmark < fenFile.length())
    {
        if (fenFile[bookmark] >= 48 && fenFile[bookmark] <= 57)
        {
            if (bookmark + 1 < fenFile.length() &&
                    fenFile[bookmark + 1] >= 48 && fenFile[bookmark + 1] <= 57)
            {
                moveCounter = 10 * (fenFile[bookmark] - 48) + fenFile[bookmark + 1] - 48;
                bookmark += 2;
                break;
            }
            else
            {
                moveCounter = fenFile[bookmark] - 48;
                bookmark++;
                break;
            }
        }
        else
        {
            bookmark++;
        }
    }

    facts.hasher = ZobristHasher(*this);
}

string Board::outputFEN() const
{
    string FEN = "";
    int count = 0;
    for(int y = 7; y >= 0; y--)
    {
        for(int x = 0; x < 8; x++)
        {
			int index = bitwise::coordToIndex(x, y);

            PieceType pieceType = getSquareType(index);
            bool pieceColor = getSquareColor(index);
            if(pieceColor)
            {
                if(pieceType == PieceType::Empty)
                {
                    count += 1;
                }
                else
                {
                    if (count)
                    {
                        FEN += char(48 + count);
                        count = 0;
                    }

                    if (pieceType == PieceType::Pawn)
                    {
                        FEN += "P";
                    }
                    else if (pieceType == PieceType::Rook)
                    {
                        FEN += "R";
                    }
                    else if (pieceType == PieceType::Knight)
                    {
                        FEN += "N";
                    }
                    else if (pieceType == PieceType::Bishop)
                    {
                        FEN += "B";
                    }
                    else if (pieceType == PieceType::Queen)
                    {
                        FEN += "Q";
                    }
                    else if (pieceType == PieceType::King)
                    {
                        FEN += "K";
                    }
                    else
                    {
                        count = 1;
                    }
                }
            }
            else
            {
                if(pieceType == PieceType::Empty)
                {
                    count += 1;
                }
                else
                {
                    if (count)
                    {
                        FEN += char(48 + count);
                        count = 0;
                    }

                    if (pieceType == PieceType::Pawn)
                    {
                        FEN += "p";
                    }
                    else if (pieceType == PieceType::Rook)
                    {
                        FEN += "r";
                    }
                    else if (pieceType == PieceType::Knight)
                    {
                        FEN += "n";
                    }
                    else if (pieceType == PieceType::Bishop)
                    {
                        FEN += "b";
                    }
                    else if (pieceType == PieceType::Queen)
                    {
                        FEN += "q";
                    }
                    else if (pieceType == PieceType::King)
                    {
                        FEN += "k";
                    }
                    else
                    {
                        count = 1;
                    }
                }
            }
        }
        if(count)
        {
            FEN += char(48 + count);
            count = 0;
        }

		if (y != 0) FEN += "/";
    }
    if(facts.turn)
    {
        FEN += " w";
    }
    else
    {
        FEN += " b";
    }
    return FEN;
}

//Creates a new copy of the current Board and all of its contents
Board Board::newCopy() const
{
    Board newBoard = Board(0);
    for(int i=0; i<7; i++)
    {
        newBoard.facts.pieces[i] = facts.pieces[i];
    }

    newBoard.facts.allPieces = facts.allPieces;
    newBoard.facts.EPdata = facts.EPdata;

    newBoard.setCastlingRights(true,  true,  getCastlingRights(true, true));
    newBoard.setCastlingRights(true,  false, getCastlingRights(true, false));
    newBoard.setCastlingRights(false, true,  getCastlingRights(false, true));
    newBoard.setCastlingRights(false, false, getCastlingRights(false, false));
    newBoard.facts.turn = facts.turn;
	newBoard.whiteKingIndex = whiteKingIndex;
	newBoard.blackKingIndex = blackKingIndex;
    newBoard.moveCounter = moveCounter;
    newBoard.halfMoveCounter = halfMoveCounter;
    newBoard.facts.hasher = ZobristHasher(facts.hasher);

    return newBoard;
}

PieceType Board::getSquareType(const int& index) const
{
    if (!squareIsPopulated(index))
    {
        return PieceType::Empty;
    }

    for(int i=1; i<7; i++)
    {
        if((facts.pieces[i] >> index) & 1)
        {
            return (PieceType)i;
        }
    }

    return PieceType::Empty;
}

bool Board::getSquareColor(int index) const
{
    return (facts.pieces[0] >> index) & 1;
}

int Board::getKingIndex(bool getColor) const
{
    if(getColor)
    {
        return whiteKingIndex;
    }
    else
    {
        return blackKingIndex;
    }
}

void Board::setSquare(PieceType type, PieceType prevType, bool color, int index)
{
	updatePositionalScore(type, color, prevType, getSquareColor(index), index);
	bitBoard square = bitwise::indexToBoard(index);
	bitBoard notSquare = ~square;

	if (prevType != PieceType::Empty)
	{
		facts.pieces[prevType] &= notSquare;
	}

	if (type > 0)
	{
		facts.pieces[type] |= square;
		facts.allPieces |= square;
	}
	else
	{
		facts.allPieces &= notSquare;
	}

	if (color)
	{
		facts.pieces[0] |= square;
	}
	else
	{
		facts.pieces[0] &= notSquare;
	}
}

void Board::setSquare(PieceType type, bool color, int index)
{
	updatePositionalScore(type, color, index);
	bitBoard square = bitwise::indexToBoard(index);
	bitBoard notSquare = ~square;
	for (int i = 1; i < 7; i++)
	{
		facts.pieces[i] &= notSquare;
	}
	if (type > 0)
	{
		facts.pieces[type] |= square;
		facts.allPieces |= square;
	}
	else
	{
		facts.allPieces &= notSquare;
	}

	if (color)
	{
		facts.pieces[0] |= square;
	}
	else
	{
		facts.pieces[0] &= notSquare;
	}
}

void Board::updatePositionalScore(PieceType type, bool color, int index)
{
	PieceType deadPieceType = getSquareType(index);
	bool deadPieceColor = getSquareColor(index);
	updatePositionalScore(type, color, deadPieceType, deadPieceColor, index);
}

void Board::updatePositionalScore(PieceType type, bool color, PieceType deadPieceType, bool deadPieceColor, int index)
{
	if (!positionalScoresEnabled) return;

	switch (deadPieceType)
	{
		case(PieceType::Pawn):
			if (deadPieceColor) pawnPositionalValue -= whitePawnPositionValue[index];
			else pawnPositionalValue += blackPawnPositionValue[index];
			break;
		case(PieceType::Knight):
			if (deadPieceColor) knightPositionalValue -= whiteKnightPositionValue[index];
			else knightPositionalValue += blackKnightPositionValue[index];
			break;
		case(PieceType::Bishop):
			if (deadPieceColor) bishopPositionalValue -= whiteBishopPositionValue[index];
			else bishopPositionalValue += blackBishopPositionValue[index];
			break;
	}

	switch (type)
	{
		case(PieceType::Pawn):
			if (color) pawnPositionalValue += whitePawnPositionValue[index];
			else pawnPositionalValue -= blackPawnPositionValue[index];
			break;
		case(PieceType::Knight):
			if (color) knightPositionalValue += whiteKnightPositionValue[index];
			else knightPositionalValue -= blackKnightPositionValue[index];
			break;
		case(PieceType::Bishop):
			if (color) bishopPositionalValue += whiteBishopPositionValue[index];
			else bishopPositionalValue -= blackBishopPositionValue[index];
			break;
	}
}

void Board::setKingLocation(bool setColor, int index)
{
	if (setColor)
	{
		whiteKingIndex = index;
	}
	else 
	{
		blackKingIndex = index;
	}
}

void Board::generateMoveArray(Move* finalMoveList, int& moveCounter)
{
	unsigned long scanIndex;
	bitBoard ownColorMap;
	if (facts.turn) ownColorMap = facts.pieces[0];
	else ownColorMap = ~facts.pieces[0] & facts.allPieces;
	for (char pType = 1; pType < 7; pType++)
	{
		bitBoard moveables = facts.pieces[pType] & ownColorMap;
		while (_BitScanForward64(&scanIndex, moveables))
		{
			Piece::appendMoveArray(finalMoveList, moveCounter, (PieceType)pType, scanIndex, *this, false);
			moveables &= moveables - 1;
		}
	}

	hashingEnabled = false;
	positionalScoresEnabled = false;
    for(int i = moveCounter - 1; i >= 0; i--)
    {
		if(!finalMoveList[i].isSafe(*this))
		{
			finalMoveList[i] = finalMoveList[moveCounter - 1];
			moveCounter--;
		}
    }
	hashingEnabled = true;
	positionalScoresEnabled = true;
}

void Board::generateCaptureMoves(Move* moveList, int& moveCounter)
{
	unsigned long scanIndex;
	bitBoard ownColorMap;
	if (facts.turn) ownColorMap = facts.pieces[0];
	else ownColorMap = ~facts.pieces[0] & facts.allPieces;
	for (char pType = 1; pType < 7; pType++)
	{
		bitBoard moveables = facts.pieces[pType] & ownColorMap;
		while (_BitScanForward64(&scanIndex, moveables))
		{
			Piece::appendMoveArray(moveList, moveCounter, (PieceType)pType, scanIndex, *this, true);
			moveables &= moveables - 1;
		}
	}

	hashingEnabled = false;
	positionalScoresEnabled = false;
	for (int i = moveCounter - 1; i >= 0; i--)
	{
		if (!moveList[i].isSafe(*this))
		{
			moveList[i] = moveList[moveCounter - 1];
			moveCounter--;
		}
	}
	hashingEnabled = true;
	positionalScoresEnabled = true;
}

void Board::unmakeMove(Move data)
{
	if (data.null) {
		facts.turn = !facts.turn;
		facts.EPdata = data.oldEPData;
		facts.castlingRights = data.oldCastlingRights;

		// Zobrist unmakeUpdate
		facts.hasher.update(*this, data);
		return;
	}

	// Move the piece back
	PieceType movedType = data.movingPiece;
	bool movedColor = !facts.turn;
	setSquare(movedType, PieceType::Empty, movedColor, data.startIndex);
	// Replace any captured piece (Except EP capture)
	if (data.promotion == PieceType::Empty)
	{
		setSquare(data.pieceCaptured, movedType, !movedColor, data.endIndex);
	}
	else {
		setSquare(data.pieceCaptured, data.promotion, !movedColor, data.endIndex);
	}

	// Replace EP data 
	// Need to do this before checking if it was an EP capture
	facts.EPdata = data.oldEPData;

	// Special undo EP cature
	if (getEPData() > -1)
	{
		setSquare(PieceType::Pawn, !movedColor, getEPData());
	}

	// Special undo promotion logic
	if (data.promotion != PieceType::Empty)
	{
		setSquare(PieceType::Pawn, movedColor, data.startIndex);
	}

	// Special undo castle logic
	if (movedType == PieceType::King && fabs(data.startIndex - data.endIndex) == 2)
	{
		int rookEndIndex = (data.startIndex + data.endIndex) / 2;
		setSquare(PieceType::Empty, false, rookEndIndex);
		if (data.endIndex % 8 == 2)
		{
			setSquare(PieceType::Rook, movedColor, data.endIndex - 2);
		}
		else 
		{
			setSquare(PieceType::Rook, movedColor, data.endIndex + 1);
		}
	}

	// Replace king location
	if (movedType == PieceType::King)
	{
		setKingLocation(movedColor, data.startIndex);
	}

	facts.castlingRights = data.oldCastlingRights;
	facts.turn = !facts.turn;

	// Zobrist unmakeUpdate
	if (hashingEnabled)
	{
		facts.hasher.update(*this, data);
	}
}

void Board::makeMove(Move data)
{
	// A null move only passes the facts.turn
	if (data.null) {
		facts.turn = !facts.turn;
		clearEP();
		facts.hasher.update(*this, data);
		return;
	}

	if (hashingEnabled)
	{
		facts.hasher.update(*this, data);
	}

    //Picking up the Piece
	PieceType movingType = data.movingPiece;
	PieceType newType = movingType;
	bool movingColor = facts.turn;

    if(movingType == PieceType::Pawn)
    {
        //Check for double move
        if(fabs(data.endIndex - data.startIndex) == 16)
        {
            setEP(data.endIndex);
        }
        //Check for enPassent capture
        else
        {
			int epIndex = getEPData();
			if (epIndex != -1)
			{
				if ((movingColor && data.endIndex == epIndex + 8)
					|| (!movingColor && data.endIndex == epIndex - 8))
				{
					setSquare(PieceType::Empty, PieceType::Pawn, false, epIndex);
				}
			}
			
			clearEP();
        }

        if(data.promotion != PieceType::Empty)
        {
			newType = data.promotion;
        }
    }
    else if(facts.EPdata != -1)
    {
		clearEP();
    }

    if(movingType == PieceType::King)
    {
        //Move the rook if castling
        if(fabs(data.startIndex - data.endIndex) == 2)
        {
			// Castling to the right
			if (data.startIndex < data.endIndex)
			{
				setSquare(PieceType::Rook, PieceType::Empty, movingColor, data.endIndex - 1);
				setSquare(PieceType::Empty, PieceType::Rook, false, data.endIndex + 1);
			}
			else // Castling to the left
			{
				setSquare(PieceType::Rook, PieceType::Empty, movingColor, data.endIndex + 1);
				setSquare(PieceType::Empty, PieceType::Rook, false, data.endIndex -2);
			}
        }

        setKingLocation(movingColor, data.endIndex);
		setCastlingRights(movingColor, true, false);
		setCastlingRights(movingColor, false, false);
    }

	switch (data.startIndex)
	{
	case(0):
		setCastlingRights(true, false, false);
		break;
	case(7):
		setCastlingRights(true, true, false);
		break;
	case(56):
		setCastlingRights(false, false, false);
		break;
	case(63):
		setCastlingRights(false, true, false);
	}

	switch (data.endIndex)
	{
	case(0):
		setCastlingRights(true, false, false);
		break;
	case(7):
		setCastlingRights(true, true, false);
		break;
	case(56):
		setCastlingRights(false, false, false);
		break;
	case(63):
		setCastlingRights(false, true, false);
	}

    setSquare(PieceType::Empty, movingType, false, data.startIndex);
    setSquare(newType, data.pieceCaptured, movingColor, data.endIndex);

    facts.turn = !facts.turn;
}


//Returns false if the Piece is in check, true otherwise
bool Board::squareAttacked(int index, bool byColor)
{
	bool color = !byColor;

	//Check for Bishop or Queen
	bitBoard bishopMap = Piece::bishopMoveBoard(index, color, *this, false);
	bishopMap &= facts.pieces[PieceType::Bishop] | facts.pieces[PieceType::Queen];
	if (bishopMap)
	{
		return true;
	}

	//Check for Rook or Queen
	bitBoard rookMap = Piece::rookMoveBoard(index, color, *this, false);
	rookMap &= facts.pieces[PieceType::Rook] | facts.pieces[PieceType::Queen];
	if (rookMap)
	{
		return true;
	}

	//Check for Knight
	bitBoard knightMap = Piece::knightMoveBoard(index, color, *this, false);
	knightMap &= facts.pieces[PieceType::Knight];
	if (knightMap)
	{
		return true;
	}

	//Check if next to a king
	bitBoard kingMap = Piece::kingRegularMoveBoard(index, color, *this, false);
	kingMap &= facts.pieces[PieceType::King];
	if (kingMap)
	{
		return true;
	}

	bitBoard pawnMap = pawnCaptureMoves[color][index];
	pawnMap &= facts.pieces[PieceType::Pawn];
	if (byColor)
	{
		pawnMap &= facts.pieces[0];
	}
	else 
	{
		pawnMap &= ~facts.pieces[0];
	}

	return pawnMap;
}



//Depth:    Expect:
// 0        1
// 1        20
// 2        400
// 3        8902
// 4        197281
// 5        4865609
// 6        119060324
double Board::perft(int depth)
{
    if(depth == 0) return(1);//No moves at 0 depth
    int moveGenCount = 0;
    Move moveList[220];
    generateMoveArray(moveList, moveGenCount);

	/*for (int i = 0; i < moveGenCount; i++)
	{
		std::cout << moveList[i].basicAlg() << std::endl;
	}*/

    if(depth == 1)
    {
        return(moveGenCount);//How many moves can we make RIGHT NOW
    }

    double moveCounter = 0;

    for(int i = 0; i < moveGenCount; i++)
    {
        makeMove(moveList[i]);
        moveCounter += perft(depth-1);
		unmakeMove(moveList[i]);
    }

    return moveCounter;
}

double Board::dividePerft(int depth)
{
    if(depth == 0) return(1);//No moves at 0 depth

    int moveGenCount = 0;
    Move moveList[220];
    generateMoveArray(moveList, moveGenCount);
    if(depth == 1)
    {
        for(int i=0; i<moveGenCount; i++)
        {
            std::cout << moveList[i].basicAlg() << " " << std::endl;
        }
        return(moveGenCount);//How many moves can we make RIGHT NOW
    }

    double moveCounter = 0;
    double newBoardMoveCount;

    for(int i=0; i < moveGenCount; i++)
    {
		makeMove(moveList[i]);
		newBoardMoveCount = perft(depth - 1);
        std::cout << moveList[i].basicAlg() << " ";
        std::cout << std::fixed << (unsigned long)newBoardMoveCount << std::endl;
        moveCounter += newBoardMoveCount;
		unmakeMove(moveList[i]);
    }
    return moveCounter;
}

bool Board::getCastlingRights(bool color, bool side) const
{
    char placer = 0;
    if(color)
    {
        placer=2;
    }
    if(side)
    {
        placer+=1;
    }
    return((facts.castlingRights >> placer) & 1);
}

char Board::getCastlingRights() const
{
    return facts.castlingRights;
}

void Board::setEP(int index)
{
	facts.EPdata = index;
}

void Board::clearEP()
{
	facts.EPdata = -1;
}

int Board::getEPData() const
{
	return facts.EPdata;
}

int Board::getCastlingData() const
{
	return facts.castlingRights;
}

void Board::setCastlingRights(bool color, bool side, bool value)
{
    //FALSE PieceType::QueenSIDE TRUE PieceType::KingSIDE
    char placer = 0;
    if(color)
    {
        placer=2;
    }
    if(side)
    {
        placer+=1;
    }

    if(value)
    {
        //sets the corresponding bit
        facts.castlingRights = facts.castlingRights | (1 << placer);
    }
    else
    {
        //Deletes the bit at the corresponding spot
        facts.castlingRights = facts.castlingRights & ~(1 << placer);
    }
}

void Board::setCastlingRights(char rights)
{
    facts.castlingRights = rights;
}

void Board::countPieces() const
{
    std::cout << "Black:  " << pieceCount(false) << std::endl;
    std::cout << "	Pawns:  " << pieceCount(PieceType::Pawn, false) << std::endl;
    std::cout << "	Queens:  " << pieceCount(PieceType::Queen, false) << std::endl;
    std::cout << "	Kings:  " << pieceCount(PieceType::King, false) << std::endl;
    std::cout << "	Bishops  " << pieceCount(PieceType::Bishop, false) << std::endl;
    std::cout << "	Knights  " << pieceCount(PieceType::Knight, false) << std::endl;
    std::cout << "	Rooks  " << pieceCount(PieceType::Rook, false) << std::endl;

    std::cout << "White:  " << pieceCount(true) << std::endl;
    std::cout << "	Pawns:  " << pieceCount(PieceType::Pawn, true) << std::endl;
    std::cout << "	Queens:  " << pieceCount(PieceType::Queen, true) << std::endl;
    std::cout << "	Kings:  " << pieceCount(PieceType::King, true) << std::endl;
    std::cout << "	Bishops  " << pieceCount(PieceType::Bishop, true) << std::endl;
    std::cout << "	Knights  " << pieceCount(PieceType::Knight, true) << std::endl;
    std::cout << "	Rooks  " << pieceCount(PieceType::Rook, true) << std::endl;
}

int Board::pieceCount() const
{
    return bitwise::countBits(facts.allPieces);
}

int Board::pieceCount(bool color) const
{
    bitBoard mask = facts.pieces[0];
    if (!color)
    {
        mask = ~mask;
    }

    return bitwise::countBits(facts.allPieces & mask);
}

int Board::pieceCount(PieceType type) const
{
    return bitwise::countBits(facts.pieces[type]);
}

int Board::pieceCount(PieceType type, bool color) const
{
    bitBoard mask = facts.pieces[0];
    if (!color)
    {
        mask = ~mask;
    }

    return bitwise::countBits(facts.pieces[type] & mask);
}

bool Board::squareIsPopulated(const int& index) const
{
    return (facts.allPieces >> index) & 1;
}

bool Board::squareIsType(int index, int type) const
{
    return (facts.pieces[type] >> index) & 1;
}

bool Board::operator==(const Board &other) const
{
	return facts == other.facts;
}