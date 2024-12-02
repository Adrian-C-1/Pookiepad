#include "content.h"

Content::Content() {
    text.setFont(font);
    text.setCharacterSize(24);
    text.setPosition(sf::Vector2f(35.f, BAR::HEIGHT));
    numbers.setFont(font);
    numbers.setCharacterSize(24);
    numbers.setPosition(0.0f, BAR::HEIGHT);
    cursor.setSize(sf::Vector2f(12.f, 24.f));
    cursor.setFillColor(sf::Color::Black);
	cursorState = false;
	numberCount = 1;
	offset = 0;
	date = std::time(0);
	numbersString = "1\n";
    updateStrings();
    updateCursor();
}

sf::Text Content::getText() {
	return text;
}

sf::Text Content::getNumbers() {
	return numbers;
}

sf::RectangleShape Content::getCursor() {
	return cursor;
}

time_t Content::getDate() {
    return date;
}

void Content::updateStrings() {
	text.setString(textString);
	numbers.setString(numbersString);
}

void Content::updateCursor() {
	cursor.setPosition(sf::Vector2f(text.findCharacterPos(textString.size() - offset).x + 2.f, text.findCharacterPos(textString.size() - offset).y + 1.f));
}

void Content::addText(char character) {
	textString.insert(textString.size() - offset, 1, character);
    updateStrings();
	updateCursor();
}

void Content::addEnter() {
    textString.insert(textString.size() - offset, 1, '\n');
    numberCount++;
    numbersString += std::to_string(numberCount) + '\n';
    updateStrings();
    updateCursor();
}

void Content::setText(std::string str) {
    textString = str;
    offset = 0;
    resetNumbers();
    updateStrings();
    updateCursor();
}

void Content::resetNumbers() {
    numbersString = "";
    numberCount = 1;
    for (int i = 0; i < textString.size(); ++i) {
        if (textString[i] == '\n') {
            numberCount++;
        }
    }
    for (int i = 1; i <= numberCount; ++i) {
        numbersString += std::to_string(i) + '\n';
    }
    updateStrings();
    updateCursor();
}

void Content::removeChar(bool isCtrlPressed) {
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
                updateStrings();
                updateCursor();
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
                textString.erase(textString.size() - offset - 1, 1);
            }
            else {
                while (!textString.empty() && textString.size() - offset > 0 && (textString[textString.size() - offset - 1] != ' ' && textString[textString.size() - offset - 1] != '\n')) {
                    textString.erase(textString.size() - offset - 1, 1);
                }
            }
            updateStrings();
            updateCursor();
        }
    }
}

void Content::left(bool isCtrlPressed) {
    if (textString.size() - offset > 0) {
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
    }
    updateCursor();
}

void Content::right(bool isCtrlPressed) {
    if (offset > 0) {
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
    }
    updateCursor();
}

void Content::up() {
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
    updateCursor();
}

void Content::down() {
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
    updateCursor();
}

void Content::updateCursorBlink() {
    date = std::time(0);
    cursorState = !cursorState;
    cursor.setFillColor((cursorState ? sf::Color(206, 206, 206) : sf::Color::Black));
}

void Content::draw_content() {
    window.draw(cursor);
    window.draw(numbers);
    window.draw(text);
}

void Content::onKeyPress(sf::Uint32 code) {
    if (code >= ' ' && code <= '~') { // Character
        addText(code);
    }
    else {
        switch (code) {
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
}
void Content::onKeyPress(sf::Keyboard::Key key) {
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
        if ((sf::Keyboard::isKeyPressed(sf::Keyboard::LControl) || sf::Keyboard::isKeyPressed(sf::Keyboard::RControl))) {
            left(true);
        }
        else {
            left(false);
        }
    }
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
        if ((sf::Keyboard::isKeyPressed(sf::Keyboard::LControl) || sf::Keyboard::isKeyPressed(sf::Keyboard::RControl))) {
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