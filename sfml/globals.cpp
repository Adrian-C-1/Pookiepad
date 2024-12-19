#include "globals.h"
#include <iostream>

sf::Font font;
sf::RenderWindow window(sf::VideoMode(600, 600), "Pookiepad");
sf::WindowHandle handle;

namespace BAR {
	sf::Font font;
	const float HEIGHT = 24.0f;
	const sf::Color BG_COLOR = sf::Color(40, 26, 38, 255);
	const sf::Color POPUP_COLOR = sf::Color(44, 44, 44, 255); // unused
	const sf::Color OUTLINE_COLOR = sf::Color(70, 56, 68, 255);
	const float OUTLINE_THICKNESS = 1.0f;
	const sf::Color HOVER_COLOR = sf::Color(58, 44, 57, 255);
	const sf::Color TEXT_COLOR = sf::Color::White;
	const float spacing = 4;

	std::queue<EVENT> events;
	Menu *menu = nullptr;

	bool SHOW_HITBOX = 1;

	std::string getFileFromFilepath(std::string path) {
		std::string file = "";
		int i = path.size() - 1;
		while (i >= 0 && path[i] != '\\') i--;
		i++;
		while (i < path.size()) file += path[i++];
		return file;
	}
}

namespace CONTENT {
	Content* content = nullptr;
}