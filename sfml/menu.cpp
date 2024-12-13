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
PopUp::PopUp() {
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
	background.setFillColor(BAR::BG_COLOR);
	background.setOutlineColor(BAR::OUTLINE_COLOR);
	background.setOutlineThickness(BAR::OUTLINE_THICKNESS);
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
			BAR::events.push(BAR::SHOULD_CLOSE_POPUP); // close after pressing a button ig
			i.onPress();
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
void onPressFind() {
	BAR::events.push(BAR::OPEN_FIND_POPUP);
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

	if (BAR::safeToExit() == 0) {
		int rez = MessageBoxA(0, "You have unsaved changes! Discard them?", "Warning", MB_YESNO | MB_ICONWARNING);
		switch (rez) {
		case IDNO:
			return;
			break;
		case IDYES:
			break;
		}
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
	in.close();

	BAR::filepath_opened = std::string(buffer);
	BAR::markUnchanged();

	std::string str(ch);

	CONTENT::content->loadText(str);

	delete[] ch;
}
void onSaveFile() {
	if (BAR::filepath_opened == "") {
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
		
		BAR::filepath_opened = std::string(buffer);
	}
	//std::cout << buffer << '\n';

	std::ofstream fout(BAR::filepath_opened);
	if (!fout.is_open()) {
		std::cout << "Couldn't open or create file\n";
		return;
	}

	BAR::markUnchanged();

	std::string text = CONTENT::content->getString();
	fout << text;
	fout.close();
}
void onCloseFile() {
	if (BAR::safeToExit() == 0) {
		int rez = MessageBoxA(0, "You have unsaved changes! Discard them?", "Warning", MB_YESNO | MB_ICONWARNING);
		switch (rez) {
		case IDNO:
			return;
			break;
		case IDYES:
			break;
		}
	}
	window.close(); 
}
void onNewFile() {
	if (BAR::safeToExit() == 0) {
		int rez = MessageBoxA(0, "You have unsaved changes! Discard them?", "Warning", MB_YESNO | MB_ICONWARNING);
		switch (rez) {
		case IDNO:
			return;
			break;
		case IDYES:
			break;
		}
	}
	BAR::filepath_opened = "";
	BAR::markUnchanged();
	CONTENT::content->loadText("");
}

Menu::Menu() :
	buttons(std::vector<Button>({ // if modify order see the PopUp spaceAround() functions
			Button("File", onPressFile),
			Button("Edit", onPressEdit),
			Button("View", onPressView)
		})),
	filePopUp({
		Button("New", onNewFile),
		Button("Open", onOpenFile),
		Button("Save", onSaveFile),
		Button("Close", onCloseFile)
	}),
	editPopUp({
		Button("Find", onPressFind),
		Button("Cut", nullptr),
		Button("Copy", nullptr),
		Button("Paste", nullptr),
		Button("Select All", nullptr)
	}),
	viewPopUp({
		Button("Zoom in", nullptr),
		Button("Zoom out", nullptr),
		Button("Show lines", nullptr),
		Button("Word wrap", nullptr),
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
		case BAR::SHOULD_CLOSE_POPUP:
			currentPopUp = nullptr;
			break;
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
		case BAR::OPEN_FIND_POPUP:
			currentPopUp = &findPopUp;
			reinterpret_cast<FindPopUp*>(currentPopUp)->focus();
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

void onFindFind() {

}
void onFindPrev() {

}
void onFindNext() {

}
void onFindCancel() {
	BAR::events.push(BAR::SHOULD_CLOSE_POPUP);
}

FindPopUp::FindPopUp() :
	Find("Find", onFindFind),
	Prev("Prev", onFindPrev),
	Next("Next", onFindNext),
	Cancel("Cancel", onFindCancel)
{
	float x_total_aprox = Find.getSize().x + Prev.getSize().x + Next.getSize().x + Cancel.getSize().x + 5 * BAR::spacing;

	background.setPosition({ window.getSize().x / 2 - x_total_aprox / 2, BAR::HEIGHT + 4});
	background.setFillColor(BAR::BG_COLOR);
	background.setSize({ 100.0, 100.0 });
	background.setOutlineColor(BAR::OUTLINE_COLOR);
	background.setOutlineThickness(BAR::OUTLINE_THICKNESS);

	float x_cur = background.getPosition().x;
	float y_cur = background.getPosition().y;

	x_cur += BAR::spacing;

	Find.setPosition({ x_cur, y_cur });
	x_cur += Find.getSize().x;
	x_cur += BAR::spacing;

	Prev.setPosition({ x_cur, y_cur });
	x_cur += Prev.getSize().x;
	x_cur += BAR::spacing;

	Next.setPosition({ x_cur, y_cur });
	x_cur += Next.getSize().x;
	x_cur += BAR::spacing;

	Cancel.setPosition({ x_cur, y_cur });
	x_cur += Cancel.getSize().x;
	x_cur += BAR::spacing;

	background.setSize(sf::Vector2f(x_total_aprox, background.getSize().y));

	TextBackground.setPosition(sf::Vector2f(background.getPosition().x + 2 * BAR::spacing, Find.getPosition().y + Find.getSize().y + 2 * BAR::spacing));
	TextBackground.setSize(sf::Vector2f(x_total_aprox - 4 * BAR::spacing, BAR::HEIGHT - 4 - BAR::spacing));
	TextBackground.setOutlineColor(BAR::OUTLINE_COLOR);
	TextBackground.setOutlineThickness(BAR::OUTLINE_THICKNESS);
	TextBackground.setFillColor(sf::Color::Transparent);
	background.setSize(sf::Vector2f(background.getSize().x, TextBackground.getPosition().y - background.getPosition().y + TextBackground.getSize().y + BAR::spacing));
}
void FindPopUp::focus() {
	focused = 1;
}
void FindPopUp::draw(Button* hover) {
	if (focused == 1) TextBackground.setOutlineColor(sf::Color::White);
	else TextBackground.setOutlineColor(BAR::OUTLINE_COLOR);

	window.draw(background);

	if (hover == &Find)
		Find.onHover();
	Find.draw();
	if (hover == &Find) {
		Find.onUnHover();
	}

	if (hover == &Prev)
		Prev.onHover();
	Prev.draw();
	if (hover == &Prev)
		Prev.onUnHover();

	if (hover == &Next)
		Next.onHover();
	Next.draw();
	if (hover == &Next)
		Next.onUnHover();

	if (hover == &Cancel)
		Cancel.onHover();
	Cancel.draw();
	if (hover == &Cancel)
		Cancel.onUnHover();

	window.draw(TextBackground);
}
bool FindPopUp::onPress() {
	sf::Vector2f mpos = sf::Vector2f(sf::Mouse::getPosition(window));
	
	if (focused == 0 && TextBackground.getGlobalBounds().contains(mpos)) {
		focused = 1;
	}
	else if (!TextBackground.getGlobalBounds().contains(mpos)) focused = 0;

	if (Find.getGlobalRect().contains(mpos)) {
		Find.onPress();
	}
	if (Next.getGlobalRect().contains(mpos)) {
		Next.onPress();
	}
	if (Prev.getGlobalRect().contains(mpos)) {
		Prev.onPress();
	}
	if (Cancel.getGlobalRect().contains(mpos)) {
		Cancel.onPress();
	}
	
	if (background.getGlobalBounds().contains(mpos) == 1) {
		return 1;
	}
	return 0;
}
Button* FindPopUp::onHover(sf::Vector2f mpos) {
	if (Find.getGlobalRect().contains(mpos)) {
		return &Find;
	}
	if (Next.getGlobalRect().contains(mpos)) {
		return &Next;
	}
	if (Prev.getGlobalRect().contains(mpos)) {
		return &Prev;
	}
	if (Cancel.getGlobalRect().contains(mpos)) {
		return &Cancel;
	}

	return nullptr;
}