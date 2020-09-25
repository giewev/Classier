#include <iostream>
#include <string>
#include <iomanip>
#include <fstream>
#include "Board.h"
#include "Engine.h"
#include "Danger.h"
#include "Move.h"
#include "ArgumentParser.h"

void runAllTests();

int main(int argc, char *argv[])
{
    ArgumentParser argParser = ArgumentParser(argc, argv);
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
    }
	else {
		Board board = Board();
		Engine engine = Engine(board);
		std::ofstream inFile;
		std::ofstream outFile;
		inFile.open("./arena_commands.txt");
		outFile.open("./engine_responses.txt");

		for (std::string line; std::getline(std::cin, line);) {
			inFile << line << std::endl;
			std::string response = "";
			if (line == "uci")
			{
				response = "uciok";
			}
			else if (line == "isready"){
				response = "readyok";
			}
			else if (line.find("position") != -1) {
				int count = 0;
				std::string accum = "";
				for (int i = 0; i < line.length(); i++)
				{
					char c = line[i];
					accum = accum + c;
					if (c == ' ' || i == line.length() - 1)
					{
						count++;
						if (count == 2)
						{
							if (line.find("startpos") != -1){
								board.loadFEN("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
							}
							else {
								board.loadFEN(accum);
							}
						}
						else if (count > 3)
						{
							board.makeMove(Move(accum));
						}
						accum = "";
					}
				}

				engine.setBoard(board);
			}
			else if (line.find("go ") != -1)
			{
				outFile << "received a go command" << std::endl;
				response = "bestmove " + engine.searchForTime(2000).basicAlg();
			}
			else if (line.find("quit") != -1)
			{
				return 0;
			}

			inFile << response << std::endl << std::endl;
			if (response != "")
			{
				std::cout << response << std::endl;
				outFile << response << std::endl;
			}
		}
		inFile.close();
	}
	
    return 0;
}
