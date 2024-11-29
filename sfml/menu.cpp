#include "menu.h"

#include <iostream>

Button::Button(std::string name, void (*onPress)())
	: onPressFunc(onPress) {

	text.setFont(font);
	text.setFillColor(BAR::TEXT_COLOR);
	text.setCharacterSize(BAR::HEIGHT - 4);
	text.setString(name);

	
	background.setSize(sf::Vector2f(text.getGlobalBounds().width, text.getGlobalBounds().height) + 
					   sf::Vector2f(5, 8));
	background.setFillColor(sf::Color::Red);
}
void Button::draw() {
	if (BAR::SHOW_HITBOX == 1) {
		window.draw(background);
	}

	window.draw(text);

	sf::FloatRect rect = text.getGlobalBounds();
}
void Button::onPress() {
	if (onPressFunc != nullptr)
		onPressFunc();
}
sf::FloatRect Button::getGlobalRect() {
	return background.getGlobalBounds();
}
sf::Vector2f Button::getPosition() {
	return background.getPosition();
}
sf::Vector2f Button::getSize() {
	return background.getSize();
}
void Button::setPosition(sf::Vector2f position) {
	text.setPosition(position + sf::Vector2f(0, 0));
	background.setPosition(position);
}

PopUp::PopUp(std::vector<Button> buttons) :children(buttons) {
	;
}
void PopUp::spaceAround(Button& button) {
	sf::Vector2f position = {9999, 9999};
	sf::Vector2f size = {0, 0};
	float last_pos = BAR::HEIGHT;
	for (auto& i : children) {
		i.setPosition({ button.getPosition().x, last_pos });
		last_pos += i.getSize().y;

		position.x = std::min(position.x, i.getPosition().x);
		position.y = std::min(position.y, i.getPosition().y);
		size.x = std::max(size.x, i.getSize().x);
		size.y += i.getSize().y;
	}
	background.setPosition(position);
	background.setSize(size);
	background.setFillColor(BAR::POPUP_COLOR);
}
void PopUp::draw() {
	window.draw(background);
	for (auto &i : children) {
		i.draw();
	}
}
bool PopUp::onPress() {
	sf::Vector2f mpos = sf::Vector2f(sf::Mouse::getPosition(window));
	for (auto& i : children) {
		if (i.getGlobalRect().contains(mpos)) {
			i.onPress();
			BAR::events.push(BAR::SHOULD_CLOSE_POPUP); // close after pressing a button ig
			break; // its not like I'm gona press two buttons
		}
	}

	if (background.getGlobalBounds().contains(mpos) == 1) {
		return 1;
	}
	return 0;
}


void onPressFile() {
	BAR::events.push(BAR::OPEN_FILE_POPUP);
}
void onPressEdit() {
	BAR::events.push(BAR::OPEN_EDIT_POPUP);
}
void onPressView() {
	BAR::events.push(BAR::OPEN_VIEW_POPUP);
}
void onOpenFile() {
	std::cout << "Opening file here need content API for this though" << '\n';
}


Menu::Menu() :
	buttons(std::vector<Button>({ // if modify order see the PopUp spaceAround() functions
			Button("File", onPressFile),
			Button("Edit", onPressEdit),
			Button("View", onPressView)
		})),
	filePopUp({
		Button("Open", onOpenFile),
		Button("This is another button", nullptr),
		Button("Yet another one", nullptr)
	}),
	editPopUp({
		Button("Edit", nullptr)
	}),
	viewPopUp({
		Button("View", nullptr)
	})
{
	background.setPosition({ 0.0f, 0.0f });
	background.setSize({ float(window.getSize().x), BAR::HEIGHT });
	background.setFillColor(BAR::BG_COLOR);

	float last_pos = 0; 
	for (auto &i : buttons) { 
		i.setPosition(sf::Vector2f(last_pos, 0));
		last_pos += i.getSize().x;
	}

	// if modify order see the buttons initialization
	filePopUp.spaceAround(buttons[0]);
	editPopUp.spaceAround(buttons[1]);
	viewPopUp.spaceAround(buttons[2]);
	currentPopUp = nullptr;
}

void Menu::draw() {
	; // updates

	while (!BAR::events.empty()) {
		BAR::EVENT ev = BAR::events.front();
		BAR::events.pop();
		switch (ev) {
		case BAR::OPEN_FILE_POPUP:
			if (currentPopUp == &filePopUp) currentPopUp = nullptr;
			else currentPopUp = &filePopUp;
			break;
		case BAR::OPEN_EDIT_POPUP:
			if (currentPopUp == &editPopUp) currentPopUp = nullptr;
			else currentPopUp = &editPopUp;
			break;
		case BAR::OPEN_VIEW_POPUP:
			if (currentPopUp == &viewPopUp) currentPopUp = nullptr;
			else currentPopUp = &viewPopUp;
			break;
		case BAR::SHOULD_CLOSE_POPUP:
			currentPopUp = nullptr;
			break;
		default:
			break;
		}
	}

	; // draws

	window.draw(background);
	for (auto& i : buttons) {
		i.draw();
	}
	if (currentPopUp != nullptr) {
		currentPopUp->draw();
	}
}

void Menu::onPress() {
	bool pressed_inside = 0;

	sf::Vector2f mpos = sf::Vector2f(sf::Mouse::getPosition(window));
	for (auto& i : buttons) {
		if (i.getGlobalRect().contains(mpos)) {
			i.onPress();
			pressed_inside = 1;
			break; // its not like I can press 2 buttons at once
		}
	}

	if (currentPopUp != nullptr) {
		bool pressed_in_popup = currentPopUp->onPress();
		if (pressed_in_popup) pressed_inside = 1;
	}

	if (pressed_inside == 0) {
		if (currentPopUp != nullptr) {
			BAR::events.push(BAR::SHOULD_CLOSE_POPUP);
		}
	}
}
void Menu::onMouseMove() {
	; // todo hover show button
}