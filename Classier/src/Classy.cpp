#include <iostream>
#include <string>
#include <iomanip>
#include <fstream>
#include "Board.h"
#include "Engine.h"
#include "Move.h"
#include "ArgumentParser.h"
#include "Piece.h"
#include "Bitwise.h"
#include <UciInterface.h>

void runAllTests();

int main(int argc, char *argv[])
{
    ArgumentParser argParser = ArgumentParser(argc, argv);
	bitwise::initializeBitboards();
    if (argParser.flagExists("-t"))
    {
        runAllTests();
    }
    else if (argParser.flagExists("-fen"))
    {
        Board board = Board();
        board.loadFEN(argParser.argumentContent("-fen"));
        Engine engine = Engine(board);

        if (argParser.flagExists("-depth"))
        {
            int moveDepth = std::stoi(argParser.argumentContent("-depth"));
            std::cout << engine.searchToDepth(moveDepth).basicAlg() << std::endl;
        }
        else if (argParser.flagExists("-time"))
        {
            double searchTime = std::stod(argParser.argumentContent("-time")) * 1000;
            std::cout << engine.searchForTime(searchTime).basicAlg() << std::endl;
        }
		else if (argParser.flagExists("-moves"))
		{
			Move moveList[210];
			int moveCounter = 0;
			board.generateMoveArray(moveList, moveCounter);
			std::cout << moveCounter << std::endl;
			for (int i = 0; i < moveCounter; i++)
			{
				std::cout << moveList[i].basicAlg() << std::endl;
			}
		}
		else if (argParser.flagExists("-divide"))
		{
			std::cout << board.dividePerft(std::stoi(argParser.argumentContent("-divide"))) << std::endl;
		}
    }
	else {
		UciInterface uci = UciInterface();

		for (std::string line; std::getline(std::cin, line);) {
			uci.processCommand(line);
		}
	}
	
    return 0;
}
