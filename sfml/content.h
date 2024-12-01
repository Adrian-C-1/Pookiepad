#pragma once

#include "globals.h"

class Content {
private:
	sf::Text text;
	std::string textString;
	sf::Text numbers;
	std::string numbersString;

	sf::RectangleShape cursor;
	bool cursorState;
	int numberCount;
	int offset;
	time_t date;
public:
	Content();

};