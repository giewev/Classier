#include <string>
#include <vector>

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

    static void generateMoves(std::vector<Move>&, int x, int y, const Board&);

    static void appendMoveArray(Move*, int&, int x, int y, const Board&);
    static void kingMoveArray(Move*, int&, int x, int y, const Board&);
    static void queenMoveArray(Move*, int&, int x, int y, const Board&);
    static void pawnMoveArray(Move*, int&, int x, int y, const Board&);
    static void bishopMoveArray(Move*, int&, int x, int y, const Board&);
    static void knightMoveArray(Move*, int&, int x, int y, const Board&);
    static void rookMoveArray(Move*, int&, int x, int y, const Board&);

    bool isSafe(const Board&);
    bool operator!=(Piece);

    virtual bool hasMoved();
    virtual void setMoved(bool);
    virtual const bool isNull();
};

