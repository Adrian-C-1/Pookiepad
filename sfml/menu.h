#pragma once

#include "globals.h"


class Button {
public:
	Button(std::string name, void (*onPress)());
	void draw();
	void onPress();

	sf::FloatRect getGlobalRect();
	sf::Vector2f getPosition();
	sf::Vector2f getSize();
	void setPosition(sf::Vector2f position);
private:
	void(*onPressFunc)();

	sf::Text text;
	sf::RectangleShape background;
};
class PopUp {
public:
	PopUp(std::vector<Button> buttons);
	void spaceAround(Button& button);

	void draw();
	/// <summary>
	/// returns 1 if I did press inside it, 0 otherwise
	/// </summary>
	/// <returns></returns>
	bool onPress();
private:
	std::vector<Button> children;
	sf::RectangleShape background;
};

class Menu {
public:
	Menu();
	void draw();
	bool onPress();
	void onMouseMove();
private:
	sf::RectangleShape background;
	std::vector<Button> buttons;

	PopUp filePopUp, editPopUp, viewPopUp;
	PopUp* currentPopUp;
};