#pragma once

#include "globals.h"

class Content {
private:
	sf::Text text;
	std::string textString;
	sf::Text numbers;
	std::string numbersString;

	sf::RectangleShape cursor(sf::Vector2f(12.f, 24.f));
	bool cursorState = false;
	int numberCount = 1;
	int offset = 0;
	time_t date = std::time(0);
public:
	Content();

};