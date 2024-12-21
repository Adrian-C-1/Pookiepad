#include <iostream>

#include <SFML/Graphics.hpp>

#include "Windows.h"
#include "dwmapi.h"

#include "globals.h"
#include "menu.h"
#include "content.h"

void initiate() {
    std::string font_path("samples/OpenSans-Regular.ttf");
    BAR::font.loadFromFile(font_path);
    // Daca modifici fontul zi-mi ca trb sa modific si eu lucruri
    // ;

    font.loadFromFile(font_path);

    handle = window.getSystemHandle();
}



int main()
{
    initiate();

    //CONTENT::content = new Content();
    BAR::menu = new Menu(); // initialize it here not there

    if (CONTENT::content == nullptr) {
        std::cout << "Zii lui Adrian ca e prost daca ajungi aici\n";
        return 0;
    }

    while (window.isOpen()) {

        CONTENT::content->update();
        
        window.clear(/*sf::Color::White*/);

        CONTENT::content->draw_content();

        BAR::menu->draw(); // menu on top

        window.display();

        sf::Event event;
        while (window.pollEvent(event)) { // todo wait ev ADRIAN NU UITA (don't waste CPU !! )
            switch (event.type) {
            case sf::Event::Closed:
                BAR::events.push(BAR::CLOSE_ALL);
                break;
            case (sf::Event::Resized):
            {
                sf::FloatRect* view = new sf::FloatRect(0, 0, event.size.width, event.size.height);
                window.setView(sf::View(*view));
                CONTENT::content->updateResize();
                BAR::menu->onResize();
                break;
            }
            case sf::Event::MouseButtonPressed:
            {
                bool pressed_on_menu = BAR::menu->onPress();
                if (!pressed_on_menu) {
                    CONTENT::content->onMousePress();
                }
                break;
            }
            case sf::Event::MouseMoved:
                BAR::menu->onMouseMove();
                break;
            case sf::Event::TextEntered:
                BAR::menu->markChanged();
                CONTENT::content->onKeyPress(event.text.unicode);
                break;
            case sf::Event::KeyPressed:
                BAR::menu->markChanged();
                CONTENT::content->onKeyPress(event.key.code);
                break;
            case sf::Event::MouseWheelScrolled:
                if (event.mouseWheelScroll.delta > 0) { 
                    if (sf::Keyboard::isKeyPressed(sf::Keyboard::LControl) || sf::Keyboard::isKeyPressed(sf::Keyboard::RControl)) {
                        CONTENT::content->zoomIn();
                    }
                    else CONTENT::content->onKeyPress(sf::Keyboard::Up);
                }
                else {
                    if (sf::Keyboard::isKeyPressed(sf::Keyboard::LControl) || sf::Keyboard::isKeyPressed(sf::Keyboard::RControl)) {
                        CONTENT::content->zoomOut();
                    }
                    CONTENT::content->onKeyPress(sf::Keyboard::Down);
                }
                break;
            }
        }
    }

    return 0;
}