#include "content.h"

Content::Content() {
	cursor.setSize(sf::Vector2f(12.f, 24.f));
	cursorState = false;
	numberCount = 1;
	offset = 0;
	date = std::time(0);
}