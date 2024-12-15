#pragma once

#include "globals.h"

void onCloseFile();


class Button {
public:
	Button(std::string name, void (*onPress)());
	void draw();
	void onPress();

	sf::FloatRect getGlobalRect();
	sf::Vector2f getPosition();
	sf::Vector2f getSize();
	void setPosition(sf::Vector2f position);

	void onHover();
	void onUnHover();
private:
	void(*onPressFunc)();

	sf::Text text;
	sf::RectangleShape background;
};
class PopUp {
public:
	PopUp(std::vector<Button> buttons);
	PopUp();
	virtual void spaceAround(Button& button);

	virtual void draw(Button* hover);
	/// <summary>
	/// returns 1 if I did press inside it, 0 otherwise
	/// </summary>
	/// <returns></returns>
	virtual bool onPress();
	virtual Button* onHover(sf::Vector2f mpos);
protected:
	std::vector<Button> children;
	sf::RectangleShape background;
};
class FindPopUp : public PopUp {
public:
	FindPopUp();
	void draw(Button* hover) override;
	bool onPress() override;
	Button* onHover(sf::Vector2f mpos) override;
	void focus();
private:
	Button Find, Next, Prev, Cancel;

	bool focused = 0;

	sf::RectangleShape TextBackground;
	sf::Text text;
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
	FindPopUp findPopUp;

	PopUp* currentPopUp;

	Button* hovering;
};