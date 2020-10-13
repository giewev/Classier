#include <iostream>
#include "UciInterface.h"

UciInterface::UciInterface()
{
	board = Board();
	engine = Engine(board);
}

void UciInterface::processCommand(std::string command)
{
	if (command == "uci")
	{
		uci();
	}
	else if (command == "isready") 
	{
		isReady();
	}
	else if (command.find("position") != -1)
	{
		loadPosition(after(command, "position"));
	}
	else if (command.find("movetime ") != -1)
	{
		goMovetime(after(command, "movetime "));
	}
	else if (command.find("wtime ") != -1)
	{
		goTourny(command);
	}
	else if (command.find("perft ") != -1)
	{
		goPerft(after(command, "perft "));
	}
	else if (command.find("quit") != -1)
	{
		exit(0);
	}
}

void UciInterface::uci()
{
	std::cout << "uciok" << std::endl;
}

void UciInterface::isReady()
{
	std::cout << "readyok" << std::endl;
}

void UciInterface::loadPosition(std::string command)
{
	if (command.find("startpos") != -1)
	{
		board.loadFEN("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
	}
	else if (command.find("fen") != -1)
	{
		board.loadFEN(after(before(command, "moves"), "fen"));
	}

	makeMoves(after(command, "moves"));
	engine.setBoard(board);
}

void UciInterface::makeMoves(std::string command)
{
	while (command.length() > 0)
	{
		if (command[0] == ' ')
		{
			command = command.substr(1);
		}
		else
		{
			board.makeMove(Move(before(command, " "), board));
			command = after(command, " ");
		}
	}
}

void UciInterface::goPerft(std::string command)
{
	command = before(command, " ");
	std::cout << board.dividePerft(std::stoi(command)) << std::endl;
}

void UciInterface::goMovetime(std::string command)
{
	command = before(command, " ");
	std::cout << "bestmove " + engine.searchForTime(std::stoi(command)).basicAlg() << std::endl;
}

void UciInterface::goTourny(std::string command)
{
	if (board.facts.turn)
	{
		command = after(command, "wtime ");
	}
	else
	{
		command = after(command, "btime ");
	}

	command = before(command, " ");
	int totalMs = std::stoi(command);
	int timeToUse = totalMs / 30;
	std::cout << "bestmove " + engine.searchForTime(timeToUse).basicAlg() << std::endl;
}

std::string UciInterface::after(std::string outer, std::string inner)
{
	if (outer.find(inner) == -1) return "";
	int afterPosition = outer.find(inner) + strlen(inner.c_str());
	return outer.substr(afterPosition);
}

std::string UciInterface::before(std::string outer, std::string inner)
{
	if (outer.find(inner) == -1) return outer;
	return outer.substr(0, outer.find(inner));
}