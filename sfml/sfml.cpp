#include <iostream>

#include <SFML/Graphics.hpp>

#include "Windows.h"

#include "globals.h"
#include "menu.h"
#include "content.h"

/* Modificarile mele */
sf::Text text;
std::string textString;
sf::Text numbers;
std::string numbersString;

sf::RectangleShape cursor(sf::Vector2f(12.f, 24.f));
bool cursorState = false;
int numberCount = 1;
int offset = 0;
time_t date = std::time(0);
/* Sfarsitul modificarilor mele */

void initiate() {
    std::string font_path("samples/OpenSans-Regular.ttf");
    font.loadFromFile(font_path);
    // Daca modifici fontul zi-mi ca trb sa modific si eu lucruri
    // ;

    /* Modificarile mele */
    text.setFont(font);
    text.setCharacterSize(24);
    text.setPosition(sf::Vector2f(35.f, 0.f)); // De modificat dimensiunile de aici pentru a marca de unde incepe "casuta" cu text in program

    numbers.setFont(font);
    numbers.setCharacterSize(24);
    numbersString = "1\n";
    numbers.setString(numbersString);

    cursor.setFillColor(sf::Color::Black);
    cursor.setPosition(sf::Vector2f(text.findCharacterPos(textString.size()).x + 2.f, text.findCharacterPos(textString.size()).y + 1.f));
    /* Sfarsitul modificarilor mele */

    handle = window.getSystemHandle();
}

/* Modificarile mele */

void addText(char character) {
    textString.insert(textString.size() - offset, 1, character);
    text.setString(textString);
    cursor.setPosition(sf::Vector2f(text.findCharacterPos(textString.size() - offset).x + 2.f, text.findCharacterPos(textString.size() - offset).y + 1.f));
}

void addEnter() {
    textString.insert(textString.size() - offset, 1, '\n');
    numberCount++;
    numbersString += std::to_string(numberCount) + '\n';
    text.setString(textString);
    numbers.setString(numbersString);
    cursor.setPosition(sf::Vector2f(text.findCharacterPos(textString.size() - offset).x + 2.f, text.findCharacterPos(textString.size() - offset).y + 1.f));
}

void removeChar(bool isCtrlPressed) {
    if (!textString.empty()) {
        if (!isCtrlPressed) {
            if (textString.size() - offset > 0) {
                if (textString[textString.size() - offset - 1] == '\n') {
                    numberCount--;
                    numbersString.pop_back(); // stergerea \n-ului
                    while (!numbersString.empty() && numbersString.back() != '\n') { // stergerea cifrelor
                        numbersString.pop_back();
                    }
                    numbers.setString(numbersString);
                }
                textString.erase(textString.size() - offset - 1, 1);
                text.setString(textString);
                cursor.setPosition(sf::Vector2f(text.findCharacterPos(textString.size() - offset).x + 2.f, text.findCharacterPos(textString.size() - offset).y + 1.f));
            }
        }
        else {
            if (textString[textString.size() - offset - 1] == ' ') {
                textString.erase(textString.size() - offset - 1, 1);
            }
            else if (textString[textString.size() - offset - 1] == '\n') {
                numberCount--;
                numbersString.pop_back(); // stergerea \n-ului
                while (!numbersString.empty() && numbersString.back() != '\n') { // stergerea cifrelor
                    numbersString.pop_back();
                }
                numbers.setString(numbersString);
                textString.erase(textString.size() - offset - 1, 1);
            }
            else {
                while (!textString.empty() && textString.size() - offset > 0 && (textString[textString.size() - offset - 1] != ' ' && textString[textString.size() - offset - 1] != '\n')) {
                    textString.erase(textString.size() - offset - 1, 1);
                }
            }
            text.setString(textString);
            cursor.setPosition(sf::Vector2f(text.findCharacterPos(textString.size() - offset).x + 2.f, text.findCharacterPos(textString.size() - offset).y + 1.f));
        }
    }
}

void left(bool isCtrlPressed) {
    if (!isCtrlPressed) {
        if (offset < textString.size()) {
            offset++;
        }
    }
    else {
        if (textString[textString.size() - offset - 1] == ' ' || textString[textString.size() - offset - 1] == '\n') {
            offset++;
        }
        else {
            while (!textString.empty() && textString.size() - offset > 0 && (textString[textString.size() - offset - 1] != ' ' && textString[textString.size() - offset - 1] != '\n')) {
                offset++;
            }
        }
    }
    cursor.setPosition(sf::Vector2f(text.findCharacterPos(textString.size() - offset).x + 2.f, text.findCharacterPos(textString.size() - offset).y + 1.f));
}

void right(bool isCtrlPressed) {
    if (!isCtrlPressed) {
        if (offset > 0) {
            offset--;
        }
    }
    else {
        if (textString[textString.size() - offset] == ' ' || textString[textString.size() - offset] == '\n') {
            offset--;
        }
        else {
            while (offset > 0 && (textString[textString.size() - offset] != ' ' && textString[textString.size() - offset] != '\n')) {
                offset--;
            }
        }
    }
    cursor.setPosition(sf::Vector2f(text.findCharacterPos(textString.size() - offset).x + 2.f, text.findCharacterPos(textString.size() - offset).y + 1.f));
}

void up() {
    int currlineoffset = 0; // offset fata de stanga
    while (textString.size() - offset - currlineoffset > 0 && (textString[textString.size() - offset - currlineoffset - 1] != '\n')) { // calcularea offset-ului
        currlineoffset++;
    }
    while (!textString.empty() && textString.size() - offset > 0 && textString[textString.size() - offset - 1] != '\n') { // aducerea la primul caracter de pe linia veche
        offset++;
    }
    if (textString.size() - offset > 0) offset++; // aducerea la ultimul caracter de pe linia noua
    int nextlinesize = 0;
    while (!textString.empty() && textString.size() - offset > 0 && textString[textString.size() - offset - 1] != '\n') { // aducerea la primul caracter de pe linia noua
        offset++;
        nextlinesize++;
    }
    if (currlineoffset <= nextlinesize) offset -= currlineoffset;
    else {
        offset -= nextlinesize;
    }
    cursor.setPosition(sf::Vector2f(text.findCharacterPos(textString.size() - offset).x + 2.f, text.findCharacterPos(textString.size() - offset).y + 1.f));

}

void down() {
    int currlineoffset = 0; // offset fata de stanga again
    while (textString.size() - offset - currlineoffset > 0 && (textString[textString.size() - offset - currlineoffset - 1] != '\n')) { // calcularea offset-ului
        currlineoffset++;
    }
    while (offset > 0 && textString[textString.size() - offset] != '\n') { // aducerea la finalul liniei vechi
        offset--;
    }
    if (offset > 0) offset--; // aducerea la inceputul noii linii
    int nextlinesize = 0;
    while (offset > 0 && currlineoffset > 0 && textString[textString.size() - offset] != '\n') { // ori ajunge la capat, ori ajunge la offset-ul de pe linia anterioara
        offset--;
        currlineoffset--;
    }
    cursor.setPosition(sf::Vector2f(text.findCharacterPos(textString.size() - offset).x + 2.f, text.findCharacterPos(textString.size() - offset).y + 1.f));
}

void updateCursorBlink() {
    date = std::time(0);
    cursorState = !cursorState;
    cursor.setFillColor((cursorState ? sf::Color(206, 206, 206) : sf::Color::Black));
}

/* Sfarsitul modificarilor mele */

void draw_content(sf::RenderWindow& window) {
    /* Modificarile mele */
    window.draw(cursor);
    window.draw(numbers);
    window.draw(text);
    /* Sfarsitul modificarilor mele */
}

int main()
{
    if (!window.isOpen()) {
        std::cout << "Error, could not open window or initialize SFML\n";
        return 0;
    }
    initiate();

    BAR::menu = new Menu(); // initialize it here not there
    Content content;

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
        sf::Event event;
        while (window.pollEvent(event)) { // todo wait ev ADRIAN NU UITA (don't waste CPU !! )
            switch (event.type) {
            case sf::Event::Closed:
                window.close();
                return 0;
                break;
            /* Modificarile mele */
            case (sf::Event::Resized):
                //sf::FloatRect* view = new sf::FloatRect(0, 0, event.size.width, event.size.height);
                //window.setView(sf::View(*view));
                break;
            /* Sfarsitul modificarilor mele */
            case sf::Event::MouseButtonPressed:
                BAR::menu->onPress();
                break;
            case sf::Event::MouseMoved:
                BAR::menu->onMouseMove();
                break;
            /* Modificarile mele */
            case (sf::Event::TextEntered):
                if (event.text.unicode >= ' ' && event.text.unicode <= '~') { // Character
                    addText(event.text.unicode);
                }
                else {
                    switch (event.text.unicode) {
                    case (13): // 13 = Enter
                        addEnter();
                        break;
                    case (9): // 9 = Tab
                        for (int i = 0; i < 6; ++i) {
                            addText(' ');
                        }
                        break;
                    case (8): // 8 = Backspace
                        removeChar(false);
                        break;
                    case (127): // 127 = Ctrl - Backspace
                        removeChar(true);
                        break;
                    }
                }
            case (sf::Event::KeyPressed):
                if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
                    if ((sf::Keyboard::isKeyPressed(sf::Keyboard::LControl) || sf::Keyboard::isKeyPressed(sf::Keyboard::RControl)) && textString.size() - offset > 0) {
                        left(true);
                    }
                    else {
                        left(false);
                    }
                }
                else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
                    if ((sf::Keyboard::isKeyPressed(sf::Keyboard::LControl) || sf::Keyboard::isKeyPressed(sf::Keyboard::RControl)) && offset > 0) {
                        right(true);
                    }
                    else {
                        right(false);

                    }
                }
                else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) {
                    up();
                }
                else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) {
                    down();
                }
            }
            /* Sfarsitul modificarilor mele */
        }

        /* Modificarile mele */
        if (std::time(0) > date) { // efectul de "blink" al cursorului
            updateCursorBlink();
        }
        /* Sfarsitul modificarilor mele */

        window.clear(/*sf::Color::White*/);

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