#include <iostream>

#include <SFML/Graphics.hpp>

#include "Windows.h"

#include "globals.h"
#include "menu.h"

void initiate() {
    std::string font_path("samples/OpenSans-Regular.ttf");
    font.loadFromFile(font_path);
    // Daca modifici fontul zi-mi ca trb sa modific si eu lucruri
    // ;

    handle = window.getSystemHandle();
}

;
void draw_content(sf::RenderWindow& window) {
    ;
}

int main()
{
    if (!window.isOpen()) {
        std::cout << "Error, could not open window or initialize SFML\n";
        return 0;
    }
    initiate();

    BAR::menu = new Menu(); // initialize it here not there

    bool letter_detection = 1;
    // exemplu la letter detection
    // trb doar sa vezi daca mausu pica intre 2 litere sau e pe o litera, f ez
    // vvvv
    sf::Text text;
    sf::RectangleShape rect;
    if (letter_detection) {
        float lineHeight;

        text.setFont(font);
        text.setCharacterSize(30);
        text.setLineSpacing(1);
        text.setString("qwertyuiopasdffghjklzxcvbnm|");
        lineHeight = text.getGlobalBounds().height;
        text.setString("Hello\nThis is another line of text\nByee\nn\nn\nn\nn\nn\nn\nn");

        sf::Vector2f textSize;
        textSize.x = text.getGlobalBounds().width;
        textSize.y = text.getGlobalBounds().height;

        rect.setPosition(text.getPosition());
        rect.setSize(textSize);
        rect.setFillColor(sf::Color::Red);
    }
    // ^^^^ 

    while (window.isOpen()) {
        sf::Event ev;
        while (window.pollEvent(ev)) { // todo wait ev ADRIAN NU UITA (don't waste CPU !! )
            switch (ev.type) {
            case sf::Event::Closed:
                window.close();
                return 0;
                break;
            case sf::Event::MouseButtonPressed:
                BAR::menu->onPress();
                break;
            case sf::Event::MouseMoved:
                BAR::menu->onMouseMove();
                break;
            }
        }
        window.clear(sf::Color::White);

        BAR::menu->draw();
        draw_content(window);

        // vvv si aici
        if (letter_detection == 1) {
            window.draw(rect);
            for (int i = 0; i < text.getString().getSize(); i++) {
                sf::FloatRect fr;
                sf::Vector2f pos = text.findCharacterPos(i);
                sf::Text t2 = text;
                t2.setString(std::string("") + char(text.getString()[i]));
                fr.width = t2.getGlobalBounds().width;
                fr.height = t2.getGlobalBounds().height;
                fr.left = pos.x;
                fr.top = pos.y;

                sf::RectangleShape r1;
                pos += sf::Vector2f(text.getLetterSpacing(), text.getLineSpacing());
                r1.setPosition(pos);
                fr.height *= 2;
                r1.setSize({ fr.width, fr.height });
                r1.setFillColor(sf::Color::Green);
                window.draw(r1);

                if (r1.getGlobalBounds().contains(sf::Vector2f(sf::Mouse::getPosition(window)))) {
                    std::cout << char(text.getString()[i]) << '\n';
                }

            }
            window.draw(text);
        }
        // ^^^

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