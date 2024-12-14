#include "menu.h"

#include <iostream>
#include <fstream>
#include "content.h"

Button::Button(std::string name, void (*onPress)())
	: onPressFunc(onPress) {

	text.setFont(BAR::font);
	text.setFillColor(BAR::TEXT_COLOR);
	text.setCharacterSize(BAR::HEIGHT - 4 - BAR::spacing);
	text.setString(name);

	
	background.setSize(sf::Vector2f(	
							text.getGlobalBounds().width + 2.0 * BAR::spacing,  
							text.getGlobalBounds().height
					   ) +  sf::Vector2f(5, 8));
	background.setFillColor(sf::Color::Transparent);
}
void Button::draw() {
	window.draw(background);
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
	background.setPosition(position);
	text.setPosition(position + sf::Vector2f(BAR::spacing, 0));
}
void Button::onHover() {
	background.setFillColor(BAR::HOVER_COLOR);
}
void Button::onUnHover() {
	background.setFillColor(sf::Color::Transparent);
}

PopUp::PopUp(std::vector<Button> buttons) :children(buttons) {
	;
}
void PopUp::spaceAround(Button& button) {
	sf::Vector2f position = {9999, 9999};
	sf::Vector2f size = {0, 0};
	float last_pos = BAR::HEIGHT;
	
	float spatiu_sub_buton = 4;
	float spatiu_sus_background = BAR::spacing;
	for (auto& i : children) {
		i.setPosition({ button.getPosition().x, last_pos + spatiu_sus_background + spatiu_sub_buton });
		last_pos += i.getSize().y;
		last_pos += BAR::spacing;

		position.x = std::min(position.x, i.getPosition().x);
		position.y = std::min(position.y, i.getPosition().y);
		size.x = std::max(size.x, i.getSize().x);
		size.y += i.getSize().y;
		size.y += BAR::spacing;
	}
	position.y -= spatiu_sus_background;
	size.y += spatiu_sus_background;

	background.setPosition(position);
	background.setSize(size);
	background.setFillColor(BAR::POPUP_COLOR);
}
void PopUp::draw(Button* hover) {
	window.draw(background);
	for (int i = 0; i < children.size(); i++) {
		if (&children[i] == hover) {
			children[i].onHover();
		}
		children[i].draw();
		if (&children[i] == hover) {
			children[i].onUnHover();
		}
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
Button* PopUp::onHover(sf::Vector2f mpos) {
	for (int i = 0; i < children.size(); i++) {
		if (children[i].getGlobalRect().contains(mpos)) {
			return &children[i];
		}
	}
	return nullptr;
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
	OPENFILENAMEA open = { 0 };

	char buffer[2048];
	buffer[0] = '\0';

	open.hwndOwner = handle;
	open.hInstance = GetModuleHandleA(NULL);
	open.lStructSize = sizeof(OPENFILENAMEA);
	open.lpstrFile = buffer;
	open.nMaxFile = sizeof(buffer);
	open.lpstrFilter = "All files\0*.*\0";
	open.nFilterIndex = 1;
	open.lpstrFileTitle = NULL;
	open.nMaxFileTitle = 0;
	open.lpstrInitialDir = NULL;
	open.lpstrTitle = "Open File";
	open.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST;

	BOOL selected = GetOpenFileNameA(&open);

	if (!selected){
		// sth happeded
		return;
	}
	
	std::ifstream in(buffer, std::ios::ate);
	if (!in.is_open()) {
		std::cout << "Couldn't open file\n";
		return;
	}
	int size = in.tellg();
	char* ch = new char[size + 1];
	std::memset(ch, 0, size + 1);
	in.seekg(0);
	in.read(ch, size);
	ch[size] = NULL;
	
	std::string str(ch);

	CONTENT::content->loadText(str);

	delete[] ch;
}
void onSaveFile() {
	OPENFILENAMEA open = { 0 };

	char buffer[2048];
	buffer[0] = '\0';

	open.hwndOwner = handle;
	open.hInstance = GetModuleHandleA(NULL);
	open.lStructSize = sizeof(OPENFILENAMEA);
	open.lpstrFile = buffer;
	open.nMaxFile = sizeof(buffer);
	open.lpstrFilter = "All files\0*.*\0";
	open.nFilterIndex = 1;
	open.lpstrFileTitle = NULL;
	open.nMaxFileTitle = 0;
	open.lpstrInitialDir = NULL;
	open.lpstrTitle = "Open File";
	open.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST;

	BOOL selected = GetSaveFileNameA(&open);

	if (!selected) {
		// sth happeded
		return;
	}

	std::cout << buffer << '\n';

	std::ofstream fin(buffer);
	if (!fin.is_open()) {
		std::cout << "Couldn't open or create file\n";
		return;
	}

	//std::string text = CONTENT::content.getWholeText();

}

Menu::Menu() :
	buttons(std::vector<Button>({ // if modify order see the PopUp spaceAround() functions
			Button("File", onPressFile),
			Button("Edit", onPressEdit),
			Button("View", onPressView)
		})),
	filePopUp({
		Button("Open", onOpenFile),
		Button("Save", onSaveFile),
		Button("Close", nullptr),
		Button("New", nullptr)
	}),
	editPopUp({
		Button("Edit", nullptr)
	}),
	viewPopUp({
		Button("View", nullptr)
	})
{
	hovering = nullptr;

	background.setPosition({ 0.0f, 0.0f });
	background.setSize({ float(window.getSize().x), BAR::HEIGHT });
	background.setFillColor(BAR::BG_COLOR);

	float last_pos = 0; 
	for (auto &i : buttons) { 
		i.setPosition(sf::Vector2f(last_pos + BAR::spacing, BAR::spacing));
		last_pos += i.getSize().x + BAR::spacing;
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
	for (int i = 0; i < buttons.size(); i++) {
		if (&buttons[i] == hovering) {
			buttons[i].onHover();
		}
		buttons[i].draw();
		if (&buttons[i] == hovering) {
			buttons[i].onUnHover();
		}
	}
	if (currentPopUp != nullptr) {
		currentPopUp->draw(hovering);
	}
}

bool Menu::onPress() {
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

	return pressed_inside;
}
void Menu::onMouseMove() {
	; // todo hover show button
	sf::Vector2f mpos = sf::Vector2f(sf::Mouse::getPosition(window));

	hovering = nullptr;
	for (int i = 0; i < buttons.size(); i++) {
		if (buttons[i].getGlobalRect().contains(mpos)) {
			hovering = &buttons[i];
		}
	}
	if (hovering == nullptr) {
		if (currentPopUp != nullptr) {
			hovering = currentPopUp->onHover(mpos);
		}
	}
}