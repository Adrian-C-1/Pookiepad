#include "globals.h"

sf::Font font;
sf::RenderWindow window(sf::VideoMode(600, 600), "title");
sf::WindowHandle handle;

namespace BAR {
	const float HEIGHT = 20.0f;
	const sf::Color BG_COLOR = sf::Color::Black;
	const sf::Color POPUP_COLOR = sf::Color(45, 45, 45, 255);
	const sf::Color HOVER_COLOR = sf::Color(155, 155, 155, 255);
	const sf::Color TEXT_COLOR = sf::Color::White;
	std::queue<EVENT> events;
	Menu *menu = nullptr;

	bool SHOW_HITBOX = 0;
}