#include <iostream>

#include <SFML/Graphics.hpp>

#include "Windows.h"

#include "globals.h"
#include "menu.h"
#include "content.h"

void initiate() {
    std::string font_path("samples/OpenSans-Regular.ttf");
    font.loadFromFile(font_path);
    // Daca modifici fontul zi-mi ca trb sa modific si eu lucruri
    // ;

    handle = window.getSystemHandle();
}

int main()
{
    if (!window.isOpen()) {
        std::cout << "Error, could not open window or initialize SFML\n";
        return 0;
    }

    initiate();

    CONTENT::content = new Content();
    BAR::menu = new Menu(); // initialize it here not there

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) { // todo wait ev ADRIAN NU UITA (don't waste CPU !! )
            switch (event.type) {
            case sf::Event::Closed:
                window.close();
                return 0;
                break;
            case (sf::Event::Resized):
            {
                sf::FloatRect* view = new sf::FloatRect(0, 0, event.size.width, event.size.height);
                window.setView(sf::View(*view));
                CONTENT::content->updateResize();
                break;
            }
            case sf::Event::MouseButtonPressed:
            {
                bool pressed_on_menu = BAR::menu->onPress();
                if (!pressed_on_menu)
                    CONTENT::content->onMousePress();
                break;
            }
            case sf::Event::MouseMoved:
                BAR::menu->onMouseMove();
                break;
            case (sf::Event::TextEntered):
                CONTENT::content->onKeyPress(event.text.unicode);
            case (sf::Event::KeyPressed):
                CONTENT::content->onKeyPress(event.key.code);
                break;
            }
        }

        CONTENT::content->update();

        window.clear(/*sf::Color::White*/);

        CONTENT::content->draw_content();
        BAR::menu->draw(); // menu on top

        window.display();
    }

    return 0;
}