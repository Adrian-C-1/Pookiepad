#pragma once
#include <queue>

#include <SFML/Graphics.hpp>

#include "Windows.h"

extern sf::Font font;
extern sf::RenderWindow window;
extern sf::WindowHandle handle;

class Menu; 
namespace BAR {
	extern sf::Font font;

	extern float spacing;
	extern float HEIGHT;
	extern float OUTLINE_THICKNESS;
	extern sf::Color BG_COLOR;
	extern sf::Color POPUP_COLOR;
	extern sf::Color HOVER_COLOR;
	extern sf::Color OUTLINE_COLOR;
	extern sf::Color TEXT_COLOR;
	extern sf::Color NOTICE_COLOR;
	extern sf::Color SCROLLBAR_ACTIVE_COLOR;
	extern sf::Color FINDPOPUP_TEXT_OUTLINE_ACTIVE_COLOR;
	extern sf::Color CURRENT_PAGE_TEXT_COLOR;
	extern sf::Color OTHER_PAGE_TEXT_COLOR;

	void changeTheme();

	extern int SCROLLBAR_PROXIMITY;
	extern sf::Color SCROLLBAR_BG_COLOR;

	extern Menu *menu;

	enum EVENT {
	SHOULD_CLOSE_POPUP,
	OPEN_FILE_POPUP,
	OPEN_EDIT_POPUP,
	OPEN_VIEW_POPUP,
	OPEN_FIND_POPUP,
	NEW_FILE,
	SAVE_FILE,
	CLOSE_FILE,
	OPEN_FILE,
	CLOSE_ALL,
	FIND_FIND,
	FIND_PREV,
	FIND_NEXT,
	CHANGED_THEME
	};
	extern std::queue<EVENT> events;
	
	/// returns 1 on succes, 0 on fail
	bool setClipBoardText(const std::string& text);
	std::string getClipboardText();
	std::string getFileFromFilepath(std::string path);
}

class Content;
namespace CONTENT {
	extern Content* content;

	extern sf::Color BG_COLOR;
	extern sf::Color TEXT_COLOR;
	extern sf::Color LINE_NR_COLOR;
	extern sf::Color SELECT_COLOR;
}