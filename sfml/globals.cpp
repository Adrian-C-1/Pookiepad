#include "globals.h"
#include <iostream>

sf::Font font;
sf::RenderWindow window(sf::VideoMode(1000, 700), "Pookiepad");
sf::WindowHandle handle;

namespace BAR {
	sf::Font font;
	const float HEIGHT = 24.0f;
	const sf::Color BG_COLOR = sf::Color(40, 26, 38, 255);
	const sf::Color POPUP_COLOR = sf::Color(44, 44, 44, 255); // unused
	const sf::Color OUTLINE_COLOR = sf::Color(70, 56, 68, 255);
	const float OUTLINE_THICKNESS = 1.0f;
	const sf::Color HOVER_COLOR = sf::Color(58, 44, 57, 255);
	const sf::Color SCROLLBAR_BG_COLOR = sf::Color(0, 0, 0, 44);
	const sf::Color TEXT_COLOR = sf::Color::White;
	const int SCROLLBAR_PROXIMITY = 15;
	const float spacing = 4;

	std::queue<EVENT> events;
	Menu *menu = nullptr;

	bool SHOW_HITBOX = 1;
	
	std::string getClipboardText() {
		OpenClipboard(nullptr);
		HANDLE handle = GetClipboardData(CF_TEXT);
		if (handle == NULL) return "";
		char* text = static_cast<char*>(GlobalLock(handle)); // NU E OBIECTUL MEU ! NU STERGE !
		if (text == 0 || strlen(text) == 0) return "";
		GlobalUnlock(handle);
		CloseClipboard();
		std::string str(text);
		return str;
	}
	bool setClipBoardText(const std::string& text) {
		OpenClipboard(nullptr);
		EmptyClipboard();
		int size = text.size() + 1; // null
		HGLOBAL handle = GlobalAlloc(GMEM_MOVEABLE, size);
		if (handle == 0) {
			CloseClipboard();
			return 0;
		}
		char* p = static_cast<char*>(GlobalLock(handle));
		memcpy(p, text.c_str(), size);
		GlobalUnlock(handle);
		bool r = SetClipboardData(CF_TEXT, handle);
		if (r == 0) {
			GlobalFree(handle); // failed for whatever reason
			CloseClipboard();
			return 0;
		}
		CloseClipboard();
		return 1;
	}
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

	const sf::Color BG_COLOR = sf::Color(39, 39, 39, 255);
	const sf::Color TEXT_COLOR = sf::Color(255, 255, 255, 255);
	const sf::Color LINE_NR_COLOR = sf::Color(110, 110, 110, 255);
}