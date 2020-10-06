#include <string>
#include <assert.h>
#include "Board.h"
#include "Piece.h"
#include "Engine.h"
#include "Move.h"
#include "Bitwise.h"
#include "Danger.h"
#include "ZobristHasher.h"

void assertAlmost(double expected, double actual)
{
	assert(fabs(expected - actual) < 1e-6);
}

void knightMoveMaps()
{
	assert(bitwise::genKnightMovement(7, 7) == 0x20400000000000);
	assert(bitwise::genKnightMovement(0, 0) == 0x20400);
	assert(bitwise::genKnightMovement(7, 0) == 0x402000);
	assert(bitwise::genKnightMovement(0, 7) == 0x4020000000000);
}

void pawnMoveMaps()
{
	assert(bitwise::genPawnCaptureMovement(true, 0, 1) == 0x20000);
	assert(bitwise::genPawnCaptureMovement(true, 7, 1) == 0x400000);
	assert(bitwise::genPawnCaptureMovement(false, 0, 6) == 0x20000000000);
	assert(bitwise::genPawnCaptureMovement(false, 7, 6) == 0x400000000000);

	assert(bitwise::genPawnCaptureMovement(true, 3, 1) == 0x140000);
	assert(bitwise::genPawnCaptureMovement(false, 3, 6) == 0x140000000000);
}

void kingMoveMaps()
{
	assert(bitwise::genKingMovement(4, 0) == 0x3828);
	assert(bitwise::genKingMovement(4, 7) == 0x2838000000000000);
}

void notBehindMaps()
{
	assert(bitwise::genNotBehind(0, 8) == 0xfefefefefefeffff);
	assert(bitwise::genNotBehind(7, 6) == 0xffffffffffffffc0);
	assert(bitwise::genNotBehind(2, 11) == 0xffff7fbfdfefffff);
}

void startingPerft_test()
{
    std::string startingFEN = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w";
    Board testBoard = Board();
    testBoard.loadFEN(startingFEN);

    double expectedPerfts[10] = { 1, 20, 400, 8902, 197281, 4865609, 119060324, 3195901860 };

    for(int i = 0; i < 6; i++)
    {
		double result = testBoard.perft(i);
		std::cout << result << std::endl;
        assert(result == expectedPerfts[i]);
    }
}

void kiwipetePerft_test()
{
    std::string kiwipeteFEN = "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq -";
    Board testBoard = Board();
    testBoard.loadFEN(kiwipeteFEN);

    double expectedPerfts[10] = { 1, 48, 2039, 97862, 4085603, 193690690, 8031647685 };

    for(int i = 0; i < 5; i++)
    {
        assert(testBoard.perft(i) == expectedPerfts[i]);
    }
}

void endgamePerft_test()
{
    std::string endgameFEN = "8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - -";
    Board testBoard = Board();
    testBoard.loadFEN(endgameFEN);

    double expectedPerfts[10] = { 1, 14, 191, 2812, 43238, 674624, 11030083, 178633661};

    for(int i = 0; i < 5; i++)
    {
        assert(testBoard.perft(i) == expectedPerfts[i]);
    }
}

void makeUnmakeEPCapture_test()
{
	std::string almostKiwipeteFEN = "r3k2r/p1ppqpb1/bn2pnp1/3PN3/Pp2P3/2N2Q1p/1PPBBPPP/R3K2R b KQkq a3 0 1 ";
	Board testBoard;
	testBoard.loadFEN(almostKiwipeteFEN);

	Move epCapture = Move(1, 3, 0, 2, PieceType::Empty, testBoard);
	std::string beforeFEN = testBoard.outputFEN();
	testBoard.makeMove(epCapture);
	testBoard.unmakeMove(epCapture);
	std::string afterFEN = testBoard.outputFEN();
	assert(beforeFEN == afterFEN);
}

void loadStartingPosition_test()
{
    std::string startingFEN = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w";

    Board testBoard = Board();
    testBoard.loadFEN(startingFEN);
	assert(testBoard.facts.turn == true);

    assert(testBoard.getSquareType(0, 0) == PieceType::Rook);
    assert(testBoard.getSquareType(1, 0) == PieceType::Knight);
    assert(testBoard.getSquareType(2, 0) == PieceType::Bishop);
    assert(testBoard.getSquareType(3, 0) == PieceType::Queen);
    assert(testBoard.getSquareType(4, 0) == PieceType::King);
    assert(testBoard.getSquareType(5, 0) == PieceType::Bishop);
    assert(testBoard.getSquareType(6, 0) == PieceType::Knight);
    assert(testBoard.getSquareType(7, 0) == PieceType::Rook);

    assert(testBoard.getSquareType(0, 7) == PieceType::Rook);
    assert(testBoard.getSquareType(1, 7) == PieceType::Knight);
    assert(testBoard.getSquareType(2, 7) == PieceType::Bishop);
    assert(testBoard.getSquareType(3, 7) == PieceType::Queen);
    assert(testBoard.getSquareType(4, 7) == PieceType::King);
    assert(testBoard.getSquareType(5, 7) == PieceType::Bishop);
    assert(testBoard.getSquareType(6, 7) == PieceType::Knight);
    assert(testBoard.getSquareType(7, 7) == PieceType::Rook);

    for (int i = 0; i < 8; i++)
    {
        assert(testBoard.getSquareType(i, 1) == PieceType::Pawn);
        assert(testBoard.getSquareType(i, 6) == PieceType::Pawn);

        assert(testBoard.getSquareColor(i, 0) == true);
        assert(testBoard.getSquareColor(i, 1) == true);
        assert(testBoard.getSquareColor(i, 6) == false);
        assert(testBoard.getSquareColor(i, 7) == false);
    }
}

void loadingStartingPosAfterE4_test()
{
	std::string startingFEN = "rnbqkbnr/pppppppp/8/8/4p3/8/PPPP1PPP/RNBQKBNR b";

	Board testBoard = Board();
	testBoard.loadFEN(startingFEN);
	assert(testBoard.facts.turn == false);

	assert(testBoard.getSquareType(0, 0) == PieceType::Rook);
	assert(testBoard.getSquareType(1, 0) == PieceType::Knight);
	assert(testBoard.getSquareType(2, 0) == PieceType::Bishop);
	assert(testBoard.getSquareType(3, 0) == PieceType::Queen);
	assert(testBoard.getSquareType(4, 0) == PieceType::King);
	assert(testBoard.getSquareType(5, 0) == PieceType::Bishop);
	assert(testBoard.getSquareType(6, 0) == PieceType::Knight);
	assert(testBoard.getSquareType(7, 0) == PieceType::Rook);

	assert(testBoard.getSquareType(0, 7) == PieceType::Rook);
	assert(testBoard.getSquareType(1, 7) == PieceType::Knight);
	assert(testBoard.getSquareType(2, 7) == PieceType::Bishop);
	assert(testBoard.getSquareType(3, 7) == PieceType::Queen);
	assert(testBoard.getSquareType(4, 7) == PieceType::King);
	assert(testBoard.getSquareType(5, 7) == PieceType::Bishop);
	assert(testBoard.getSquareType(6, 7) == PieceType::Knight);
	assert(testBoard.getSquareType(7, 7) == PieceType::Rook);

	assert(testBoard.getSquareType(4, 1) == PieceType::Empty);
	assert(testBoard.getSquareType(4, 3) == PieceType::Pawn);


	for (int i = 0; i < 8; i++)
	{
		if (i == 4)
		{
			continue;
		}
		assert(testBoard.getSquareType(i, 1) == PieceType::Pawn);
		assert(testBoard.getSquareType(i, 6) == PieceType::Pawn);

		assert(testBoard.getSquareColor(i, 0) == true);
		assert(testBoard.getSquareColor(i, 1) == true);
		assert(testBoard.getSquareColor(i, 6) == false);
		assert(testBoard.getSquareColor(i, 7) == false);
	}
}

void noMovesInMate_test()
{
    std::string mateFEN = "4k3/4Q3/5K2/8/8/8/8/8 b - - 0 0";

    Board testBoard = Board();
    testBoard.loadFEN(mateFEN);

    int moveCount = 0;
    Move moveList[220];
    testBoard.generateMoveArray(moveList, moveCount);
    assert(moveCount == 0);
}

void basicMateInOnePuzzle_test_1()
{
    std::string matePuzzleFEN = "4k3/Q7/5K2/8/8/8/8/8 w - - 0 1";

    Board testBoard = Board();
    testBoard.loadFEN(matePuzzleFEN);
    Engine engine = Engine(testBoard);

    for (int depth = 2; depth < 5; depth++)
    {
        Move bestMove = engine.searchToDepth(depth);
        assert(bestMove.startX == 0);
        assert(bestMove.startY == 6);
        assert(bestMove.endX == 4);
        assert(bestMove.endY == 6);
    }
}

void basicMateInOnePuzzle_test_2()
{
    std::string matePuzzleFEN = "4k3/R7/5K2/8/8/8/8/7R w - - 0 1";

    Board testBoard = Board();
    testBoard.loadFEN(matePuzzleFEN);
    Engine engine = Engine(testBoard);

    for (int depth = 2; depth < 5; depth++)
    {
        Move bestMove = engine.searchToDepth(depth);
        assert(bestMove.startX == 7);
        assert(bestMove.startY == 0);
        assert(bestMove.endX == 7);
        assert(bestMove.endY == 7);
    }
}

void promotionMateInOnePuzzle_test_1()
{
    std::string promotionMatePuzzleFEN = "8/P7/1B6/8/8/6K1/8/7k w - - 0 1";

    Board testBoard = Board();
    testBoard.loadFEN(promotionMatePuzzleFEN);
    Engine engine = Engine(testBoard);

    for (int depth = 2; depth < 5; depth++)
    {
        Move bestMove = engine.searchToDepth(depth);

        assert(bestMove.startX == 0);
        assert(bestMove.startY == 6);
        assert(bestMove.endX == 0);
        assert(bestMove.endY == 7);
        assert(bestMove.promotion == PieceType::Queen || bestMove.promotion == PieceType::Bishop);
    }
}

void mateInThreePuzzle_test_1()
{
    std::string matePuzzleFEN = "1K3Q2/8/8/4k3/8/N7/N7/7B w - - 0 1";

    Board testBoard = Board();
    testBoard.loadFEN(matePuzzleFEN);
    Engine engine = Engine(testBoard);

    // Solution requires 3 white moves (5 ply)
    Move bestMove = engine.searchToDepth(6);
    assert(bestMove.startX == 0);
    assert(bestMove.startY == 1);
    assert(bestMove.endX == 2);
    assert(bestMove.endY == 0);
}

void avoidMatePuzzle_test_1()
{
    std::string avoidMatePuzzleFEN = "8/2k5/K7/8/1r6/8/8/8 w - - 0 1";

    Board testBoard = Board();
    testBoard.loadFEN(avoidMatePuzzleFEN);
    Engine engine = Engine(testBoard);

    for (int depth = 2; depth < 5; depth++)
    {
        Move bestMove = engine.searchToDepth(depth);
        assert(bestMove.startX == 0);
        assert(bestMove.startY == 5);
        assert(bestMove.endX == 0);
        assert(bestMove.endY == 4);
    }
}

void pawnPositionalScore_test_1()
{
	std::string startingFEN = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w";
	Board testBoard = Board();
	testBoard.loadFEN(startingFEN);
	
	Move firstMove = Move("e2e4", testBoard);
	Move secondMove = Move("e7e6", testBoard);

	assertAlmost(testBoard.pawnPositionalValue, 0);
	testBoard.makeMove(firstMove);
	testBoard.unmakeMove(firstMove);
	assertAlmost(testBoard.pawnPositionalValue, 0);
	testBoard.makeMove(firstMove);
	testBoard.makeMove(secondMove);
	testBoard.unmakeMove(secondMove);
	testBoard.unmakeMove(firstMove);
	assertAlmost(testBoard.pawnPositionalValue, 0);
}

void pawnPositionalScore_test_2()
{
	std::string startingFEN = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w";
	Board testBoard = Board();
	testBoard.loadFEN(startingFEN);

	for (int i = 0; i < 8; i++)
	{
		testBoard.makeMove(Move(i, 1, i, 3, PieceType::Empty, testBoard));
		testBoard.makeMove(Move(i, 6, i, 4, PieceType::Empty, testBoard));
	}

	assertAlmost(testBoard.pawnPositionalValue, 0);
}

void zobristConsistancy_test_helper(Board testBoard, Move testMove)
{
    ZobristHasher updateZobrist = ZobristHasher(testBoard);
    updateZobrist.update(testBoard, testMove);

    Board moveBoard = testBoard.newCopy();
    moveBoard.makeMove(testMove);

    ZobristHasher moveZobrist = ZobristHasher(updateZobrist);
    moveZobrist.load(moveBoard);
    assert(moveZobrist.hashValue == updateZobrist.hashValue);
}

void zobristConsistancy_test()
{
    std::string zobristTestFEN = "2n1k3/1P6/8/5Pp1/8/8/8/R3K2R w KQ g6 0 2";
	Board testBoard = Board();
	testBoard.loadFEN(zobristTestFEN);

    Move promotionMove = Move(1, 6, 2, 7, PieceType::Queen, testBoard);
    Move enPassantCaptureMove = Move(5, 4, 6, 5, PieceType::Empty, testBoard);
    Move castingRuinedByKingMove = Move(4, 0, 4, 1, PieceType::Empty, testBoard);
    Move castingRuinedByRookMove = Move(0, 0, 1, 0, PieceType::Empty, testBoard);

    zobristConsistancy_test_helper(testBoard, promotionMove);
    zobristConsistancy_test_helper(testBoard, enPassantCaptureMove);
    zobristConsistancy_test_helper(testBoard, castingRuinedByKingMove);
    zobristConsistancy_test_helper(testBoard, castingRuinedByRookMove);
}

void zobristConsistancy_perft_test()
{
	std::string startingFEN = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w";
	Board testBoard = Board();
	testBoard.loadFEN(startingFEN);

	long long beforeHash = testBoard.facts.hasher.hashValue;
	testBoard.perft(5);
	long long afterHash = testBoard.facts.hasher.hashValue;
	assert(beforeHash == afterHash);
}

void centerPawnMask_test()
{
    std::string fullCenterFEN = "8/8/8/3Pp3/3pP3/8/8/8 w - - 0 1";
    std::string threeCenterFEN = "8/8/8/3P4/3pP3/8/8/8 w - - 0 1";
    std::string twoCenterFEN = "8/8/8/3P4/3p4/8/8/8 w - - 0 1";
    std::string oneCenterFEN = "8/8/8/8/3p4/8/8/8 w - - 0 1";
    std::string emptyCenterFEN = "8/8/8/8/8/8/8/8 w - - 0 1";

    Board board = Board();
    board.loadFEN(fullCenterFEN);
    assert(bitwise::countBits(board.facts.pieces[PieceType::Pawn] & centerBoard) == 4);

    board.loadFEN(threeCenterFEN);
    assert(bitwise::countBits(board.facts.pieces[PieceType::Pawn] & centerBoard) == 3);

    board.loadFEN(twoCenterFEN);
    assert(bitwise::countBits(board.facts.pieces[PieceType::Pawn] & centerBoard) == 2);

    board.loadFEN(oneCenterFEN);
    assert(bitwise::countBits(board.facts.pieces[PieceType::Pawn] & centerBoard) == 1);

    board.loadFEN(emptyCenterFEN);
    assert(bitwise::countBits(board.facts.pieces[PieceType::Pawn] & centerBoard) == 0);
}

void firstAndLastRankMask_test() {
	std::string firstRankFEN = "8/8/8/8/8/8/8/PPPPPPPP w - - 0 1 ";
	std::string lastRankFEN = "PPPPPPPP/8/8/8/8/8/8/8 w - - 0 1 ";

	Board board = Board();
	board.loadFEN(firstRankFEN);
	assert(bitwise::countBits(board.facts.pieces[PieceType::Pawn] & rank1) == 8);
	assert(bitwise::countBits(board.facts.pieces[PieceType::Pawn] & rank8) == 0);

	board.loadFEN(lastRankFEN);
	assert(bitwise::countBits(board.facts.pieces[PieceType::Pawn] & rank1) == 0);
	assert(bitwise::countBits(board.facts.pieces[PieceType::Pawn] & rank8) == 8);
}

void saveAndLoadEmptyTransTable_test()
{
    Engine engine = Engine();
    engine.exportTransTable("dump.trans");
    engine.importTransTable("dump.trans");
}

void loadingEmptyTransTableClearsEntries_test()
{
    std::string startingFEN = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w";

    Board board = Board();
    board.loadFEN(startingFEN);
    Engine engine = Engine(board);
    engine.exportTransTable("dump.trans");
    engine.updateTranspositionBestIfDeeper(board, 5, Move(1,1,2,2, PieceType::Empty, board));
    engine.importTransTable("dump.trans");
    assert(engine.getTransposition(board).bestDepth == -1);
}

void loadingTranspositionTableLoadsEntries_test()
{
    std::string startingFEN = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w";

    Board board = Board();
    board.loadFEN(startingFEN);
    Engine originalEngine = Engine(board);
    originalEngine.updateTranspositionBestIfDeeper(board, 5, Move(1,1,2,2, PieceType::Empty, board));
    originalEngine.exportTransTable("dump.trans");

    Engine futureEngine = Engine(board);
    futureEngine.importTransTable("dump.trans");
    assert(futureEngine.getTransposition(board).bestDepth == 5);
    assert(futureEngine.getTransposition(board).bestMove == Move(1,1,2,2, PieceType::Empty, board));
}

void nullMoveChangesOnlyTurn_test()
{
	std::string startingFEN = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w";

	Board board = Board();
	board.loadFEN(startingFEN);
	board.makeMove(Move());
	assert(board.outputFEN() == "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR b");
	board.makeMove(Move());
	assert(board.outputFEN() == "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w");
}

void runAllTests()
{
	zobristConsistancy_perft_test();
	notBehindMaps();
	pawnMoveMaps();
	knightMoveMaps();
	kingMoveMaps();
	pawnPositionalScore_test_1();
	pawnPositionalScore_test_2();
	makeUnmakeEPCapture_test();
    loadStartingPosition_test();
	loadingStartingPosAfterE4_test();
    startingPerft_test();
    kiwipetePerft_test();
    endgamePerft_test();
    noMovesInMate_test();
    basicMateInOnePuzzle_test_1();
    basicMateInOnePuzzle_test_2();
    promotionMateInOnePuzzle_test_1();
    avoidMatePuzzle_test_1();
    zobristConsistancy_test();
    centerPawnMask_test();
	firstAndLastRankMask_test();
	nullMoveChangesOnlyTurn_test();
	//mateInThreePuzzle_test_1();


    //saveAndLoadEmptyTransTable_test();
    //loadingEmptyTransTableClearsEntries_test();
    //loadingTranspositionTableLoadsEntries_test();
}
