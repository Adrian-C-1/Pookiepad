#include <iostream>

#include <SFML/Graphics.hpp>

#include "Windows.h"

using namespace std;

int main()
{
    sf::RenderWindow window(sf::VideoMode(600, 600), "Title");

    sf::WindowHandle handle = window.getSystemHandle();
    
    sf::Font font;
    font.loadFromFile("Roboto-Black.ttf");

    sf::Text text;
    text.setFont(font);
    text.setString("Hello. THis is a string of characters\nThis is a second line");
    text.setPosition({ 100.0, 100.0 });

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

        window.draw(text);

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