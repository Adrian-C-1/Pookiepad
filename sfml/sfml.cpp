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



int WINAPI WinMain(HINSTANCE hThisInstance, HINSTANCE hPrevInstance, LPSTR lpszArgument, int nCmdShow)
{
    bool debug = false;
    initiate();

    //BAR::changeTheme(); // light theme ON
    BAR::menu = new Menu(); // initialize it here not there
    if (CONTENT::content == nullptr) {
        std::cout << "Menu nu merge? \n";
        return 0;
    }


    window.setFramerateLimit(100);
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
                CONTENT::content->onMouseMove();
                break;
            case sf::Event::TextEntered:
                BAR::menu->markChanged();
                if (!BAR::menu->onTextEntered(event.text.unicode))
                    CONTENT::content->onKeyPress(event.text.unicode);
                BAR::menu->scrollbar_make_good();

                break;
            case sf::Event::KeyPressed:
                BAR::menu->markChanged();
                if (!BAR::menu->onKeyPress())
                    CONTENT::content->onKeyPress(event.key.code);
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