#pragma once
#include <queue>

#include <SFML/Graphics.hpp>

#include "Windows.h"

extern sf::Font font;
extern sf::RenderWindow window;
extern sf::WindowHandle handle;

class Menu; // sorry not sorry vreau menu sa fie pus aici
namespace BAR {
	extern sf::Font font;
	extern const float HEIGHT;
	extern const sf::Color BG_COLOR;
	extern const sf::Color POPUP_COLOR;
	extern const sf::Color HOVER_COLOR;
	extern const sf::Color OUTLINE_COLOR;
	extern const float OUTLINE_THICKNESS;
	extern const sf::Color TEXT_COLOR;
	extern const float spacing;

	extern Menu *menu;

	extern bool SHOW_HITBOX; // todo remove before release

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
}