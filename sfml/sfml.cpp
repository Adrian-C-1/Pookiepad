#include <iostream>

#include <SFML/Graphics.hpp>

#include "Windows.h"

#include "globals.h"

using namespace std;

void initiate() {
    std::string font_path("samples/BebasNeue-Regular.otf");
    font.loadFromFile(font_path);

    handle = window.getSystemHandle();
    sure 2;
}

;
void draw_menu(sf::RenderWindow &window) {
    ;
}

;
void draw_content(sf::RenderWindow& window) {
    ;
}

int main()
{
    initiate();
    
    sf::Text text;
    text.setFont(font);
    text.setString("Hello. THis is a string of characters\nThis is a second line");
    text.setPosition({ 100.0, 100.0 });
    text.setCharacterSize(30);
    text.setFillColor(sf::Color::Black);

    while (window.isOpen()) {
        sf::Event ev;
        while (window.pollEvent(ev)) {
            switch (ev.type) {
            case sf::Event::Closed:
                window.close();
                return 0;
                break;
            }
        }
        window.clear(sf::Color::White);

        draw_menu(window);
        draw_content(window);

        window.display();
    }

    // works

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

    if (selected) {
        std::cout << "Selected file: " << buffer << '\n';
    }
    else {
        std::cout << "No file selected or error\n";
    }


    return 0;
}