#ifndef ZOBRISTHASHER_H
#define ZOBRISTHASHER_H
#include <random>
class Board;
class Move;
enum PieceType : char;

class ZobristHasher
{
public:
    long long hashValue;

    ZobristHasher();
    ZobristHasher(const Board& board);
    ZobristHasher(const ZobristHasher &original);
    void load(const Board&);
    void update(const Board&, Move);

    template <class Archive>
    void serialize(Archive& ar)
    {
        ar(hashValue, whitePieceHashCodes, blackPieceHashCodes, castlingHashCodes, enPassantHashCodes, turnHashCode);
    }

protected:
private:
    static std::mt19937_64 hashGenerator;
    static long long generateHashCode();
    static bool alreadySeeded;
    static void seed();

    static long long whitePieceHashCodes[8][8][6];
    static long long blackPieceHashCodes[8][8][6];
    static long long castlingHashCodes[4];
    static long long enPassantHashCodes[8];
    static long long turnHashCode;

    void loadPieces(const Board& board);
    void loadEnPassant(const Board& board);
    void loadCastlingRights(const Board& board);

    void updateEnPassant(const Board& prevBoard, Move nextMove);
    void updateCastling(const Board& prevBoard, Move nextMove);
    void updatePieces(const Board& prevBoard, Move nextMove);

    void togglePiece(int x, int y, PieceType pieceType, bool color);
    void toggleTurn();
    void toggleEnPassant(int file);
    void toggleCastlingRights(bool color, bool side);
};

#endif // ZOBRISTHASHER_H
