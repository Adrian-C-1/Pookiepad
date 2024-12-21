#include "content.h"

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
    cursor.setSize(sf::Vector2f((float)zoomstates[state] / 2, (float)zoomstates[state]));
    cursor.setFillColor(sf::Color::Black);
    cursorState = false;
    date = std::time(0);
    offset = 0;
    showLines = 1;
    frameoffset = 0;
    lineoffset = 0;
    localoffset = 0;

    linesizes.clear();
    updateResize();
}


void Content::onKeyPress(sf::Uint32 code) {
    if (code == 'z') { // debug purposes
        ;
    }
    if (code >= ' ' && code <= '~') { // Character
        insert(getPhrasePosition(lines() - lineoffset - 1) + offset, code);        
        offset++;
        text.setString(composeStrings());
        updateCursor();
    }
    else {
        switch (code) {
        case (13): // 13 = Enter
        {
            insert(getPhrasePosition(lines() - lineoffset - 1) + offset, '\n');
            offset = 0;
            if (localoffset < propcount - 1 && lines() <= propcount) localoffset++;
            text.setString(composeStrings());
            updateSizes();
            updateNumbers();
            updateCursor();
            break;
        }
        case (9): // 9 = Tab
        {
            for (int i = 0; i < 6; ++i) {
                insert(getPhrasePosition(lines() - lineoffset - 1) + offset, ' ');
            }
            offset += 6;
            text.setString(composeStrings());
            updateCursor();
            break;
        }
        case (8): // 8 = Backspace
        {
            if (offset == 0 && lines() - lineoffset - 1 <= 0) return;
            int pos = getPhrasePosition(lines() - lineoffset - 1) + offset - 1;
            if (at(pos) == '\n') {
                if (localoffset > 0 && lines() <= propcount) {
                    localoffset--;
                    offset = linesizes[localoffset] - 1;
                }
                else {
                    offset = linesizes[localoffset - 1] - 1;
                }
                
            }
            else offset--;
            erase(pos);
            text.setString(composeStrings());
            updateSizes();
            updateNumbers();
            updateCursor();
            break;
        }
        case (127): // 127 = Ctrl - Backspace
        {
            if (offset == 0 && lines() - lineoffset - 1 <= 0) return;
            int pos = getPhrasePosition(lines() - lineoffset - 1) + offset - 1;
            char ch = at(pos);
            if (ch == ' ') {
                offset--;
                erase(pos);
            }
            else if (ch == '\n') {
                if (localoffset > 0 && lines() <= propcount) {
                    localoffset--;
                    offset = linesizes[localoffset] - 1;
                }
                else {
                    offset = linesizes[localoffset - 1] - 1;
                }
                erase(pos);
            }
            else {
                while (offset > 1 && (ch != ' ' && ch != '\n')) {
                    offset--;
                    erase(pos);
                    pos--;
                    ch = at(pos);
                }
                if (ch != ' ' && ch != '\n') {
                    offset--;
                    erase(pos);
                }
            }
            text.setString(composeStrings());
            updateSizes();
            updateNumbers();
            updateCursor();
            break;
        }
        }
    }
    cursorState = false;
}
void Content::onKeyPress(sf::Keyboard::Key key) {
    if (key == sf::Keyboard::Left) {
        if ((sf::Keyboard::isKeyPressed(sf::Keyboard::LControl) || sf::Keyboard::isKeyPressed(sf::Keyboard::RControl))) {
            left(true);
        }
        else {
            left(false);
        }
    }
    else if (key == sf::Keyboard::Right) {
        if ((sf::Keyboard::isKeyPressed(sf::Keyboard::LControl) || sf::Keyboard::isKeyPressed(sf::Keyboard::RControl))) {
            right(true);
        }
        else {
            right(false);

        }
    }
    else if (key == sf::Keyboard::Up) {
        up();
    }
    else if (key == sf::Keyboard::Down) {
        down();
    }
    else if (key == 55) { // Ctrl + '+'
        zoomIn();
    }
    else if (key == 56) { // Ctrl + '-'
        zoomOut();
    }
}
void Content::onMousePress() {
    sf::Vector2f mpos = sf::Vector2f(sf::Mouse::getPosition(window));
    if (mpos.x <= text.getLocalBounds().left || mpos.y <= text.getLocalBounds().top) return;
    updateSizes();
    int pos = 0;
    for (int i = 0; i < linesizes.size(); ++i) {
        pos += linesizes[i];
    }

    if (mpos.y > text.getLocalBounds().height + text.getLocalBounds().top) {
        offset = linesizes[linesizes.size() - 1];
        while (localoffset <= propcount - 1 && lineoffset > 0) { // daca nu da bine ceva, incearca sa scoti egalul, ca in while-ul de mai jos
            lineoffset--;
            localoffset++;
        }

        while (mpos.x <= text.findCharacterPos(pos).x && text.findCharacterPos(pos).x > text.getLocalBounds().left) {
            offset--;
            pos--;
        }
    }
    else {
        int oldlocal = localoffset;
        while (localoffset < propcount - 1 && lineoffset > 0) {
            lineoffset--;
            localoffset++;
        }
        std::string str = text.getString();
        if (frameoffset <= 0) str += '\n';

        int cnt = 0;
        while (mpos.y <= text.findCharacterPos(pos).y) {
            if (str[pos] == '\n') {
                localoffset--;
                lineoffset++;
                cnt++;
            }
            pos--;
        }

        offset = linesizes[linesizes.size() - cnt - 1] - 1;
        if (lineoffset <= 0) {
            offset++;
        }

        while (mpos.x <= text.findCharacterPos(pos).x && text.findCharacterPos(pos).x > text.getLocalBounds().left) {
            offset--;
            pos--;
        }
    }
    cursorState = false;
    updateCursor();
}


void Content::left(bool isCtrlPressed) {
    if (offset >= 1 || lines() - lineoffset - 1 > 0) {
        if (!isCtrlPressed) {
            int pos = getPhrasePosition(lines() - lineoffset - 1) + offset - 1;
            if (at(pos) == '\n') {
                offset = getPhrasePosition(lines() - lineoffset - 1) - getPhrasePosition(lines() - lineoffset - 2);
                lineoffset++;
                if (localoffset > 0) localoffset--;
                else if (localoffset == 0) frameoffset++;
                updateSizes();
                updateNumbers();
            }
            offset--;
        }
        else {
            int pos = getPhrasePosition(lines() - lineoffset - 1) + offset - 1;
            char ch = at(pos);
            if (ch == ' ') {
                offset--;
            }
            else if (ch == '\n') {
                offset = getPhrasePosition(lines() - lineoffset - 1) - getPhrasePosition(lines() - lineoffset - 2) - 1;
                lineoffset++;
                if (localoffset > 0) localoffset--;
                else if (localoffset == 0) frameoffset++;
                updateSizes();
                updateNumbers();
            }
            else {
                while (offset > 1 && (ch != ' ' && ch != '\n')) {
                    offset--;
                    pos--;
                    ch = at(pos);
                }
                if (ch != ' ' && ch != '\n') offset--;
            }
        }
    }
    cursorState = false;
    text.setString(composeStrings());
    updateCursor();
}
void Content::right(bool isCtrlPressed) {
    if (getPhrasePosition(lines() - lineoffset - 1) + offset < getLength(root)) {
        if (!isCtrlPressed) {
            int pos = getPhrasePosition(lines() - lineoffset - 1) + offset;
            if (at(pos) == '\n') {
                lineoffset--;
                offset = -1;
                if (localoffset < propcount - 1) localoffset++;
                else if (localoffset >= propcount - 1 && frameoffset > 0) {
                    frameoffset--;
                }
                updateSizes();
                updateNumbers();
            }
            offset++;
        }
        else {
            int pos = getPhrasePosition(lines() - lineoffset - 1) + offset;
            char ch = at(pos);
            if (ch == ' ') {
                offset++;
            }
            else if (ch == '\n') {
                lineoffset--;
                offset = 0;
                if (localoffset < propcount - 1) localoffset++;
                else if (localoffset >= propcount - 1 && frameoffset > 0) {
                    frameoffset--;
                }
                updateSizes();
                updateNumbers();
            }
            else {
                int maxim = getPhrasePosition(lines() - lineoffset) - getPhrasePosition(lines() - lineoffset - 1);
                std::cout << maxim << '\n';
                while (offset < maxim && (ch != ' ' && ch != '\n')) {
                    offset++;
                    pos++;
                    ch = at(pos);
                }
                //if (ch != ' ' && ch != '\n') offset--;
            }
        }
    }
    cursorState = false;
    text.setString(composeStrings());
    updateCursor();
}
void Content::up() {
    if (lines() - lineoffset > 1) {
        lineoffset++;
        if (localoffset > 0) localoffset--;
        else if (localoffset == 0) {
            frameoffset++;
        }
        text.setString(composeStrings());
        updateSizes();
        updateNumbers();
        if (offset > 0) {
            int pos = 0;
            for (int i = 0; i <= localoffset; ++i) {
                pos += linesizes[i];
            }
            pos += offset;
            sf::Vector2f oldPos = text.findCharacterPos(pos); // pozitia pe care se afla cuvantul de pe care am plecat
            pos = pos - offset - 1; // revenirea la randul anterior
            offset = linesizes[localoffset];
            if (text.findCharacterPos(pos).x < oldPos.x) { // de pe un rand mai mare pe un rand mai mic
                offset--;
            }
            else { // invers
                while (oldPos.x <= text.findCharacterPos(pos).x && text.findCharacterPos(pos).x > text.getGlobalBounds().left) {
                    pos--;
                    offset--;
                }
            }
        }
    }
    else {
        if (lines() - lineoffset == 1) offset = 0;
        text.setString(composeStrings());
    }
    cursorState = false;
    updateCursor();
}
void Content::down() {
    if (lineoffset > 0) {
        lineoffset--;
        if (localoffset < propcount - 1) localoffset++;
        else if (localoffset >= propcount - 1) {
            frameoffset--;
        }
        text.setString(composeStrings());
        updateSizes();
        updateNumbers();
        if (offset > 0) {
            int pos = 0;
            for (int i = 0; i < localoffset - 1; ++i) {
                pos += linesizes[i];
            }
            pos += offset;
            sf::Vector2f oldPos = text.findCharacterPos(pos); // pozitia caracterului de pe care plec
            pos = pos - offset + linesizes[localoffset - 1] + linesizes[localoffset]; // pozitia caracterului de pe propozitia pe care ma duc
            offset = linesizes[localoffset];
            if (lineoffset > 0) { // caz in care ma aflu pe ultima propozitie din sir si nu e \n la final (lineoffset se modifica mai sus)
                offset--;
                pos--;
            }
            while (oldPos.x < text.findCharacterPos(pos).x && text.findCharacterPos(pos).x > text.getGlobalBounds().left) {
                pos--;
                offset--;
            }
        }
    }
    else if (lineoffset == 0) {
        text.setString(composeStrings());
        updateSizes();
        updateNumbers();
        offset = linesizes[linesizes.size() - 1];
    }
    cursorState = false;
    updateCursor();
}
void Content::zoomIn() {
    if (state < 6) state++;
    text.setCharacterSize(zoomstates[state]);
    numbers.setCharacterSize(zoomstates[state]);
    cursor.setSize(sf::Vector2f((float)zoomstates[state] / 2, (float)zoomstates[state]));
    updateResize();
}
void Content::zoomOut() {
    if (state > 0) state--;
    text.setCharacterSize(zoomstates[state]);
    numbers.setCharacterSize(zoomstates[state]);
    cursor.setSize(sf::Vector2f((float)zoomstates[state] / 2, (float)zoomstates[state]));
    updateResize();
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
    frameoffset = 0;
    if (lines() > propcount) localoffset = propcount - 1;
    else localoffset = lines() - 1;
    lineoffset = 0;
    text.setString(composeStrings());
    updateSizes();
    offset = linesizes[linesizes.size() - 1];
    updateNumbers();
    updateCursor();
}
void Content::updateCursor() {
    int localpos = 0;
    for (int i = 0; i < localoffset; ++i) {
        localpos += linesizes[i];
    }
    localpos += offset;
    
    cursor.setPosition(sf::Vector2f(text.findCharacterPos(localpos).x + 2.f, text.findCharacterPos(localpos).y + 1.f));
}
void Content::updateNumbers() {
    std::string str;
    for (int i = (lines() - frameoffset - propcount + 1 > 0 ? lines() - frameoffset - propcount + 1 : 1); i <= lines() - frameoffset; ++i) {
        str += std::to_string(i) + '\n';
    }
    numbers.setString(str);
    numbers.setPosition(0.0f, BAR::HEIGHT);

    std::string tempString;
    for (int i = 0; i < digitsCount(lines() - frameoffset); ++i) {
        tempString += '4';
    }
    sf::Text temp(tempString, font, zoomstates[state]);

    float leftsize = showLines * (temp.getLocalBounds().width - temp.getLocalBounds().left) + ((float)zoompercentages[state] / 100) * 15;    text.setPosition(sf::Vector2f(leftsize, BAR::HEIGHT));
    text.setPosition(sf::Vector2f(leftsize, BAR::HEIGHT));

}
void Content::updateSizes() {
    linesizes.clear();
    for (int i = (lines() - frameoffset - propcount + 1 > 0 ? lines() - frameoffset - propcount + 1 : 1); i <= lines() - frameoffset; ++i) {
        linesizes.push_back(getLineLength(i - 1));
    }
}
std::string Content::composeStrings() {
    // Effective y size: window.getSize().y - BAR::HEIGHT - 10

    std::string str;
    for (int i = (lines() - frameoffset - propcount + 1 > 0 ? lines() - frameoffset - propcount + 1 : 1); i <= lines() - frameoffset; ++i) {
        str += getPhrase(i - 1);
    }
    return str;
}
void Content::draw_content() {
    window.draw(text);
    window.draw(cursor);
    if (showLines)
        window.draw(numbers);
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
    frameoffset = 0;
    if (lines() > propcount) localoffset = propcount - 1;
    else localoffset = lines() - 1;
    lineoffset = 0;
    text.setString(composeStrings());
    updateSizes();
    offset = linesizes[linesizes.size() - 1];
    updateNumbers();
    updateCursor();
}


int Content::lines() { // indexare de la 1, adica daca e o linie, se va returna 1
    if (root == nullptr) return 1;
    else return 1 + root->newline_characters;
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
    if (line == lines() - 1)
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
        // cout << "Here 1\n";
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
            // cout << "Before rebalancing: \n";
            // out();
            local_rebalance(c);
        }
    }
}
void Content::insert(int pos, std::string text)
{
    for (int i = 0; i < text.size(); i++)
    {
        insert(pos + i, text[i]);
    }
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


int Content::get_phrase_position(nod* c, int phrase_index, int left_positions)
{
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
    else
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
    }
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
    // cout << "Here with " << (c == root ? "root" : "not the root?") << '\n';
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
            height = std::max(c->l->height, height), leaf_nodes += c->r->leaf_nodes, subtree_size += c->r->subtree_size;
        c->left_subtree_size = left_subtree_size;
        c->subtree_size = subtree_size;
        c->leaf_nodes = leaf_nodes;
        c->height = height + 1;
        c->newline_characters = (c->l == nullptr ? 0 : c->l->newline_characters) + (c->r == nullptr ? 0 : c->r->newline_characters);
    }
}
void Content::recalculate_node_values_up_from(nod* c)
{
    while (c != nullptr)
    {
        // cout << "here " << c << '\n';
        recalculate_node_value(c);
        c = c->p;
    }
}
nod* Content::get_node_at_pos(int pos, nod* c, int& pos_in_string)
{
    if (c->l == nullptr && c->r == nullptr)
    {
        pos_in_string = pos;
        return c;
    }
    if (c->l->subtree_size > pos)
        return get_node_at_pos(pos, c->l, pos_in_string);
    return get_node_at_pos(pos - c->l->subtree_size, c->r, pos_in_string);
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
    recalculate_node_values_up_from(fiu);
    return fiu;
}

nod* Content::get_leftmost_leaf_of_subtree(nod* r)
{
    if (r->l != nullptr)
        return get_leftmost_leaf_of_subtree(r->l);
    if (r->r != nullptr)
        return get_leftmost_leaf_of_subtree(r->r);
    return r;
}
nod* Content::get_rightmost_leaf_of_subtree(nod* r)
{
    if (r->r != nullptr)
        return get_rightmost_leaf_of_subtree(r->r);
    if (r->l != nullptr)
        return get_rightmost_leaf_of_subtree(r->l);
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
        std::cout << c->subtree_size << ' ' << c->height << ' ' << c->newline_characters << '\n';
    }
    else
    {
        std::cout << c->data << '\n';
    }
    if (c->l != nullptr)
    {
        if (c->l->p != c)
        {
            std::cout << "Problem at LEFT node with parent value " << c->subtree_size << '\n';
        }
    }
    if (c->r != nullptr)
    {
        if (c->r->p != c)
        {
            std::cout << "Problem at LEFT node with parent value " << c->subtree_size << '\n';
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
    // cout << c->subtree_size << ' ' << index << '\n';
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
    else
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
    }
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