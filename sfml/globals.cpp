#include "globals.h"

sf::Font font;
sf::RenderWindow window(sf::VideoMode(600, 600), "Pookiepad");
sf::WindowHandle handle;

namespace BAR {
	sf::Font font;
	const float HEIGHT = 24.0f;
	const sf::Color BG_COLOR = sf::Color::Black;
	const sf::Color POPUP_COLOR = sf::Color(45, 45, 45, 255);
	const sf::Color HOVER_COLOR = sf::Color(95, 95, 95, 255);
	const sf::Color TEXT_COLOR = sf::Color::White;
	const float spacing = 4;

	std::queue<EVENT> events;
	Menu *menu = nullptr;

	bool SHOW_HITBOX = 1;
}

namespace CONTENT {
	Content* content = nullptr;
}