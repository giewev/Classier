#pragma once
#include <string>
#include <vector>

#include "Engine.h"
#include "Board.h"

class UciInterface
{
public:
	UciInterface();
	void processCommand(std::string command);
private:
	void loadPosition(std::string);
	void makeMoves(std::string);

	void goPerft(std::string);
	void goTourny(std::string);
	void goMovetime(std::string);
	void uci();
	void isReady();

	std::string after(std::string outer, std::string inner);
	std::string before(std::string outer, std::string inner);

	Engine engine;
	Board board;
};