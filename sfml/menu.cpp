#include "menu.h"

#include <iostream>
#include <fstream>
#include "content.h"

Button::Button(std::string name, void (*onPress)())
	: onPressFunc(onPress) {

	text.setFont(BAR::font);
	text.setCharacterSize(BAR::HEIGHT - 4 - BAR::spacing);
	text.setString(name);

	
	background.setSize(sf::Vector2f(	
							text.getGlobalBounds().width + 2.0 * BAR::spacing,  
							text.getGlobalBounds().height
					   ) +  sf::Vector2f(5, 8));
	background.setFillColor(sf::Color::Transparent);
	changeTheme();
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
void Button::setTextFillColor(sf::Color col) {
	text.setFillColor(col);
}
void Button::setText(std::string text) {
	this->text.setString(text);
	background.setSize(sf::Vector2f(
		this->text.getGlobalBounds().width + 2.0 * BAR::spacing,
		this->text.getGlobalBounds().height
	) + sf::Vector2f(5, 8));
}
std::string Button::getText() {
	return text.getString();
}

PopUp::PopUp(std::vector<Button> buttons) :children(buttons) {
	
}
PopUp::PopUp() {
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
	background.setOutlineThickness(BAR::OUTLINE_THICKNESS);
	changeTheme();
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
std::string getOpenPath(){
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

	if (!selected) {
		// sth happeded
		return "";
	}

	return std::string(buffer);
}
std::string getSavePath() {

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
		return "";
	}

	return std::string(buffer);
}
void onOpenFile() {
	BAR::events.push(BAR::OPEN_FILE);
}
void onSaveFile() {
	BAR::events.push(BAR::SAVE_FILE);
}
void onCloseFile() {
	BAR::events.push(BAR::CLOSE_FILE);
}
void onNewFile() {
	BAR::events.push(BAR::NEW_FILE);
}
void onZoomIn() {
	CONTENT::content->zoomIn();
}
void onZoomOut() {
	CONTENT::content->zoomOut();
}
void onEditpaste() {
	CONTENT::content->paste();
}
void onShowLines() {
	CONTENT::content->showlines();
}
void onEditCut() {
	CONTENT::content->copy(true);
}
void onEditCopy() {
	CONTENT::content->copy(false);
}
void onEditSelectAll() {
	CONTENT::content->selectAll();
}
void onChangeTheme() {
	BAR::changeTheme();
	BAR::events.push(BAR::CHANGED_THEME);
}
void Menu::changeTheme() {
	background_butoane.setFillColor(BAR::BG_COLOR);
	background.setFillColor(BAR::BG_COLOR);
	notice.setFillColor(BAR::NOTICE_COLOR);
	for (auto& i : buttons) {
		i.changeTheme();
	}
	filePopUp.changeTheme();
	editPopUp.changeTheme();
	viewPopUp.changeTheme();
	findPopUp.changeTheme();
	for (auto& i : pages) {
		i.button.changeTheme();
		i.content->changeTheme();
	}
	scrollbar_background.setFillColor(BAR::SCROLLBAR_BG_COLOR);
	scrollbar_active.setFillColor(BAR::SCROLLBAR_ACTIVE_COLOR);
	ordonPages();
}

void f() {
	std::cout << "mami sunt bun!\n";
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
		Button("Find", onPressFind), // todo
		Button("Cut", onEditCut),
		Button("Copy", onEditCopy),
		Button("Paste", onEditpaste),
		Button("Select All", onEditSelectAll)
		}),
	viewPopUp({
		Button("Zoom in", onZoomIn),
		Button("Zoom out", onZoomOut),
		Button("Show lines", onShowLines),
		Button("Change theme", onChangeTheme)
	})
{
	onChangeTheme(); // default light theme

	hovering = nullptr;

	background.setPosition({ 0.0f, 0.0f });
	background.setSize({ float(window.getSize().x), BAR::HEIGHT });

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

	background_butoane.setPosition({ 0, 0 });
	background_butoane.setSize(sf::Vector2f(buttons[buttons.size() - 1].getPosition().x + buttons[buttons.size() -1 ].getSize().x, BAR::HEIGHT));

	pages.push_back({ new Content(), Button("Untitled", nullptr) });
	current_page = 0;
	CONTENT::content = pages[0].content;
	ordonPages();

	notice.setFont(font);
	notice.setCharacterSize(24);

	onResize();
	changeTheme();
}
void Menu::scrollbar_make_good() { 
	float count = CONTENT::content->getLineCount(), line = CONTENT::content->getCurrentLine();
	float percentage = line / count;
	float possible = scrollbar_background.getSize().y - scrollbar_active.getSize().y;
	float off_y = possible * percentage;
	scrollbar_active.setPosition(scrollbar_background.getPosition());
	scrollbar_active.move(sf::Vector2f(0, off_y));
}
float Menu::getPage0x() {
	return buttons[buttons.size() - 1].getPosition().x + buttons[buttons.size() - 1].getSize().x + BAR::spacing * 3;
}
void Menu::ordonPages() {
	pages[0].button.setPosition(sf::Vector2f(getPage0x(), buttons[2].getPosition().y));
	for (int i = 1; i < pages.size(); i++) {
		sf::Vector2f pos = pages[i - 1].button.getPosition();
		sf::Vector2f siz = pages[i - 1].button.getSize();
		pages[i].button.setPosition(pos + sf::Vector2f(siz.x, 0.0));
	}
	for (int i = 0; i < pages.size(); i++) {
		if (i == current_page) {
			pages[i].button.setTextFillColor(BAR::CURRENT_PAGE_TEXT_COLOR);
		}
		else {
			pages[i].button.setTextFillColor(BAR::OTHER_PAGE_TEXT_COLOR);
		}
	}
	CONTENT::content = pages[current_page].content;
}
void Menu::onResize() {
	background.setSize({ float(window.getSize().x), BAR::HEIGHT });


	scrollbar_background.setSize(sf::Vector2f(BAR::spacing * 2, window.getSize().y - BAR::HEIGHT - 4 * BAR::spacing));
	scrollbar_background.setPosition(sf::Vector2f(window.getSize().x - BAR::spacing * 5, BAR::HEIGHT + 2 * BAR::spacing));

	scrollbar_holding = 0;

	float height = std::max(scrollbar_background.getSize().y / 10, 10.0f);
	height = std::min(scrollbar_background.getSize().y, height);
	scrollbar_active.setSize(sf::Vector2f(scrollbar_background.getSize().x, height));
	scrollbar_active.setPosition(scrollbar_background.getPosition());
	scrollbar_make_good();

	findPopUp.onResize();
}
void Menu::saveFile() {
	if (pages[current_page].path.size() == 0) { // no path selected
		pages[current_page].path = getSavePath();

		if (pages[current_page].path.size() == 0) return;
		pages[current_page].button.setText(BAR::getFileFromFilepath(pages[current_page].path));
		ordonPages();
	}
	if (pages[current_page].path.size() == 0) return;

	std::ofstream fout(pages[current_page].path);
	if (!fout.is_open()) {
		std::cout << "Couldn't open or create file\n";
		return;
	}
	markUnchanged();
	std::string text = CONTENT::content->getString();
	fout << text;
	fout.close();
}
void Menu::closeFile() {
	if (pages[current_page].saved == 0) {
		std::string smallp = BAR::getFileFromFilepath(pages[current_page].path);
		std::string str = std::string("You have unsaved changed for the file ") + (smallp == "" ? "Untitled" : smallp);
		str += std::string("\nWould you like to save?");
		int response = MessageBoxA(NULL, str.c_str(), "Warning!", MB_YESNOCANCEL | MB_ICONWARNING);
		switch (response) {
		case IDYES:
			saveFile();
			if (pages[current_page].path == "") return;
			break;
		case IDNO:
			break;
		case IDCANCEL:
			return;
		}
	}
	pages[current_page].content->destroyTree();
	delete pages[current_page].content;
	pages.erase(pages.begin() + current_page);
	current_page = 0;
	if (pages.size() == 0) {
		window.close();
		return;
	}
	ordonPages();
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
			reinterpret_cast<FindPopUp*>(currentPopUp)->reset();
			break;
		case BAR::NEW_FILE:
			pages.push_back({ new Content(), Button("Untitled", nullptr) });
			current_page = pages.size() - 1;
			pages[current_page].saved = 1;
			ordonPages();
			break;
		case BAR::FIND_FIND:
			if (CONTENT::content->find(reinterpret_cast<FindPopUp*>(currentPopUp)->getText()) == 0) {
				setNotice("Couldn't find text");
			}
			break;
		case BAR::FIND_NEXT:
			if (CONTENT::content->findNext(reinterpret_cast<FindPopUp*>(currentPopUp)->getText()) == 0) {
				setNotice("Couln't find next text");
			}
			break;
		case BAR::FIND_PREV:
			if (CONTENT::content->findPrev(reinterpret_cast<FindPopUp*>(currentPopUp)->getText()) == 0) {
				setNotice("Couldn't find previous text");
			}
			break;
		case BAR::SAVE_FILE:
		{
			saveFile();
			break;
		}
		case BAR::OPEN_FILE:
		{
			std::string buffer = getOpenPath();
			if (buffer == "") break;

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

			std::string str(ch);

			pages.push_back({ new Content(str), Button(BAR::getFileFromFilepath(buffer), nullptr) });
			current_page = pages.size() - 1;
			pages[current_page].saved = 1;
			pages[current_page].path = buffer;
			ordonPages();

			delete[] ch;
			break;
		}
		case BAR::CLOSE_FILE:
		{
			closeFile();
			if (pages.size() == 0) return;
			break;
		}
		case BAR::CLOSE_ALL:
		{
			while(pages.size()){
				int old = pages.size();
				closeFile();
				if (pages.size() == old) break;
			}
			if (pages.size() == 0) {
				window.close();
				return;
			}
			break;
		}
		case BAR::CHANGED_THEME:
			changeTheme();
			CONTENT::content->changeTheme();
			break;
		default:
			break;
		
		}
	}

	; // draws

	window.draw(background);

	for (int i = 0; i < pages.size(); i++) {
		if (&pages[i].button == hovering) {
			pages[i].button.onHover();
		}
		pages[i].button.setPosition(pages[i].button.getPosition() + sf::Vector2f(page_draw_offset, 0.0));
		pages[i].button.draw(); // draw on left a bar and draw buttons on top
		pages[i].button.setPosition(pages[i].button.getPosition() + sf::Vector2f(-page_draw_offset, 0.0));
		if (&pages[i].button == hovering) {
			pages[i].button.onUnHover();
		}
	}

	window.draw(background_butoane);

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
	
	if (scrollbar_holding || should_draw_scrollbar()) {
		window.draw(scrollbar_background);
		window.draw(scrollbar_active);
	}

	sf::Color col = notice.getFillColor();
	int a = col.a;
	a -= 1;
	a = std::max(a, 0);
	col.a = a;
	notice.setFillColor(col);
	window.draw(notice);
}
void Menu::setNotice(std::string text) {
	notice.setString(text);
	sf::Vector2f size = sf::Vector2f(notice.getGlobalBounds().width, notice.getGlobalBounds().height);
	notice.setPosition(sf::Vector2f(window.getSize().x / 2 - size.x / 2, window.getSize().y - 40));
	notice.setFillColor(BAR::TEXT_COLOR);
}
void Menu::markChanged() {
	if (pages[current_page].saved == 1) {
		std::string acm = pages[current_page].button.getText();
		acm.insert(acm.begin(), '*');
		pages[current_page].button.setText(acm);
		ordonPages();
		pages[current_page].saved = 0;
	}
}
void Menu::markUnchanged() {
	if (pages[current_page].saved == 0) {
		std::string acm = pages[current_page].button.getText();
		acm.erase(acm.begin());
		if (pages[current_page].path != "") acm = BAR::getFileFromFilepath(pages[current_page].path);
		pages[current_page].button.setText(acm);
		ordonPages();
		pages[current_page].saved = 1;
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

	for (int i = 0; i < pages.size(); i++) {
		mpos.x -= page_draw_offset;
		if (pages[i].button.getGlobalRect().contains(mpos)) {
			if (current_page == i) return 1;
			current_page = i;
			ordonPages();
			CONTENT::content = pages[i].content;
		}
		mpos.x += page_draw_offset;
	}

	if (!pressed_inside && background.getGlobalBounds().contains(mpos)) {
		last_mouse_press_position = mpos;
	}
	else
		last_mouse_press_position = { -1, -1 };

	if (!pressed_inside) {
		sf::FloatRect sbr = scrollbar_background.getGlobalBounds();
		sbr.left -= BAR::SCROLLBAR_PROXIMITY;
		sbr.width += 2 * BAR::SCROLLBAR_PROXIMITY;
		if (sbr.contains(mpos)) {
			pressed_inside = 1;
			sf::FloatRect sba = scrollbar_active.getGlobalBounds();
			sba.left -= BAR::SCROLLBAR_PROXIMITY;
			sba.width += 2 * BAR::SCROLLBAR_PROXIMITY;
			if (sba.contains(mpos)) {
				scrollbar_holding = 1;
				scrollbar_last_mouse_y = mpos.y;
			}
			else {
				scrollbar_move_to(mpos);
				scrollbar_make_good();
			}
		}
	}

	return pressed_inside;
}
void Menu::onMouseMove() {
	
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
	if (hovering == nullptr) {
		mpos.x -= page_draw_offset;
		for (int i = 0; i < pages.size(); i++) {
			if (pages[i].button.getGlobalRect().contains(mpos)) {
				hovering = &pages[i].button;
			}
		}
		mpos.x += page_draw_offset;
	}

	if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
		if (last_mouse_press_position.x != -1 || last_mouse_press_position.y != -1) {
			page_draw_offset += mpos.x - last_mouse_press_position.x;
			last_mouse_press_position = mpos;
			
			float last_page_x_after = pages[pages.size() - 1].button.getPosition().x + pages[pages.size() - 1].button.getSize().x + page_draw_offset;
			if (last_page_x_after < window.getSize().x) {
				page_draw_offset += (window.getSize().x - last_page_x_after);
			}
			if (pages[0].button.getPosition().x + page_draw_offset > getPage0x())
				page_draw_offset -= (getPage0x() - pages[0].button.getPosition().x + page_draw_offset);
		}
	}

	if (scrollbar_holding == 1) {
		if (!sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
			scrollbar_holding = 0;
			return;
		}
		else {
			sf::Vector2f old_scrollbar_pos = scrollbar_active.getPosition();
			scrollbar_active.move(sf::Vector2f(0, mpos.y - scrollbar_last_mouse_y));
			if (scrollbar_active.getPosition().y < scrollbar_background.getPosition().y)
				scrollbar_active.setPosition(scrollbar_background.getPosition());
			if (scrollbar_active.getPosition().y + scrollbar_active.getSize().y > scrollbar_background.getPosition().y + scrollbar_background.getSize().y)
				scrollbar_active.move(-sf::Vector2f(0, scrollbar_active.getPosition().y + scrollbar_active.getSize().y - scrollbar_background.getPosition().y - scrollbar_background.getSize().y));
			if (scrollbar_active.getPosition() != old_scrollbar_pos) {
				scrollbar_was_moved();
			}
			scrollbar_last_mouse_y = mpos.y;
		}
	}

}
/// returneaza 1 daca am facut ceva cu cheia (am deschid find)
bool Menu::onKeyPress() {
	return (currentPopUp == &findPopUp) || scrollbar_holding;
}
/// returneaza 1 daca am facut ceva cu cheia (am deschid find)
bool Menu::onTextEntered(sf::Uint32 code) {
	if (scrollbar_holding) 
		return 1;

	if (currentPopUp == &findPopUp) {
		reinterpret_cast<FindPopUp*>(currentPopUp)->onKeyPress(code);
		return 1;
	}
	return 0;
}
void Menu::scrollbar_was_moved() {
	float percentage = 0.0;
	float real_end = scrollbar_background.getPosition().y + scrollbar_background.getSize().y - scrollbar_active.getSize().y;
	percentage = (scrollbar_active.getPosition().y - scrollbar_background.getPosition().y) * 100.0 / (real_end - scrollbar_background.getPosition().y);

	int c_count = CONTENT::content->getLineCount();
	int l = int(c_count * percentage / 100) + 1;

	CONTENT::content->onScrollBar(l);
}
void Menu::scrollbar_move_to(sf::Vector2f mpos) {
	float y = mpos.y - scrollbar_background.getPosition().y;
	float y_max = scrollbar_background.getSize().y - scrollbar_active.getSize().y;
	if (y < 0) y = 0;
	if (y > y_max) y = y_max;
	float percentage = y / y_max;

	float count = CONTENT::content->getLineCount(), line = CONTENT::content->getCurrentLine();
	int page = count * percentage;

	CONTENT::content->onScrollBar(page + 1);
}
bool Menu::should_draw_scrollbar() {
	sf::Vector2f mpos = sf::Vector2f(sf::Mouse::getPosition(window));
	sf::FloatRect sb_bg = scrollbar_background.getGlobalBounds();
	sb_bg.left -= BAR::SCROLLBAR_PROXIMITY;
	sb_bg.width += 2 * BAR::SCROLLBAR_PROXIMITY;
	if (sb_bg.contains(mpos)) return 1;
	return 0;
}

void onFindFind() {
	BAR::events.push(BAR::FIND_FIND);
}
void onFindPrev() {
	BAR::events.push(BAR::FIND_PREV);
}
void onFindNext() {
	BAR::events.push(BAR::FIND_NEXT);
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
	text.setFont(BAR::font);
	text.setString("");
	onResize();

	changeTheme();
}
void FindPopUp::reset() {
	line = 0, occurence = 0;
	text.setString("");
	focused = 1;
}
void FindPopUp::focus() {
	focused = 1;
}
void FindPopUp::onKeyPress(sf::Uint32 code) {
	if (focused == 0) return;

	// 8 = Backspace
	// 13 = Enter
	if (code >= ' ' && code <= '~') { // Character
		text.setString(text.getString() + code);
		if (text.getPosition().x + text.getGlobalBounds().width >= TextBackground.getPosition().x + TextBackground.getSize().x) {
			code = 8; // backspace
		}
	}
	if (code == 8) { // back
		std::string str = text.getString();
		if (str.size() != 0)
			str.pop_back();
		text.setString(str);
	}
	if (code == 13) { // enter
		focused = 0;
		Find.onPress();
	}

}
void FindPopUp::draw(Button* hover) {
	if (focused == 1) TextBackground.setOutlineColor(BAR::FINDPOPUP_TEXT_OUTLINE_ACTIVE_COLOR);
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
	window.draw(text);
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
void FindPopUp::onResize() {
	float x_total_aprox = Find.getSize().x + Prev.getSize().x + Next.getSize().x + Cancel.getSize().x + 5 * BAR::spacing;

	background.setPosition({ window.getSize().x / 2 - x_total_aprox / 2, BAR::HEIGHT + 4 });
	background.setSize({ 100.0, 100.0 });
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
	TextBackground.setOutlineThickness(BAR::OUTLINE_THICKNESS);
	TextBackground.setFillColor(sf::Color::Transparent);
	background.setSize(sf::Vector2f(background.getSize().x, TextBackground.getPosition().y - background.getPosition().y + TextBackground.getSize().y + BAR::spacing));

	text.setCharacterSize(BAR::HEIGHT - 2 - BAR::spacing - 2);
	text.setPosition(TextBackground.getPosition() + sf::Vector2f(0.0f, -2.0f));
	//text.setString("");
}
void FindPopUp::changeTheme() {
	background.setFillColor(BAR::BG_COLOR);
	background.setOutlineColor(BAR::OUTLINE_COLOR);
	TextBackground.setOutlineColor(BAR::OUTLINE_COLOR);
	text.setFillColor(BAR::TEXT_COLOR);
	Find.changeTheme();
	Next.changeTheme();
	Prev.changeTheme();
	Cancel.changeTheme();
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
