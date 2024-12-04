#pragma once

#include <iostream> // de scos
#include "globals.h"

class Content {
public:
	Content();

	/// <summary>
	/// Updates blinking state
	/// </summary>
	void update();

	void loadText(std::string str);

	void draw_content();

	void onKeyPress(sf::Uint32 code);
	void onKeyPress(sf::Keyboard::Key key);
	void onMousePress();
private:
	sf::Text getText();
	sf::Text getNumbers();

	void addText(char character);
	void addEnter();
	void removeChar(bool isCtrlPressed);
	void left(bool isCtrlPressed);
	void right(bool isCtrlPressed);
	void up();
	void down();

	void updateStrings();
	void updateCursor();

	sf::Text text;
	std::string textString;
	sf::Text numbers;
	std::string numbersString;

	sf::RectangleShape cursor;
	bool cursorState;
	int numberCount;
	int offset;
	time_t date;
};