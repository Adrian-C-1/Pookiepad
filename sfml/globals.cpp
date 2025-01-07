#include "globals.h"
#include "menu.h"
#include <iostream>

sf::Font font;
sf::RenderWindow window(sf::VideoMode(1000, 700), "Pookiepad");
sf::WindowHandle handle;

namespace BAR {
	sf::Font font;

	float spacing = 4;
	float HEIGHT = 24.0f;
	float OUTLINE_THICKNESS = 1.0f;
	
	sf::Color BG_COLOR = sf::Color(40, 26, 38, 255);
	sf::Color TEXT_COLOR = sf::Color::White;
	sf::Color HOVER_COLOR = sf::Color(58, 44, 57, 255);
	sf::Color POPUP_COLOR = sf::Color(44, 44, 44, 255); // unused
	sf::Color OUTLINE_COLOR = sf::Color(70, 56, 68, 255);
	sf::Color NOTICE_COLOR = sf::Color::White;
	sf::Color SCROLLBAR_ACTIVE_COLOR = sf::Color::Black;
	sf::Color FINDPOPUP_TEXT_OUTLINE_ACTIVE_COLOR = sf::Color::White;
	sf::Color CURRENT_PAGE_TEXT_COLOR = sf::Color(255, 255, 255, 255);
	sf::Color OTHER_PAGE_TEXT_COLOR = sf::Color(120, 120, 120, 255);

	sf::Color SCROLLBAR_BG_COLOR = sf::Color(0, 0, 0, 44);
	int SCROLLBAR_PROXIMITY = 15;

	std::queue<EVENT> events;
	Menu *menu = nullptr;

	bool SHOW_HITBOX = 1;
	
	void changeTheme() {
		static bool black_theme = 0;
		black_theme = 1 - black_theme;

		if (black_theme) {
			BG_COLOR = sf::Color(40, 26, 38, 255);
			TEXT_COLOR = sf::Color::White;
			HOVER_COLOR = sf::Color(58, 44, 57, 255);
			OUTLINE_COLOR = sf::Color(70, 56, 68, 255);
			NOTICE_COLOR = sf::Color::White;
			SCROLLBAR_ACTIVE_COLOR = sf::Color::Black;
			FINDPOPUP_TEXT_OUTLINE_ACTIVE_COLOR = sf::Color::White;
			SCROLLBAR_BG_COLOR = sf::Color(0, 0, 0, 44);
			OTHER_PAGE_TEXT_COLOR = sf::Color::White;
			CURRENT_PAGE_TEXT_COLOR = sf::Color(120, 120, 120, 255);

			CONTENT::BG_COLOR = sf::Color(39, 39, 39, 255);
			CONTENT::TEXT_COLOR = sf::Color(255, 255, 255, 255);
			CONTENT::LINE_NR_COLOR = sf::Color(110, 110, 110, 255);
			CONTENT::SELECT_COLOR = sf::Color(100, 100, 100, 255);
		}
		else { // WHITE THEME
			BG_COLOR = sf::Color::White;
			TEXT_COLOR = sf::Color::Black;
			HOVER_COLOR = sf::Color(200, 200, 200, 255);
			OUTLINE_COLOR = sf::Color(70, 56, 68, 255);
			NOTICE_COLOR = sf::Color::Black;
			SCROLLBAR_BG_COLOR = sf::Color(200, 200, 200, 255);
			SCROLLBAR_ACTIVE_COLOR = sf::Color(0, 0, 0);
			FINDPOPUP_TEXT_OUTLINE_ACTIVE_COLOR = sf::Color::Black;
			CURRENT_PAGE_TEXT_COLOR = sf::Color(100, 150, 150, 255);
			OTHER_PAGE_TEXT_COLOR = sf::Color::Black;

			CONTENT::BG_COLOR = sf::Color::White;
			CONTENT::TEXT_COLOR = sf::Color::Black;
			CONTENT::LINE_NR_COLOR = sf::Color(110, 110, 110, 255);
			CONTENT::SELECT_COLOR = sf::Color(200, 240, 240, 255);
		}

	}

	std::string getClipboardText() {
		OpenClipboard(nullptr);
		HANDLE handle = GetClipboardData(CF_TEXT);
		if (handle == NULL) {
			menu->setNotice("Could not acces win32 API");
			return "";
		}
		char* text = static_cast<char*>(GlobalLock(handle)); // NU E OBIECTUL MEU ! NU STERGE !
		if (text == 0 || strlen(text) == 0) {
			menu->setNotice("Nothing in the clipboard");
			return "";
		}
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
			menu->setNotice("Could not acces win32 API");
			return 0;
		}
		char* p = static_cast<char*>(GlobalLock(handle));
		memcpy(p, text.c_str(), size);
		GlobalUnlock(handle);
		bool r = SetClipboardData(CF_TEXT, handle);
		if (r == 0) {
			GlobalFree(handle); // failed for whatever reason
			CloseClipboard();
			menu->setNotice("Text setting failed");
			return 0;
		}
		menu->setNotice("Text copied to clipboard");
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

	sf::Color BG_COLOR = sf::Color(39, 39, 39, 255);
	sf::Color TEXT_COLOR = sf::Color(255, 255, 255, 255);
	sf::Color LINE_NR_COLOR = sf::Color(110, 110, 110, 255);
	sf::Color SELECT_COLOR = sf::Color::Blue;
}