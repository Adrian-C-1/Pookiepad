#pragma once

#include "globals.h"
#include "content.h"

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
	void setFillColor(sf::Color col);
	void setText(std::string text);
	std::string getText();
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
	void reset();
	void onKeyPress(sf::Uint32 code);
private:
	Button Find, Next, Prev, Cancel;

	bool focused = 0;

	sf::RectangleShape TextBackground;
	sf::Text text;
	int line = 0, occurence = 0;
};

class Menu {
public:
	Menu();
	void draw();
	bool onPress();
	void onMouseMove();
	void onResize();
	void markChanged();
	void markUnchanged();

	/// returneaza 1 daca am facut ceva cu cheia (am deschid find)
	bool onKeyPress();
	/// returneaza 1 daca am facut ceva cu cheia (am deschid find)
	bool onTextEntered(sf::Uint32 code);
	
	void scrollbar_make_good();
private:
	void ordonPages();
	float getPage0x();
	void saveFile();
	void closeFile();
	sf::RectangleShape background;
	sf::RectangleShape background_butoane;
	std::vector<Button> buttons;

	PopUp filePopUp, editPopUp, viewPopUp;
	FindPopUp findPopUp;

	PopUp* currentPopUp;

	Button* hovering;


	struct page {
		Content *content;
		Button button;
		std::string path = "";
		bool saved = 1;
	};
	std::vector<page> pages;
	int current_page = 0;
	float page_draw_offset = 0.0;
	sf::Vector2f last_mouse_press_position = {-1, -1};

	bool should_draw_scrollbar();
	void scrollbar_move_to(sf::Vector2f mpos);
	void scrollbar_was_moved();
	sf::RectangleShape scrollbar_background;
	sf::RectangleShape scrollbar_active;
	bool scrollbar_holding;
	int scrollbar_last_mouse_y;

};