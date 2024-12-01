#pragma once

#include <iostream> // de scos
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
	sf::Text getText();
	sf::Text getNumbers();
	sf::RectangleShape getCursor();
	time_t getDate();
	void updateStrings();
	void updateCursor();

	// todo
	void setText(std::string str);
	void resetNumbers();

	void addText(char character);
	void addEnter();
	void removeChar(bool isCtrlPressed);
	void left(bool isCtrlPressed);
	void right(bool isCtrlPressed);
	void up();
	void down();
	void updateCursorBlink();
	void draw_content(sf::RenderWindow& windoww);
};