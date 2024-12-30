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
    bool debug = true;
    initiate();

    //CONTENT::content = new Content();
    BAR::menu = new Menu(); // initialize it here not there

    if (CONTENT::content == nullptr) {
        std::cout << "Zii lui Adrian ca e prost daca ajungi aici\n";
        return 0;
    }

    while (window.isOpen()) {

        CONTENT::content->updateBlinker();
        
        window.clear(CONTENT::BG_COLOR);

        CONTENT::content->draw_content();

        BAR::menu->draw(); // menu on top

        window.display();

        sf::Event event;
        while (window.pollEvent(event)) { // todo wait ev ADRIAN NU UITA (don't waste CPU !! )
            switch (event.type) {
            case sf::Event::Closed:
            {
                if (debug) {
                    window.close();
                    return 0;
                }
                else {
                    BAR::events.push(BAR::CLOSE_ALL);
                    break;
                }
            }
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
                //std::cout << "Text entered before\n";
                BAR::menu->markChanged();
                if (!BAR::menu->onTextEntered(event.text.unicode))
                    CONTENT::content->onKeyPress(event.text.unicode);
                //std::cout << "Text entered after\n";
                BAR::menu->scrollbar_make_good();

                break;
            case sf::Event::KeyPressed:
                //std::cout << "Key pressed before\n";
                BAR::menu->markChanged();
                if (!BAR::menu->onKeyPress())
                    CONTENT::content->onKeyPress(event.key.code);
                //std::cout << "Key pressed after\n";
                BAR::menu->scrollbar_make_good();


                break;
            case sf::Event::MouseWheelScrolled:
                if (event.mouseWheelScroll.delta > 0) { 
                    if (sf::Keyboard::isKeyPressed(sf::Keyboard::LControl) || sf::Keyboard::isKeyPressed(sf::Keyboard::RControl)) {
                        CONTENT::content->zoomIn();
                    }
                    else CONTENT::content->scroll(true);
                }
                else {
                    if (sf::Keyboard::isKeyPressed(sf::Keyboard::LControl) || sf::Keyboard::isKeyPressed(sf::Keyboard::RControl)) {
                        CONTENT::content->zoomOut();
                    }
                    else CONTENT::content->scroll(false);
                }
                BAR::menu->scrollbar_make_good();
                break;
            }
        }
    }

    return 0;
}