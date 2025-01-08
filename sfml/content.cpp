#include "content.h"
#include "menu.h"
#include "globals.h"

Content::Content() {
    root = nullptr;
    init();
}
Content::Content(std::string str)
{
    if (str.size() == 0)
        root = nullptr;
    else
    {
        root = new nod;
        createRope(str, 0, str.size() - 1, root, nullptr);
    }
    init();
    text.setString(composeStrings());
    changeTheme();
}
void Content::destroyNode(nod* c) {
	if (c == nullptr) return;
	destroyNode(c->l);
	destroyNode(c->r);
	delete c;
}
void Content::init() {
    text.setFont(font);
    text.setFillColor(CONTENT::TEXT_COLOR);
    zoomstates = { 12, 18, 24, 30, 36, 42, 48 }; // min = 0 , max = 6 | 24 = 100% , 48 = 200% | 50% - 200%
    zoompercentages = { 50, 75, 100, 125, 150, 175, 200 };
    state = 2;
    text.setCharacterSize(zoomstates[state]);
    numbers.setFont(font);
    numbers.setCharacterSize(zoomstates[state]);
    numbers.setFillColor(CONTENT::LINE_NR_COLOR);
    numberRectangle.setFillColor(CONTENT::BG_COLOR);
    cursor.setSize(sf::Vector2f(2, (float)zoomstates[state]));
    cursor.setFillColor(CONTENT::BG_COLOR);
    cursorState = false;
    date = std::time(0);
    
    offset = 0;

    currChar = 0;
    currLine = 0;
    currFrame = 0;

    showLines = 1;
    updateResize();
    text.setPosition(sf::Vector2f(leftsize, BAR::HEIGHT));
}


void Content::onKeyPress(sf::Uint32 code) {
    /*
    if (code == 'z') { // debug purposes
        std::cout << "\n\n";
        std::cout << "Lines: " << lines() << '\n';
        std::cout << "Current line: " << currLine << '\n';
        std::cout << "Current character: " << currChar << '\n';
        std::cout << "Current & Different frame: " << currFrame << " " << diffFrame << '\n';
        std::cout << "Is frame moved?: " << isFrameMoved << '\n';
        std::cout << "Number of sentences: " << propcount << '\n';
        std::cout << "Lower frame bound: " << getLowerBoundFrame() << '\n';
        std::cout << "Upper frame bound: " << getUpperBoundFrame() << '\n';
        std::cout << "Selected: " << selected << '\n';
        std::cout << "Left pos: " << selectXleft << " " << selectYleft << '\n';
        std::cout << "Right pos: " << selectXright<< " " << selectYright<< '\n';
        std::cout << "Starting pos: " << startingPosX << " " << startingPosY << '\n';
        std::cout << "\n\n";
        return;
    }
    */
    if (code >= ' ' && code <= '~') { // Character
        if (isFrameMoved) {
            diffFrame = currFrame;
            isFrameMoved = false;
        }
        if (selected) {
            BIGERASE();
            removeSelection();
        }
        insert(getPhrasePosition(currLine) + currChar, code);       
        currChar++;
        text.setString(composeStrings());
        updateNumbers();
        updateCursor();
    }
    else {
        switch (code) {
            case (13): // 13 = Enter
            {
                if (isFrameMoved) {
                    diffFrame = currFrame;
                    isFrameMoved = false;
                }
                if (selected) {
                    BIGERASE();
                    removeSelection();
                }
                insert(getPhrasePosition(currLine) + currChar, '\n');
                currChar = 0;
                currLine++;
                if (currLine > getUpperBoundFrame()) {
                    currFrame++;
                    diffFrame = currFrame;
                }
                text.setString(composeStrings());
                updateNumbers();
                updateCursor();
                break;
            }
            case (9): // 9 = Tab
            {
                if (isFrameMoved) {
                    diffFrame = currFrame;
                    isFrameMoved = false;
                }
                if (selected) {
                    BIGERASE();
                    removeSelection();
                }
                std::string spaces = "      ";
                insert(getPhrasePosition(currLine) + currChar, spaces);
                currChar += 6;
                text.setString(composeStrings());
                updateCursor();
                break;
            }
            case (8): // 8 = Backspace
            {
                if (isFrameMoved) {
                    diffFrame = currFrame;
                    isFrameMoved = false;
                }
                if (selected) {
                    BIGERASE();
                    removeSelection();
                    break;
                }
                if (currChar <= 0 && currLine <= 0) return; // <= similar cu ==, doar de siguranta
                int pos = getPhrasePosition(currLine) + currChar - 1;
                if (at(pos) == '\n') {
                    currChar = getLineLength(currLine - 1) - 1;
                    currLine--;
                    if (currLine < getLowerBoundFrame() && currFrame > 0) {
                        currFrame--;
                        diffFrame = currFrame;
                    }
                    //updateNumbers();
                }
                else currChar--;
                erase(pos);
                text.setString(composeStrings());
                updateNumbers();
                updateCursor();
                break;
            }
            case (127): // 127 = Ctrl - Backspace
            {
                if (isFrameMoved) {
                    diffFrame = currFrame;
                    isFrameMoved = false;
                }
                if (selected) {
                    BIGERASE();
                    removeSelection();
                    break;
                }
                if (currChar <= 0 && currLine <= 0) return; // <= similar cu ==, doar de siguranta
                int pos = getPhrasePosition(currLine) + currChar - 1;
                char ch = at(pos);
                if (ch == ' ') {
                    currChar--;
                    erase(pos);
                }
                else if (ch == '\n') {
                    currChar = getLineLength(currLine - 1) - 1;
                    currLine--;
                    if (currLine < getLowerBoundFrame() && currFrame > 0) {
                        currFrame--;
                        diffFrame = currFrame;
                    }
                    erase(pos);
                }
                else {
                    while (currChar > 1 && (ch != ' ' && ch != '\n')) {
                        currChar--;
                        erase(pos);
                        pos--;
                        ch = at(pos);
                    }
                    if (ch != ' ' && ch != '\n') {
                        currChar--;
                        erase(pos);
                    }
                }
                text.setString(composeStrings());
                updateNumbers();
                updateCursor();
                break;
            }
        }
    }
    cursorState = false;
}
void Content::onKeyPress(sf::Keyboard::Key key) {
    //std::cout << key << '\n';
    if (key == sf::Keyboard::Left) {
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::LControl) && sf::Keyboard::isKeyPressed(sf::Keyboard::LShift)) {
            select(0, true);
        }
        else if ((sf::Keyboard::isKeyPressed(sf::Keyboard::LControl) || sf::Keyboard::isKeyPressed(sf::Keyboard::RControl))) {
            left(true, true);
        }
        else if ((sf::Keyboard::isKeyPressed(sf::Keyboard::LShift)) || (sf::Keyboard::isKeyPressed(sf::Keyboard::RShift))) {
            select(0, false);
        }
        else {
            left(false, true);
        }
    }
    else if (key == sf::Keyboard::Right) {
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::LControl) && sf::Keyboard::isKeyPressed(sf::Keyboard::LShift)) {
            select(1, true);
        }
        else if ((sf::Keyboard::isKeyPressed(sf::Keyboard::LControl) || sf::Keyboard::isKeyPressed(sf::Keyboard::RControl))) {
            right(true, true);
        }
        else if ((sf::Keyboard::isKeyPressed(sf::Keyboard::LShift)) || (sf::Keyboard::isKeyPressed(sf::Keyboard::RShift))) {
            select(1, false);
        }
        else {
            right(false, true);

        }
    }
    else if (key == sf::Keyboard::Up) {
        if ((sf::Keyboard::isKeyPressed(sf::Keyboard::LShift)) || (sf::Keyboard::isKeyPressed(sf::Keyboard::RShift))) {
            select(2, false);
        }
        else {
            up(true);
        }
    }
    else if (key == sf::Keyboard::Down) {
        if ((sf::Keyboard::isKeyPressed(sf::Keyboard::LShift)) || (sf::Keyboard::isKeyPressed(sf::Keyboard::RShift))) {
            select(3, false);
        }
        else {
            down(true);
        }
    }
    else if (key == sf::Keyboard::Equal && (sf::Keyboard::isKeyPressed(sf::Keyboard::LControl) || sf::Keyboard::isKeyPressed(sf::Keyboard::RControl))) { // Ctrl + '+'
        zoomIn();
    }
    else if (key == sf::Keyboard::Dash && (sf::Keyboard::isKeyPressed(sf::Keyboard::LControl) || sf::Keyboard::isKeyPressed(sf::Keyboard::RControl))) { // Ctrl + '-'
        zoomOut();
    }
    else if (key == sf::Keyboard::X && (sf::Keyboard::isKeyPressed(sf::Keyboard::LControl) || sf::Keyboard::isKeyPressed(sf::Keyboard::RControl))) { // Ctrl + 'x'
        copy(true);
    }
    else if (key == sf::Keyboard::C && (sf::Keyboard::isKeyPressed(sf::Keyboard::LControl) || sf::Keyboard::isKeyPressed(sf::Keyboard::RControl))) { // Ctrl + 'c'
        copy(false);
    }
    else if (key == sf::Keyboard::V && (sf::Keyboard::isKeyPressed(sf::Keyboard::LControl) || sf::Keyboard::isKeyPressed(sf::Keyboard::RControl))) { // Ctrl + 'v'
        paste();
    }
    else if (key == sf::Keyboard::A && (sf::Keyboard::isKeyPressed(sf::Keyboard::LControl) || sf::Keyboard::isKeyPressed(sf::Keyboard::RControl))) { // Ctrl + 'a'
        select(4, false);
    }
    else if (key == sf::Keyboard::F && (sf::Keyboard::isKeyPressed(sf::Keyboard::LControl) || sf::Keyboard::isKeyPressed(sf::Keyboard::RControl))) { // Ctrl + 'f'
        BAR::events.push(BAR::OPEN_FIND_POPUP);
    }
    else if (key == sf::Keyboard::S && (sf::Keyboard::isKeyPressed(sf::Keyboard::LControl) || sf::Keyboard::isKeyPressed(sf::Keyboard::RControl))) {
        BAR::events.push(BAR::SAVE_FILE);
    }
    else if (key == sf::Keyboard::N && (sf::Keyboard::isKeyPressed(sf::Keyboard::LControl) || sf::Keyboard::isKeyPressed(sf::Keyboard::RControl))) {
        BAR::events.push(BAR::NEW_FILE);
    }
    else if (key == sf::Keyboard::O && (sf::Keyboard::isKeyPressed(sf::Keyboard::LControl) || sf::Keyboard::isKeyPressed(sf::Keyboard::RControl))) {
        BAR::events.push(BAR::OPEN_FILE);
    }
    else if (key == sf::Keyboard::W && (sf::Keyboard::isKeyPressed(sf::Keyboard::LControl) || sf::Keyboard::isKeyPressed(sf::Keyboard::RControl))) {
        BAR::events.push(BAR::CLOSE_FILE);
    }
    else if (key == sf::Keyboard::Delete) {
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::LControl) || sf::Keyboard::isKeyPressed(sf::Keyboard::RControl)) {
            deleteBtn(true);
        }
        else {
            deleteBtn(false);
        }
    }
}

void Content::onSelectText(sf::Vector2f mpos) {
    int oldLine = currLine, oldChar = currChar;
    moveCursor(mpos);
    if (selected) {
        if (currLine == startingPosY && currChar == startingPosX) {
            removeSelection();
            return;
        }
        else {
            if (currLine < startingPosY || currLine == startingPosY && currChar < startingPosX) {
                selectXleft = currChar, selectYleft = currLine;
                selectXright = startingPosX, selectYright = startingPosY;
            }
            else {
                selectXleft = startingPosX, selectYleft = startingPosY;
                selectXright = currChar, selectYright = currLine;
            }
        }
    }
    else {
        if (currLine == oldLine && currChar == oldChar) {
            removeSelection();
            return;
        }
        if (currLine < oldLine || currLine == oldLine && currChar < oldChar) {
            selected = true;
            selectXleft = currChar, selectYleft = currLine;
            selectXright = oldChar, selectYright = oldLine;
            startingPosX = selectXright, startingPosY = selectYright;
        }
        else {
            selected = true;
            selectXleft = oldChar, selectYleft = oldLine;
            selectXright = currChar, selectYright = currLine;
            startingPosX = selectXleft, startingPosY = selectYleft;
        }
    }
}
bool holding = 0;
void Content::onMousePress() {
    if (root == nullptr) return;
    sf::Vector2f mpos = sf::Vector2f(sf::Mouse::getPosition(window));
    if (mpos.x <= text.getGlobalBounds().left || mpos.y <= text.getGlobalBounds().top) return;

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::LShift)) {
        onSelectText(mpos);
    }
    else {
        if (selected) removeSelection();
        moveCursor(mpos);
        holding = 1;
    }
    cursorState = false;
    text.setString(composeStrings());
    updateNumbers();
    updateCursor();
}
void Content::onMouseMove() {
    if (holding == 1) {
        if (!sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
            holding = 0;
            return;
        }
        sf::Vector2f mpos = sf::Vector2f(sf::Mouse::getPosition(window));
        onSelectText(mpos);

        if (currLine == getUpperBoundFrame())
            scroll(false);
        if (currLine <= getLowerBoundFrame())
            scroll(true);

        cursorState = false;
        text.setString(composeStrings());
        updateNumbers();
        updateCursor();
    }
}
void Content::onScrollBar(int line) {
    line = std::max(std::min(line, lines()), 1);
    if (lines() < propcount) line = 1;
    diffFrame = line - 1;
    isFrameMoved = 1;
    text.setString(composeStrings());
    updateNumbers();
    int oldl = currLine;
    currLine = diffFrame;
    updateCursor();
    currLine = oldl;
    return;

    /*
        TODO vezi cum faci aici sa mearga
    */
    // Eu am scris-o da nush exact ce iti mai trb updateuri si alte chestii
    line = std::max(std::min(line, lines()), 1);
    if (lines() < propcount) line = 1;
    line = line - 1;
    if (line < diffFrame && diffFrame > 0) {
        diffFrame--;
        if (currLine >= getLowerBoundFrame() && currLine <= getUpperBoundFrame()) currFrame--;
        else isFrameMoved = true;
    }
    else if (line > diffFrame && diffFrame <= lines() - propcount) {
        diffFrame++;
        if (currLine >= getLowerBoundFrame() && currLine <= getUpperBoundFrame()) currFrame++;
        else isFrameMoved = true;
    }
    text.setString(composeStrings());
    updateNumbers();
    updateCursor();
}



void Content::moveCursor(sf::Vector2f mpos) {
    if (mpos.y > text.getLocalBounds().height + text.getLocalBounds().top) {
        currFrame = diffFrame;
        if (lines() < getUpperBoundFrame()) currLine = lines();
        else currLine = getUpperBoundFrame();
        currChar = getLineLength(currLine) - 1;
        int pos = getPhrasePosition(currLine) - getPhrasePosition(getLowerBoundFrame()) + getLineLength(currLine) - 1;
        if (currLine == lines()) {
            pos++;
            currChar++;
        }

        while (mpos.x < text.findCharacterPos(pos).x && text.findCharacterPos(pos).x > text.getGlobalBounds().left) {
            pos--;
            currChar--;
        }

    }
    else {
        currFrame = diffFrame;
        if (lines() < getUpperBoundFrame()) currLine = lines();
        else currLine = getUpperBoundFrame();
        int pos = getPhrasePosition(currLine) - getPhrasePosition(getLowerBoundFrame());
        while (text.findCharacterPos(pos).y > mpos.y && currLine > 0) {
            currLine--;
            pos = getPhrasePosition(currLine) - getPhrasePosition(getLowerBoundFrame());
        }
        pos = getPhrasePosition(currLine) - getPhrasePosition(getLowerBoundFrame()) + getLineLength(currLine) - 1;
        currChar = getLineLength(currLine) - 1;
        if (currLine == lines()) {
            pos++;
            currChar++;
        }

        while (mpos.x < text.findCharacterPos(pos).x && text.findCharacterPos(pos).x > text.getGlobalBounds().left) {
            pos--;
            currChar--;
        }
    }
}
void Content::scroll(bool direction) {
    if (direction) { // Up
        if (diffFrame > 0) {
            diffFrame--;
            if (currLine >= getLowerBoundFrame() && currLine <= getUpperBoundFrame()) currFrame--;
            else isFrameMoved = true;
       }
    }
    else {
        if (diffFrame <= lines() - propcount) {
            diffFrame++;
            if (currLine >= getLowerBoundFrame() && currLine <= getUpperBoundFrame()) currFrame++;
            else isFrameMoved = true;
        }
    }
    text.setString(composeStrings());
    updateNumbers();
    updateCursor();
}
void Content::left(bool isCtrlPressed, bool selectionDeletion) {
    if (isFrameMoved) {
        diffFrame = currFrame;
        isFrameMoved = false;
    }
    if (selected && selectionDeletion) removeSelection();
    if (currChar >= 1 || currLine > 0) {
        if (!isCtrlPressed) {
            int pos = getPhrasePosition(currLine) + currChar - 1;
            if (at(pos) == '\n') {
                currChar = getLineLength(currLine - 1) - 1;
                currLine--;
                if (currLine < getLowerBoundFrame() && currFrame > 0) {
                    currFrame--;
                    diffFrame = currFrame;
                }
                updateNumbers();
            }
            else currChar--;
        }
        else {
            int pos = getPhrasePosition(currLine) + currChar - 1;
            char ch = at(pos);
            if (ch == ' ') {
                currChar--;
            }
            else if (ch == '\n') {
                currChar = getLineLength(currLine - 1) - 1;
                currLine--;
                if (currLine < getLowerBoundFrame() && currFrame > 0) {
                    currFrame--;
                    diffFrame = currFrame;
                }
                updateNumbers();
            }
            else {
                while (currChar > 1 && (ch != ' ' && ch != '\n')) {
                    currChar--;
                    pos--;
                    ch = at(pos);
                }
                if (ch != ' ' && ch != '\n') {
                    currChar--;
                }
            }
        }
    }
    cursorState = false;
    text.setString(composeStrings());
    updateCursor();
}
void Content::right(bool isCtrlPressed, bool selectionDeletion) {
    if (isFrameMoved) {
        diffFrame = currFrame;
        isFrameMoved = false;
    }
    if (selected && selectionDeletion) removeSelection();
    if (currLine < lines() || currChar < getLineLength(currLine)) {
        if (!isCtrlPressed) {
            int pos = getPhrasePosition(currLine) + currChar;
            if (at(pos) == '\n') {
                currChar = 0;
                currLine++;
                if (currLine > getUpperBoundFrame()) {
                    currFrame++;
                    diffFrame = currFrame;
                }
                updateNumbers();
            }
            else currChar++;
        }
        else {
            int pos = getPhrasePosition(currLine) + currChar;
            char ch = at(pos);
            if (ch == ' ') {
                currChar++;
            }
            else if (ch == '\n') {
                currChar = 0;
                currLine++;
                if (currLine > getUpperBoundFrame()) {
                    currFrame++;
                    diffFrame = currFrame;
                }
                updateNumbers();
            }
            else {
                int maxim = getLineLength(currLine) - 1;
                if (currLine == lines()) maxim++;
                while (currChar < maxim && (ch != ' ' && ch != '\n')) {
                    currChar++;
                    pos++;
                    ch = at(pos);
                }
                
            }
        }
    }
    cursorState = false;
    text.setString(composeStrings());
    updateCursor();
}
void Content::up(bool selectionDeletion) {
    if (isFrameMoved) {
        diffFrame = currFrame;
        isFrameMoved = false;
    }
    if (selected && selectionDeletion) removeSelection();
    if (currLine > 0) {
        currLine--;
        if (currLine < getLowerBoundFrame() && currFrame > 0) {
            currFrame--;
            diffFrame = currFrame;
        }
        text.setString(composeStrings());
        int pos = getPhrasePosition(currLine + 1) + currChar - getPhrasePosition(getLowerBoundFrame());
        int oldX = text.findCharacterPos(pos).x;
        pos = pos - currChar - 1;
        currChar = getLineLength(currLine) - 1;
        while (oldX < text.findCharacterPos(pos).x && text.findCharacterPos(pos).x > text.getGlobalBounds().left) {
            pos--;
            currChar--;
        }
    }
    else {
        if (currLine == 0 && currChar > 0) currChar = 0;
    }
    cursorState = false;
    updateNumbers();
    updateCursor();
}
void Content::down(bool selectionDeletion) {
    if (isFrameMoved) {
        diffFrame = currFrame;
        isFrameMoved = false;
    }
    if (selected && selectionDeletion) removeSelection();
    if (currLine < lines()) {
        currLine++;
        if (currLine > getUpperBoundFrame()) {
            currFrame++;
            diffFrame = currFrame;
        }
        text.setString(composeStrings());
        int pos = getPhrasePosition(currLine - 1) + currChar - getPhrasePosition(getLowerBoundFrame());
        int oldX = text.findCharacterPos(pos).x;
        pos = getPhrasePosition(currLine) - getPhrasePosition(getLowerBoundFrame()) + getLineLength(currLine) - 1;
        currChar = getLineLength(currLine) - 1;
        if (currLine == lines()) {
            pos++;
            currChar++;
        }
        while (oldX < text.findCharacterPos(pos).x && text.findCharacterPos(pos).x > text.getGlobalBounds().left) {
            pos--;
            currChar--;
        }
    }
    else {
        if (currLine == lines() && currChar < getLineLength(currLine)) {
            currChar = getLineLength(currLine);
        }
    }
    cursorState = false;
    updateNumbers();
    updateCursor();
}
void Content::deleteBtn(bool isCtrlPressed) {
    if (isFrameMoved) {
        diffFrame = currFrame;
        isFrameMoved = false;
    }
    if (selected) removeSelection();
    int bakLine = currLine, bakChar = currChar;
    if (isCtrlPressed) right(true, true);
    else right(false, true);
    if (bakLine == currLine && bakChar == currChar) return;
    if (isCtrlPressed) onKeyPress(127);
    else onKeyPress(8);
}
void Content::zoomIn() {
    if (state < 6) state++;
    text.setCharacterSize(zoomstates[state]);
    numbers.setCharacterSize(zoomstates[state]);
    cursor.setSize(sf::Vector2f(2, (float)zoomstates[state]));
    updateResize();
}
void Content::zoomOut() {
    if (state > 0) state--;
    text.setCharacterSize(zoomstates[state]);
    numbers.setCharacterSize(zoomstates[state]);
    cursor.setSize(sf::Vector2f(2, (float)zoomstates[state]));
    updateResize();
}
void Content::copy(bool cut) {
    if (!selected) BAR::menu->setNotice("No text selected");
    else {
        BAR::setClipBoardText(composeSelectedStrings());
        if (cut) {
            if (!selected) return;
            BIGERASE();
            removeSelection();
        }
    }
}
void Content::paste() {
    if (isFrameMoved) {
        diffFrame = currFrame;
        isFrameMoved = false;
    }
    if (selected) removeSelection();
    std::string clipboardText = BAR::getClipboardText();
    insert(getPhrasePosition(currLine) + currChar, clipboardText);
    int newLineCnt = std::count(clipboardText.begin(), clipboardText.end(), '\n');
    int newLineFirstCharPos = clipboardText.find_last_of('\n');
    int lastChars = (newLineCnt == 0) ? clipboardText.length() : clipboardText.length() - (newLineFirstCharPos + 1);
    currLine += newLineCnt;
    currChar = (newLineCnt == 0) ? currChar + lastChars : lastChars;
    if (currLine > getUpperBoundFrame()) {
        currFrame = currLine - propcount + 1;
        diffFrame = currFrame;
    }
    text.setString(composeStrings());
    updateNumbers();
    updateCursor();
}
void Content::select(int control, bool isCtrlShiftPressed) {
    if (control == 0) { // Left
        if (!selected) {
            selectXright = currChar, selectYright = currLine;
            startingPosX = selectXright, startingPosY = selectYright;
            if (isCtrlShiftPressed) left(true, false);
            else left(false, false);
            selectXleft = currChar, selectYleft = currLine;
            selected = true;
        }
        else {
            if (isCtrlShiftPressed) left(true, false);
            else left(false, false);
            if (currChar == startingPosX && currLine == startingPosY) {
                removeSelection();
            }
            else if (currLine < startingPosY || currLine == startingPosY && currChar < startingPosX) {
                selectXleft = currChar, selectYleft = currLine;
            }
            else if (currLine > startingPosY || currLine == startingPosY && currChar > startingPosX) {
                selectXright = currChar, selectYright = currLine;
            }
        }
    }
    else if (control == 1) { // Right
        if (!selected) {
            selectXleft = currChar, selectYleft = currLine;
            startingPosX = selectXleft, startingPosY = selectYleft;
            if (isCtrlShiftPressed) right(true, false);
            else right(false, false);
            selectXright = currChar, selectYright = currLine;
            selected = true;
        }
        else {
            if (isCtrlShiftPressed) right(true, false);
            else right(false, false);
            if (currChar == startingPosX && currLine == startingPosY) {
                removeSelection();
            }
            else if (currLine < startingPosY || currLine == startingPosY && currChar < startingPosX) {
                selectXleft = currChar, selectYleft = currLine;
            }
            else if (currLine > startingPosY || currLine == startingPosY && currChar > startingPosX) {
                selectXright = currChar, selectYright = currLine;
            }
        }
    }
    else if (control == 2) { // Up
        if (!selected) {
            selectXright = currChar, selectYright = currLine;
            startingPosX = selectXright, startingPosY = selectYright;
            up(false);
            selectXleft = currChar, selectYleft = currLine;
            selected = true;
        }
        else {
            if (currLine > startingPosY || currLine == startingPosY && currChar > startingPosX) {
                up(false);
                if (currLine == startingPosY && currChar == startingPosX) {
                    removeSelection();
                }
                else if (currLine < startingPosY) {
                    selectXright = selectXleft, selectYright = selectYleft;
                    selectXleft = currChar, selectYleft = currLine;
                }
                else {
                    selectXright = currChar, selectYright = currLine;
                }
            }
            else {
                up(false);
                selectXleft = currChar, selectYleft = currLine;
            }
        }
    }
    else if (control == 3) { // Down
        if (!selected) {
            selectXleft = currChar, selectYleft = currLine;
            startingPosX = selectXleft, startingPosY = selectYleft;
            down(false);
            selectXright = currChar, selectYright = currLine;
            selected = true;
        }
        else {
            if (currLine < startingPosY || currLine == startingPosY && currChar < startingPosX) {
                down(false);
                if (currLine == startingPosY && currChar == startingPosX) {
                    removeSelection();
                }
                else if (currLine > startingPosY) {
                    selectXleft = selectXright, selectYleft = selectYright;
                    selectXright = currChar, selectYright = currLine;
                }
                else {
                    selectXleft = currChar, selectYleft = currLine;
                }
            }
            else {
                down(false);
                selectXright = currChar, selectYright = currLine;
            }
        }
    }
    else if (control == 4) {
        selectAll();
        return;
    }
    if (selectXleft == selectXright && selectYleft == selectYright) removeSelection();
    text.setString(composeStrings());
    updateNumbers();
    updateCursor();
}
void Content::selectAll() {
    if (root == nullptr) return;
    selected = true;
    selectXleft = 0, selectXright = 0;
    selectXright = getLineLength(lines()), selectYright = lines();
    startingPosX = 0, startingPosY = 0;
    currLine = lines();
    if (lines() < propcount) {
        currFrame = 0;
        diffFrame = currFrame;
    }
    else {
        currFrame = lines() - propcount + 1;
        diffFrame = currFrame;
    }
    currChar = getLineLength(lines());
    text.setString(composeStrings());
    updateNumbers();
    updateCursor();
}
void Content::removeSelection() {
    selected = false;
    selectXleft = 0, selectXright = 0;
    selectYleft = 0, selectYright = 0;
    startingPosX = 0, startingPosY = 0;
}


int digitsCount(int x) {
    int result = 0;
    while (x) {
        result++;
        x /= 10;
    }
    return result;
}

void Content::updateBlinker() {
    if (std::time(0) > date) { // efectul de "blink" al cursorului
        date = std::time(0);
        cursorState = !cursorState;
        cursor.setFillColor((cursorState ? sf::Color(206, 206, 206) : sf::Color::Transparent));
    }
}
void Content::updateResize() {
    int size1, size2;
    sf::Text temp("Mp", font, zoomstates[state]);
    size1 = temp.getLocalBounds().height;
    temp.setString("Mp\nMp");
    size2 = temp.getLocalBounds().height;
    propsize = size2 - size1;
    propcount = (window.getSize().y - BAR::HEIGHT) / propsize;
    if (lines() < propcount) {
        currFrame = 0;
        diffFrame = currFrame;
    }
    else if (currLine > getUpperBoundFrame()) {
        currFrame = currLine - propcount + 1;
        diffFrame = currFrame;
    }
    else if (currLine < getLowerBoundFrame()) {
        currFrame = currLine;
        diffFrame = currFrame;
    }
    text.setString(composeStrings());
    updateNumbers();
    updateCursor();
}
void Content::updateCursor() {
    if (currLine > getUpperBoundFrame()) {
        cursor.setPosition(sf::Vector2f(-1, -1));
        if (selected) updateSelections();
        return;
    }
    else if (currLine < getLowerBoundFrame()) {
        cursor.setPosition(sf::Vector2f(-1, -1));
        if (selected) updateSelections();
        return;
    }

    int cursorPos = getPhrasePosition(currLine) + currChar - getPhrasePosition(getLowerBoundFrame());
    if (text.findCharacterPos(cursorPos).x + 2.f < leftsize) {
        while (offset > 0 && text.findCharacterPos(cursorPos).x + 2.f < leftsize) {
            offset--;
            text.setPosition(sf::Vector2f(leftsize + (-200.f * offset), BAR::HEIGHT));
        }
    }
    else if (text.findCharacterPos(cursorPos).x + 2.f > window.getSize().x) {
        while (text.findCharacterPos(cursorPos).x + 2.f > window.getSize().x) {
            offset++;
            text.setPosition(sf::Vector2f(leftsize + (-200.f * offset), BAR::HEIGHT));
        }
    }
    
    cursor.setPosition(sf::Vector2f(text.findCharacterPos(cursorPos).x + 2.f, text.findCharacterPos(cursorPos).y + 1.f));

    if (selected) {
        updateSelections();
    }
}
void Content::updateSelections() {
    if (selectYleft == selectYright && (getLowerBoundFrame() <= selectYleft && selectYright <= getUpperBoundFrame())) {
        std::string selLine = getPhrase(selectYleft);
        sf::Text pretext(selLine.substr(0, selectXleft), font, zoomstates[state]);
        sf::Text text(selLine.substr(selectXleft, selectXright - selectXleft), font, zoomstates[state]);
        int sizetoadd = 0;
        if (selectXleft > 0) {
            sf::Text ambele(selLine.substr(selectXleft - 1, 2), font, zoomstates[state]), stanga(selLine.substr(selectXleft - 1, 1), font, zoomstates[state]), dreapta(selLine.substr(selectXleft, 1), font, zoomstates[state]);
            sizetoadd = ambele.getGlobalBounds().width - stanga.getGlobalBounds().width - dreapta.getGlobalBounds().width;
        }
        sf::RectangleShape blue;
        blue.setSize(sf::Vector2f(text.getGlobalBounds().width, propsize));
        int currVisibleLine = 0;
        if (lines() < propcount) currVisibleLine = selectYleft;
        else {
            currVisibleLine = selectYleft - getLowerBoundFrame();
        }
        blue.setPosition(leftsize + pretext.getLocalBounds().width + sizetoadd + (-200.f * offset), BAR::HEIGHT + propsize * currVisibleLine);
        blue.setFillColor(CONTENT::SELECT_COLOR);
        selectionBoxes.clear();
        selectionBoxes.push_back(blue);
    }
    else {
        int cnt = 0;
        selectionBoxes.clear();
        for (int i = getLowerBoundFrame(); i <= std::min(getUpperBoundFrame(), lines()); ++i) {
            if (i == selectYleft) {
                std::string selLine = getPhrase(i);
                sf::Text pretext(selLine.substr(0, selectXleft), font, zoomstates[state]);
                sf::Text text(selLine.substr(selectXleft, selLine.size() - selectXleft), font, zoomstates[state]);
                int sizetoadd = 0;
                if (selectXleft > 0) {
                    sf::Text ambele(selLine.substr(selectXleft - 1, 2), font, zoomstates[state]), stanga(selLine.substr(selectXleft - 1, 1), font, zoomstates[state]), dreapta(selLine.substr(selectXleft, 1), font, zoomstates[state]);
                    sizetoadd = ambele.getGlobalBounds().width - stanga.getGlobalBounds().width - dreapta.getGlobalBounds().width;
                }
                sf::RectangleShape blue;
                blue.setSize(sf::Vector2f(text.getGlobalBounds().width, propsize));
                blue.setPosition(leftsize + pretext.getLocalBounds().width + sizetoadd + (-200.f * offset), BAR::HEIGHT + cnt * propsize);
                blue.setFillColor(CONTENT::SELECT_COLOR);
                selectionBoxes.push_back(blue);
            }
            else if (i == selectYright) {
                std::string selLine = getPhrase(i);
                sf::Text text(selLine.substr(0, selectXright), font, zoomstates[state]);
                sf::RectangleShape blue;
                blue.setSize(sf::Vector2f(text.getGlobalBounds().width, propsize));
                blue.setPosition(leftsize + (-200.f * offset), BAR::HEIGHT + cnt * propsize);
                blue.setFillColor(CONTENT::SELECT_COLOR);
                selectionBoxes.push_back(blue);
            }
            else if (i > selectYleft && i < selectYright) {
                std::string selLine = getPhrase(i);
                sf::Text text(selLine, font, zoomstates[state]);
                sf::RectangleShape blue;
                blue.setSize(sf::Vector2f(text.getGlobalBounds().width, propsize));
                blue.setPosition(leftsize + (-200.f * offset), BAR::HEIGHT + cnt * propsize);
                blue.setFillColor(CONTENT::SELECT_COLOR);
                selectionBoxes.push_back(blue);
            }
            cnt++;
        }
    }
}
void Content::updateNumbers() {
    std::string str, tempString;
    if (lines() == -1) {
        str += "1\n";
        tempString = '4';
    }
    else {
        for (int i = getLowerBoundFrame(); i <= std::min(getUpperBoundFrame(), lines()); ++i) {
            str += std::to_string(i + 1) + '\n';
        }
        for (int i = 0; i < digitsCount(std::min(getUpperBoundFrame(), lines()) + 1); ++i) {
            tempString += '4';
        }
    }
    numbers.setString(str);
    numbers.setPosition(0.0f, BAR::HEIGHT);

    sf::Text temp(tempString, font, zoomstates[state]);

    leftsize = showLines * (temp.getGlobalBounds().width - temp.getGlobalBounds().left) + ((float)zoompercentages[state] / 100) * 15;
    
    numberRectangle.setSize(sf::Vector2f(leftsize, window.getSize().y));
    text.setPosition(sf::Vector2f(leftsize + (-200.f * offset), BAR::HEIGHT));
}
std::string Content::composeStrings() {
    // Effective y size: window.getSize().y - BAR::HEIGHT - 10

    std::string str;
    for (int i = getLowerBoundFrame(); i <= std::min(getUpperBoundFrame(), lines()); ++i) {
        str += getPhrase(i);
    }
    return str;
}
std::string Content::composeSelectedStrings() {
    if (selectYright - selectYleft == 0) {
        std::string phrase = getPhrase(selectYleft);
        phrase = phrase.substr(selectXleft, selectXright - selectXleft);
        return phrase;
    }
    else {
        std::string phrases = getPhrase(selectYleft);
        phrases = phrases.substr(selectXleft, getLineLength(selectYleft));
        for (int i = selectYleft + 1; i < selectYright; ++i) {
            phrases += getPhrase(i);
        }
        std::string tempPhrase = getPhrase(selectYright);
        tempPhrase = tempPhrase.substr(0, selectXright);
        phrases += tempPhrase;
        return phrases;
    }
}
void Content::draw_content() {
    if (selected) {
        for (int i = 0; i < selectionBoxes.size(); ++i) {
            window.draw(selectionBoxes[i]);
        }
    }
    window.draw(text);
    window.draw(cursor);
    window.draw(numberRectangle);
    if (showLines)
        window.draw(numbers); // numerele deasupra textului pentru partea cu "prea in dreapta"
    
}

int Content::find(std::string str) {
    if (str.size() == 0) return 0;
    if (root == nullptr) return 0;
    for (int i = 0; i <= lines(); ++i) {
        std::string currPhrase = getPhrase(i);
        int found = currPhrase.find(str);
        if (found != std::string::npos) {
            removeSelection();
            selected = true;
            selectXleft = found, selectXright = found + str.size();
            selectYleft = i, selectYright = i;
            startingPosX = selectXleft, startingPosY = selectYleft;
            currLine = i;
            currChar = found + str.size();
            if (lines() < propcount) {
                currFrame = 0;
                diffFrame = currFrame;
            }
            else if (currLine > getUpperBoundFrame()) {
                currFrame = currLine - propcount + 1;
                diffFrame = currFrame;
            }
            else if (currLine < getLowerBoundFrame()) {
                currFrame = currLine;
                diffFrame = currFrame;
            }
            text.setString(composeStrings());
            updateNumbers();
            updateCursor();
            return 1;
        }
    }
    return 0;
}
int Content::findPrev(std::string str) {
    if (str.size() == 0) return 0;
    if (root == nullptr) return 0;
    int maxLine, maxChar;
    if (!selected) {
        maxLine = currLine;
        maxChar = currChar;
    }
    else {
        maxLine = selectYleft;
        maxChar = selectXleft;
    }
    std::string currPhrase = getPhrase(maxLine);
    if (maxChar <= 0) currPhrase = "";
    else currPhrase = currPhrase.substr(0, maxChar);
    int goodFind = std::string::npos;
    int found = currPhrase.find(str);
    while (found != std::string::npos) {
        goodFind = found;
        found = currPhrase.find(str, found + 1);
    }
    if (goodFind != std::string::npos) {
        removeSelection();
        selected = true;
        selectXleft = goodFind, selectXright = goodFind + str.size();
        selectYleft = maxLine, selectYright = maxLine;
        startingPosX = selectXleft, startingPosY = selectYleft;
        currLine = maxLine;
        currChar = goodFind + str.size();
        if (lines() < propcount) {
            currFrame = 0;
            diffFrame = currFrame;
        }
        else if (currLine > getUpperBoundFrame()) {
            currFrame = currLine - propcount + 1;
            diffFrame = currFrame;
        }
        else if (currLine < getLowerBoundFrame()) {
            currFrame = currLine;
            diffFrame = currFrame;
        }
        text.setString(composeStrings());
        updateNumbers();
        updateCursor();
        return 1;
    }
    else {
        for (int i = maxLine - 1; i >= 0; --i) {
            std::string currPhrase = getPhrase(i);
            int goodFind = std::string::npos, found = currPhrase.find(str);
            while (found != std::string::npos) {
                goodFind = found;
                found = currPhrase.find(str, found + 1);
            }
            if (goodFind != std::string::npos) {
                removeSelection();
                selected = true;
                selectXleft = goodFind, selectXright = goodFind + str.size();
                selectYleft = i, selectYright = i;
                startingPosX = selectXleft, startingPosY = selectYleft;
                currLine = i;
                currChar = goodFind + str.size();
                if (lines() < propcount) {
                    currFrame = 0;
                    diffFrame = currFrame;
                }
                else if (currLine > getUpperBoundFrame()) {
                    currFrame = currLine - propcount + 1;
                    diffFrame = currFrame;
                }
                else if (currLine < getLowerBoundFrame()) {
                    currFrame = currLine;
                    diffFrame = currFrame;
                }
                text.setString(composeStrings());
                updateNumbers();
                updateCursor();
                return 1;
            }
        }
    }
    
    return 0;
}
int Content::findNext(std::string str) {
    if (str.size() == 0) return 0;
    if (root == nullptr) return 0;
    int minLine, minChar;
    if (!selected) {
        minLine = currLine;
        minChar = currChar;
    }
    else {
        minLine = selectYright;
        minChar = selectXright;
    }
    std::string currPhrase = getPhrase(minLine);
    if (minChar > currPhrase.size()) currPhrase = "";
    else currPhrase = currPhrase.substr(minChar, currPhrase.size());
    int found = currPhrase.find(str);
    if (found != std::string::npos) {
        removeSelection();
        selected = true;
        selectXleft = minChar + found, selectXright = minChar + found + str.size();
        selectYleft = minLine, selectYright = minLine;
        startingPosX = selectXleft, startingPosY = selectYleft;
        currLine = minLine;
        currChar = minChar + found + str.size();
        if (lines() < propcount) {
            currFrame = 0;
            diffFrame = currFrame;
        }
        else if (currLine > getUpperBoundFrame()) {
            currFrame = currLine - propcount + 1;
            diffFrame = currFrame;
        }
        else if (currLine < getLowerBoundFrame()) {
            currFrame = currLine;
            diffFrame = currFrame;
        }
        text.setString(composeStrings());
        updateNumbers();
        updateCursor();
        return 1;
    }
    for (int i = minLine + 1; i <= lines(); ++i) {
        std::string currPhrase = getPhrase(i);
        int found = currPhrase.find(str);
        if (found != std::string::npos) {
            removeSelection();
            selected = true;
            selectXleft = found, selectXright = found + str.size();
            selectYleft = i, selectYright = i;
            startingPosX = selectXleft, startingPosY = selectYleft;
            currLine = i;
            currChar = found + str.size();
            if (lines() < propcount) {
                currFrame = 0;
                diffFrame = currFrame;
            }
            else if (currLine > getUpperBoundFrame()) {
                currFrame = currLine - propcount + 1;
                diffFrame = currFrame;
            }
            else if (currLine < getLowerBoundFrame()) {
                currFrame = currLine;
                diffFrame = currFrame;
            }
            text.setString(composeStrings());
            updateNumbers();
            updateCursor();
            return 1;
        }
    }
    return 0;
}


int Content::getPercentage() {
    return zoompercentages[state];
}
void Content::loadText(std::string str) {
    destroyNode(root);
    if (str.size() == 0)
        root = nullptr;
    else
    {
        root = new nod;
        createRope(str, 0, str.size() - 1, root, nullptr);
    }
    init();
    text.setString(composeStrings());
    updateNumbers();
    updateCursor();
}


int Content::lines() { // indexare de la 0, daca nu exista text se va afisa -1
    if (root == nullptr) return -1;
    else return root->newline_characters;
}
void Content::out()
{
    if (root == nullptr)
        return;
    out(root, 0);
}
int Content::getLineLength(int line) {
    if (root == nullptr)
        return 0;
    if (line == lines())
        return root->subtree_size - getPhrasePosition(line);
    return getPhrasePosition(line + 1) - getPhrasePosition(line);
}
int Content::getPhrasePosition(int phrase_index)
{
    if (root == nullptr) {
        return 0;
    }
    return get_phrase_position(root, phrase_index, 0);
}
char Content::at(int pos) {
    int newPos = 0;
    nod* c = get_node_at_pos(pos, root, newPos);
    return c->data[newPos];
}
void Content::insert(int pos, char val)
{
    if (root == nullptr)
    {
        root = new nod;
        root->data = std::string("") + val;
        root->p = nullptr;
        root->l = nullptr;
        root->r = nullptr;
        recalculate_node_value(root);
        return;
    };

    int pos_in_string = 0;
    nod* c = get_node_at_pos(pos, root, pos_in_string);

    if (pos_in_string > c->data.size())
    {
        std::cout << "Where are you trying to insert bro?\n";
        return;
    }

    c->data.insert(c->data.begin() + pos_in_string, val);
    recalculate_node_values_up_from(c);

    if (c->data.size() > leaf_size)
    {
        nod* p = c->p;
        split_node(c);
        c = p;
        while (c != nullptr)
        {
            c = c->p;
            if (c == nullptr)
                break;
            local_rebalance(c);
        }
    }
}
void Content::insert(int pos, std::string& str)
{
    if (str.size() == 0) return;

    if (root == nullptr)
    {
        Content doi(str);
        root = doi.getRoot();
        return;
    }
    insert_string_pos(root, str, pos);
}
void Content::erase(int pos)
{
    if (root == nullptr)
    {
        std::cout << "Trying to erase nothing? Philosophcal ik\n";
        return;
    }

    int pos_in_string = 0;
    nod* c = get_node_at_pos(pos, root, pos_in_string);

    if (pos_in_string > c->data.size() - 1)
    {
        std::cout << "Trying to erase AFTER the string ended? How???\n";
        return;
    }

    c->data.erase(c->data.begin() + pos_in_string);

    if (c->data.size() != 0)
    {
        recalculate_node_values_up_from(c);
    }

    if (c->data.size() == 0)
    {
        if (c == root)
        {
            delete root;
            root = nullptr;
            return;
        }
        // ?   <- am pus asta aici cand am scris codul dar smr de mai stiu de ce l-am pus
        // daca intalnesti un bug pe parcurs anuntama
        nod* p = c->p;
        if (p->l == c)
            p->l = nullptr;
        else
            p->r = nullptr;
        delete c;
        nod* new_nod = resolve_removal(p);
        rebalance_nodes_up_from(new_nod->p);
    }
}
void Content::BIGERASE() {
    int x = getPhrasePosition(selectYleft) + selectXleft, y = getPhrasePosition(selectYright) + selectXright;
    erase(x, y - x);
    currChar = selectXleft;
    currLine = selectYleft;
    if (lines() < propcount) {
        currFrame = 0;
        diffFrame = currFrame;
    }
    else if (currLine > getUpperBoundFrame()) {
        currFrame = currLine - propcount + 1;
        diffFrame = currFrame;
    }
    else if (currLine < getLowerBoundFrame()) {
        currFrame = currLine;
        diffFrame = currFrame;
    }
    text.setString(composeStrings());
    updateNumbers();
    updateCursor();
}
int Content::getLowerBoundFrame() {
    return diffFrame;
}
int Content::getUpperBoundFrame() {
    return propcount + diffFrame - 1;
}
int Content::get_phrase_position(nod* c, int phrase_index, int left_positions)
{
    while (c->l != nullptr && c->r != nullptr) {
        int l = c->l->newline_characters;
        int r = c->r->newline_characters;
        if (l >= phrase_index)
        { // primul caracter e in partea din stanga (positibl cazu dubios)
            //return get_phrase_position(c->l, phrase_index, left_positions);
            c = c->l;
        }
        else
        { // primul caracter e in drepata
            //return get_phrase_position(c->r, phrase_index - l, left_positions + c->l->subtree_size);
            phrase_index -= l;
            left_positions += c->l->subtree_size;
            c = c->r;
        }
    }
    if (c->l == nullptr && c->r == nullptr)
    {
        // frunza
        int cnt = 0;
        int i = 0;
        while (cnt < phrase_index && i < c->data.size())
        {
            if (c->data[i] == '\n')
                cnt++;
            i++;
        }
        return left_positions + i;
    }
    /*else
    {
        int l = c->l->newline_characters;
        int r = c->r->newline_characters;
        if (l >= phrase_index)
        { // primul caracter e in partea din stanga (positibl cazu dubios)
            return get_phrase_position(c->l, phrase_index, left_positions);
        }
        else
        { // primul caracter e in drepata
            return get_phrase_position(c->r, phrase_index - l, left_positions + c->l->subtree_size);
        }
    }*/
}
void Content::recalculate_node_values_up_from(nod* c)
{
    while (c != nullptr)
    {
        recalculate_node_value(c);
        c = c->p;
    }
}
void Content::insert_string_pos(nod* c, const std::string& str, int pos)
{
    while (c->l != nullptr && c->r != nullptr) {
        if (c->l->subtree_size <= pos)
            pos -= c->l->subtree_size, c = c->r;
        else
            c = c->l;
    }
    // acum sunt in cazul asta indiferent 
    {
        // At a leaf
        if (pos == 0)
        {
            // inserez in stanga nodului curent
            Content t(str);
            if (t.getRoot() == nullptr) return;

            if (c == root) { // 1 singur nod in copac
                nod* newr = new nod;
                newr->p = 0;
                newr->r = root;
                newr->l = t.getRoot();
                root->p = newr;
                newr->l->p = newr;
                root = newr;
                recalculate_node_values_up_from(root);
                return;
            }

            nod* nou = new nod;
            nou->p = c->p;
            if (c->p != nullptr) {
                if (c->p->l == c)
                    c->p->l = nou;
                else
                    c->p->r = nou;
            }
            nou->l = t.getRoot();
            nou->l->p = nou;
            nou->r = c;
            c->p = nou;
            recalculate_node_values_up_from(nou);
            return;
        }
        else if (pos == c->data.size())
        {
            // isnerez in dreapta nodului curent
            Content t(str);
            if (t.getRoot() == nullptr) return;

            if (c == root) { // 1 singur nod in copac
                nod* newr = new nod;
                newr->p = 0;
                newr->l = root;
                newr->r = t.getRoot();
                root->p = newr;
                newr->r->p = newr;
                root = newr;
                recalculate_node_values_up_from(root);
                return;
            }

            nod* nou = new nod;
            
                nou->p = c->p;
                    if (c->p->l == c)
                        c->p->l = nou;
                    else
                        c->p->r = nou;
                
            
            nou->r = t.getRoot();
            nou->r->p = nou;
            nou->l = c;
            c->p = nou;
            recalculate_node_values_up_from(nou);
            return;
        }
        else
        {
            // trebuie sa dau split la nod si sa pun intre bucati noul text
            std::string left_data = c->data.substr(0, pos);
            std::string right_data = c->data.substr(pos, c->data.size() - pos);
            Content rl(left_data), rd(right_data), rt(str);
            nod* nou_1 = new nod;
            nou_1->p = c->p;
            if (c->p->l == c)
                c->p->l = nou_1;
            else
                c->p->r = nou_1;
            nou_1->l = rl.getRoot();
            nou_1->l->p = nou_1;
            nod* nou_2 = new nod;
            nou_2->p = nou_1;
            nou_1->r = nou_2;
            nou_2->l = rt.getRoot();
            nou_2->r = rd.getRoot();
            nou_2->r->p = nou_2;
            nou_2->l->p = nou_2;
            recalculate_node_values_up_from(nou_2);
        }
    }
    /* }
    else
    {
        if (c->l->subtree_size <= pos)
            insert_string_pos(c->r, str, pos - c->l->subtree_size);
        else
            insert_string_pos(c->l, str, pos);
    }*/
}

/// Will return The n-th phrase that ends with newline.
std::string Content::getPhrase(int index)
{
    if (root == nullptr) return "";
    std::string ret = "";
    get_phrase(root, index, ret);
    return ret;
}

/// Will return the node to the right of this one
/// Will return nullptr if there is no node to the right
nod* Content::getNextNode(nod* c)
{
    return get_next_node(c->p, c);
}

/// Will return the node to the right of this one
/// Will return nullptr if there is no node to the right
nod* Content::getPrevNode(nod* c)
{
    return get_prev_node(c->p, c);
}

/// Will return the first text node
nod* Content::getFirstNode(nod* c = nullptr)
{
    if (c == nullptr)
        c = root;
    if (c->l != nullptr)
        return getFirstNode(c->l);
    if (c->r != nullptr)
        return getFirstNode(c->r);
    return c;
}

/// Will return the first text node
nod* Content::getLastNode(nod* c = nullptr)
{
    if (c == nullptr)
        c = root;
    if (c->r != nullptr)
        return getLastNode(c->r);
    if (c->l != nullptr)
        return getLastNode(c->l);
    return c;
}

int Content::getLength(nod* c) {
    if (c == nullptr) return 0;
    return c->subtree_size;
}

std::string Content::getString() {
    if (root == nullptr) return "";
    std::string ret = "";
    get_string(root, ret);
    return ret;
}

void Content::get_string(nod* c, std::string& str) {
    if (c->l == nullptr && c->r == nullptr) {
        str += c->data;
        return;
    }
    if (c->l != nullptr)
        get_string(c->l, str);
    if (c->r != nullptr)
        get_string(c->r, str);
}

void Content::recalculate_node_value(nod* c)
{
    if (c == nullptr)
        return;
    if (c->l == nullptr && c->r == nullptr)
    {
        // leaf
        c->height = 1;
        c->leaf_nodes = 1;
        c->left_subtree_size = 0;
        c->subtree_size = c->data.size();
        c->newline_characters = 0;
        for (auto i : c->data)
            if (i == '\n')
                c->newline_characters++;
    }
    else
    {
        int subtree_size = 0;
        int left_subtree_size = 0;
        int leaf_nodes = 0;
        int height = 0;
        if (c->l != nullptr)
            height = std::max(c->l->height, height), leaf_nodes += c->l->leaf_nodes, subtree_size += c->l->subtree_size, left_subtree_size += c->l->subtree_size;
        if (c->r != nullptr)
            height = std::max(c->r->height, height), leaf_nodes += c->r->leaf_nodes, subtree_size += c->r->subtree_size;
        c->left_subtree_size = left_subtree_size;
        c->subtree_size = subtree_size;
        c->leaf_nodes = leaf_nodes;
        c->height = height + 1;
        c->newline_characters = (c->l == nullptr ? 0 : c->l->newline_characters) + (c->r == nullptr ? 0 : c->r->newline_characters);
    }
}

nod* Content::get_node_at_pos(int pos, nod* c, int& pos_in_string)
{
    while (c->l != nullptr || c->r != nullptr) {
        if (c->l->subtree_size > pos)
            c = c->l;
        else
            pos -= c->l->subtree_size, c = c->r;
    }

    if (c->l == nullptr && c->r == nullptr)
    {
        pos_in_string = pos;
        return c;
    }
    return nullptr;
}
void Content::split_node(nod* c)
{
    nod* p = c->p;
    nod* newc = new nod;
    nod* l = new nod;
    nod* r = new nod;
    l->l = nullptr;
    l->r = nullptr;
    r->l = nullptr;
    r->r = nullptr;
    l->data = c->data.substr(0, c->data.size() / 2);
    r->data = c->data.substr(c->data.size() / 2, c->data.size() - c->data.size() / 2);

    if (c == root)
    {
        delete newc;
        c->data = "";
        c->l = l;
        c->r = r;
        l->p = root;
        r->p = root;
        recalculate_node_value(l);
        recalculate_node_value(r);
        recalculate_node_value(root);
        return;
    }

    l->p = newc;
    r->p = newc;
    if (p->l == c)
        p->l = newc;
    else
        p->r = newc;
    newc->l = l;
    newc->r = r;
    newc->p = p;
    recalculate_node_value(l);
    recalculate_node_value(r);
    recalculate_node_values_up_from(newc);
    delete c;
}
void Content::deleteSubtree(nod* c)
{
    if (c->l != nullptr)
    {
        deleteSubtree(c->l);
        delete c->l;
        c->l = 0;
    }
    if (c->r != nullptr)
    {
        deleteSubtree(c->r);
        delete c->r;
        c->r = 0;
    }
}

void Content::_erase(nod* c, int pos, int count)
{
    nod* p = c->p;
    if (c->subtree_size <= count && pos == 0)
    {
        deleteSubtree(c);
        if (p != nullptr) {
            if (p->l == c)
                p->l = 0;
            else
                p->r = 0;
            delete c;
            p = resolve_removal(p);
            recalculate_node_values_up_from(p);
        }
        else {
            delete root;
            root = 0;
        }
        return;
    }

    if (c->l == nullptr && c->r == nullptr)
    {
        int fin = std::min(pos + count, int(c->data.size()));
        for (int i = pos; i < fin; i++)
        {
            c->data.erase(c->data.begin() + pos);
        }
        recalculate_node_values_up_from(c);
    }
    else
    {
        nod* nl = c->l, * nr = c->r;
        int l = c->l->subtree_size;

        if (pos < l) // stanga
            _erase(nl, pos, count);

        if (pos + count > l) // dreapta
        {
            if (pos < l) // a existat cv pe stanga
                _erase(nr, 0, count - (l - pos));
            else // sterg doar in dreapta
                _erase(nr, pos - l, count);
        }
    }
}

/// @brief
/// One of c's sons was removed.
/// Makes the node good and recalculates values.
/// @param c
/// Returns the new node that's instead of c.
nod* Content::resolve_removal(nod* c)
{
    nod* fiu = (c->l == nullptr ? c->r : c->l);

    if (c == root)
    {
        delete root;
        root = fiu;
        root->p = nullptr;
        return root;
    }

    if (c->p->r == c)
    {
        c->p->r = fiu;
        fiu->p = c->p;
    }
    else
    {
        c->p->l = fiu;
        fiu->p = c->p;
    }
    delete c;
    recalculate_node_values_up_from(fiu->p);
    return fiu;
}

nod* Content::get_leftmost_leaf_of_subtree(nod* r)
{
    while (r->l != nullptr || r->r != nullptr) {
        if (r->l != nullptr)
            r = r->l;
        else if (r->r != nullptr)
            r = r->r;
    }
    /*
    if (r->l != nullptr)
        return get_leftmost_leaf_of_subtree(r->l);
    if (r->r != nullptr)
        return get_leftmost_leaf_of_subtree(r->r);
        */
    return r;
}
nod* Content::get_rightmost_leaf_of_subtree(nod* r)
{
    while (r->l != nullptr || r->r != nullptr) {
        if (r->r != nullptr)
            r = r->r;
        else if (r->l != nullptr)
            r = r->l;
    }
    /*
    
    if (r->r != nullptr)
        return get_rightmost_leaf_of_subtree(r->r);
    if (r->l != nullptr)
        return get_rightmost_leaf_of_subtree(r->l);
    */
    return r;
}

void Content::createRope(std::string& str, int l, int r, nod* c, nod* p)
{
    if (r - l + 1 <= leaf_size)
    {
        c->l = nullptr;
        c->r = nullptr;
        c->data = str.substr(l, r - l + 1);
        c->p = p;
        recalculate_node_value(c);
    }
    else
    {
        int m = (l + r) / 2;
        if (m >= l)
        {
            c->l = new nod;
            createRope(str, l, m, c->l, c);
        }
        if (m + 1 <= r)
        {
            c->r = new nod;
            createRope(str, m + 1, r, c->r, c);
        }
        recalculate_node_value(c);
        c->p = p;
    }
}
void Content::out(nod* c, int h)
{
    if (c == root && c->p != nullptr)
    {
        std::cout << "Roots parent is not nullptr!\n";
    }
    if (c == nullptr)
        return;
    for (int i = 0; i < h; i++)
        std::cout << ' ';
    if (c->l != nullptr || c->r != nullptr)
    {
        std::cout << c->subtree_size << " newl: " << c->newline_characters << '\n';
    }
    else
    {
        std::cout << c->data << '\n';
    }
    if (c->l != nullptr)
    {
        if (c->l->p != c)
        {
            std::cout << "Problem at LEFT node with parent value " << c->subtree_size
                << "[this node's left child parent is not himself] \n";
        }
    }
    if (c->r != nullptr)
    {
        if (c->r->p != c)
        {
            std::cout << "Problem at RIGHT node with parent value " 
                << "[this node's right child parent is not himself] \n";
        }
    }
    out(c->l, h + 1);
    out(c->r, h + 1);
}

/// @brief
/// Tries to rebalance the ammount of leaf nodes in the left and right subtree
///      A                    A
///     / \                  /  \ 
///    B   G                B    H
///   / \          ->      / \   /\ 
///  C   D                C   E F  G
///     / \       
///    E   F
/// (Removes the node in the left of G and puts it in the actual left of G)
/// - ! Works for small sub-trees
///    (abs(node->left->leaf_nodes - node->right->leaf_nodes) <= 6)
/// todo When is it faster to just rebalance a subtree ?
/// @param c
void Content::local_rebalance(nod* c)
{
    int leaf_nodes_left = (c->l == nullptr ? 0 : c->l->leaf_nodes);
    int leaf_nodes_right = (c->r == nullptr ? 0 : c->r->leaf_nodes);

    if (1 == 0)
        if (leaf_nodes_left - 1 > leaf_nodes_right || leaf_nodes_right - 1 > leaf_nodes_left)
        {
            std::cout << "Actualyl rebalansing!\n";
            std::cout << "At the node with size " << c->subtree_size << '\n';
        }

    if (leaf_nodes_left - 1 > leaf_nodes_right)
    {
        // take leaf like example above (from left tree)
        // In punctul asta ar trebui sa avem nodul

        nod* put_it_left_of = get_leftmost_leaf_of_subtree(c->r);
        nod* put_this = get_rightmost_leaf_of_subtree(c->l);

        // cout << "Modifying " << put_this->data << ' ' << put_it_left_of->data << '\n';
        // cout << "Parent sizes: " << put_this->p->subtree_size << ' ' << put_it_left_of->p->subtree_size << '\n';

        if (put_this->p->r == put_this)
            put_this->p->r = nullptr;
        else
            put_this->p->l = nullptr;
        resolve_removal(put_this->p);

        // cout << "Left of: " << put_it_left_of->data << '\n';

        nod* p = put_it_left_of->p;
        if (p->l == put_it_left_of)
        {
            p->l = new nod;
            p->l->p = p;
            p->l->l = put_this;
            put_this->p = p->l;
            p->l->r = put_it_left_of;
            put_it_left_of->p = p->l;
        }
        else
        {
            // cout << "Is it possible to be here? if so remove me !\n";
            //  todo
            p->r = new nod;
            p->r->p = p;
            p->r->l = put_this;
            put_this->p = p->r;
            p->r->r = put_it_left_of;
            put_it_left_of->p = p->r;
        }
        recalculate_node_values_up_from(put_this->p);
    }
    else if (leaf_nodes_right - 1 > leaf_nodes_left)
    {
        // take leaf like opposite above

        nod* put_it_right_of = get_rightmost_leaf_of_subtree(c->l);
        nod* put_this = get_leftmost_leaf_of_subtree(c->r);

        if (put_this->p->r == put_this)
            put_this->p->r = nullptr;
        else
            put_this->p->l = nullptr;
        resolve_removal(put_this->p);

        //std::cout << "Datas: " << put_it_right_of->data << ' ' << put_this->data << '\n';

        nod* p = put_it_right_of->p;
        if (p->l == put_it_right_of)
        {
            //std::cout << "Is it possible to be here? if so remove me !\n";
            // todo
            p->l = new nod;
            p->l->p = p;
            p->l->r = put_this;
            put_this->p = p->l;
            p->l->l = put_it_right_of;
            put_it_right_of->p = p->l;
        }
        else
        {
            p->r = new nod;
            p->r->p = p;
            p->r->r = put_this;
            put_this->p = p->r;
            p->r->l = put_it_right_of;
            put_it_right_of->p = p->r;
        }
        recalculate_node_values_up_from(put_this->p);
    }
}
void Content::rebalance_nodes_up_from(nod* c)
{
    if (c == nullptr)
        return;
    local_rebalance(c);
    rebalance_nodes_up_from(c->p);
}
// todo cred ca nu trb sa balancez mai sus decat daca acum chiar s-a intamplat balansarea, altfel nare rost ? poate
void Content::get_phrase(nod* c, int index, std::string& str)
{
    //std::cout << "New function\n";
    while (c->l != nullptr && c->r != nullptr) {
        //std::cout << "Here " << c->subtree_size << '\n';
        int l = c->l->newline_characters;
        int r = c->r->newline_characters;
        if (l > index)
        { // prop e in partea din stanga complet
            //get_phrase(c->l, index, str);
            c = c->l;
        }
        else if (l == index)
        { // prop e atat in stanga, cat si in dreapta
            get_phrase(c->l, index, str);
            get_phrase(c->r, 0, str);
            return;
        }
        else
        { // prop e in drepata
            //get_phrase(c->r, index - l, str);
            c = c->r;
            index -= l;
        }
    }
    // aici oricum ?
    if (c->l == nullptr && c->r == nullptr)
    {
        // frunza
        int cnt = 0;
        int i = 0;
        while (cnt < index && i < c->data.size())
        {
            if (c->data[i] == '\n')
                cnt++;
            i++;
        }
        while (i < c->data.size())
        {
            str += c->data[i];
            if (c->data[i] == '\n')
                return;
            i++;
        }
    }
    /*else
    {
        int l = c->l->newline_characters;
        int r = c->r->newline_characters;
        if (l > index)
        { // prop e in partea din stanga complet
            get_phrase(c->l, index, str);
        }
        else if (l == index)
        { // prop e atat in stanga, cat si in dreapta
            get_phrase(c->l, index, str);
            get_phrase(c->r, 0, str);
        }
        else
        { // prop e in drepata
            get_phrase(c->r, index - l, str);
        }
    }*/
}

nod* Content::get_next_node(nod* c, nod* fiu)
{
    while (c != nullptr)
    {
        if (c->l == fiu)
            break;

        fiu = c;
        c = c->p;
    }
    if (c == nullptr)
        return nullptr;

    c = c->r;
    return get_leftmost_leaf_of_subtree(c);
}
nod* Content::get_prev_node(nod* c, nod* fiu)
{
    while (c != nullptr)
    {
        if (c->r == fiu)
            break;

        fiu = c;
        c = c->p;
    }
    if (c == nullptr)
        return nullptr;
    c = c->l;
    return get_rightmost_leaf_of_subtree(c);
}