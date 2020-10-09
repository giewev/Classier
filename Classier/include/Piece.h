#include <string>
#include <vector>
#include "Bitwise.h"

struct Move;
class Board;
enum PieceType : char;

class Piece
{

public:
    bool moved;
    PieceType type;
    int xPos, yPos;//coordinates
    bool color;//True is White, False is Black

    Piece();//Simple constructor
    Piece(bool);//Also sets color
    Piece(int, int, bool);//sets coordinates and Color
    Piece(PieceType);
    Piece(PieceType, int, int, bool);

    virtual ~Piece();

    int getX()
    {
        return(xPos);
    }
    int getY()
    {
        return(yPos);
    }
    bool getColor()
    {
        return(color);
    }

	static void appendMoveArray(Move*, int&, PieceType, int x, int y, const Board&, bool captureOnly);
    static void kingMoveArray(Move*, int&, int x, int y, const Board&, bool captureOnly);
    static void queenMoveArray(Move*, int&, int x, int y, const Board&, bool captureOnly);
    static void pawnMoveArray(Move*, int&, int x, int y, const Board&, bool captureOnly);
    static void bishopMoveArray(Move*, int&, int x, int y, const Board&, bool captureOnly);
    static void knightMoveArray(Move*, int&, int x, int y, const Board&, bool captureOnly);
    static void rookMoveArray(Move*, int&, int x, int y, const Board&, bool captureOnly);

	static bitBoard kingRegularMoveBoard(int x, int y, bool color, const Board& gameBoard, bool captureOnly);
	static bitBoard queenMoveBoard(int x, int y, bool color, const Board& gameBoard, bool captureOnly);
	static bitBoard bishopMoveBoard(int x, int y, bool color, const Board& gameBoard, bool captureOnly);
	static bitBoard knightMoveBoard(int x, int y, bool color, const Board& gameBoard, bool captureOnly);
	static bitBoard rookMoveBoard(int x, int y, bool color, const Board& gameBoard, bool captureOnly);

    bool isSafe(const Board&);
    bool operator!=(Piece);

    virtual bool hasMoved();
    virtual void setMoved(bool);
    virtual const bool isNull();
};

