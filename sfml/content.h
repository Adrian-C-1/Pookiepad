#pragma once

#include <iostream> // de scos
#include <vector>
#include "globals.h"

class nod {
public:
	nod* l, * r, * p;
	std::string data;
	int left_subtree_size;
	int subtree_size;
	int height;
	int leaf_nodes;

	/// number of newline characters in subtree
	int newline_characters;
};

class Content {
public:

    Content();
    Content(std::string str);
    inline void destroyTree() { destroyNode(root); }
    
    // chestii
    void scroll(bool direction);
    // pe taste
    void onKeyPress(sf::Uint32 code);
    // chestii speciale
    void onKeyPress(sf::Keyboard::Key key);

    // selecteaza textul cand apesi cu mausu
    void onSelectText(sf::Vector2f mpos);

    void onMousePress();
    void onMouseMove();

    void loadText(std::string str);
    void copy(bool cut);
    void paste();
    void selectAll();
    
    // updateaza cursorul on-off
    void updateBlinker();
    // resile la window
    void updateResize();
    // asta va ramane mereu cu case-ul asta 
    void draw_content(); 

    inline void changeTheme() {
        numberRectangle.setFillColor(CONTENT::BG_COLOR);
        for (auto& i : selectionBoxes) {
            i.setFillColor(CONTENT::SELECT_COLOR);
        }
        text.setFillColor(CONTENT::TEXT_COLOR);
    }

    // din butoanele din bar
    int find(std::string str);
    int findPrev(std::string str);
    int findNext(std::string str);
    void zoomIn();
    void zoomOut();

    // returneaza zoom percentageul
    int getPercentage();
    
    // ia tot textul
    std::string getString();

    inline nod* getRoot() { return root; }
    inline int getCurrentLine() { return diffFrame; }
    inline int getLineCount() { return std::max(lines() - propcount + 1, 1); }
    
    // line e cea mai de sus linie care trb sa apara (indexare de la 0 !)
    void onScrollBar(int line);

    // cu l mic ca deja se numeste showLines si nu ma pot gandi la nume originale
    inline void showlines() { showLines = 1 - showLines; updateResize(); } // AM MURIT =)))))
private: // private

    nod* root;
    const int leaf_size = 6;

    sf::Text text; // textul vizibil
    sf::Text numbers; // numerele vizibile

    sf::RectangleShape cursor; 

    float leftsize; // x pt numberRectangle 
    sf::RectangleShape numberRectangle; // overlay pt 'numbers'

    std::vector<sf::RectangleShape> selectionBoxes; 
    bool cursorState;

    int currChar; // distanta de la inceputul liniei si pana la cursor | [0, m] (m = numarul de caractere din propzitia curenta, incluzand ultimul \n, daca exista)
    int currLine; // linia curenta la care ne aflam | [0, n] (n = numarul de linii din fisier)
    int currFrame; // frame-ul curent internal, fiecare frame avand propcount propozitii pe el | [0, n - propcount] cred (propcount = numarul de propozitii care pot incapea in frame)
    int propcount; // numarul de propozitii care se pot afla pe ecran in orice moment in functie de marimea ferestrei
    int propsize; // cat de mare poate fi o propozitie in functie de marimea ferestrei

    bool isFrameMoved;
    int diffFrame; // frameul vizibil (ca sa se miste inapoi la currFrame la type cand te uiti la altceva)
    
    time_t date; // pt cursor

    bool selected;
    // y - linia, x - caracter
    int selectXleft, selectYleft;  // caracterul din stanga selectiei
    int selectXright, selectYright; // caracterul din dreapta selectiei
    int startingPosX, startingPosY; // de unde ai inceput sa selctezi
    
    // folosit la size-ul cursorului si characterSize
    std::vector<int> zoomstates; // cate zoomuri sunt disponibile (50% - 200%)
    std::vector<int> zoompercentages; // procentaju pt om
    int state; // zoom-ul
    
    int offset; // pt cand ma duc prea in dreapta | off-ul textului
    
    bool showLines; 

    void init();

    void moveCursor(sf::Vector2f mpos); // pt click
    
    /// selectionDeletion nu mai selecteaza ce era selectat inainte</param>
    void left(bool isCtrlPressed, bool selectionDeletion); 
    void right(bool isCtrlPressed, bool selectionDeletion);
    void up(bool selectionDeletion);
    void down(bool selectionDeletion);
    void deleteBtn(bool isCtrlPressed); 

    // functie generala pt selct left/right/up/down
    // control - selecteaza up/down/l/r
    void select(int control, bool isCtrlShiftPressed); 

    // nu mai selecteaza ce era selectat inainte
    void removeSelection();

    // updateaza unde se afla cursoul
    void updateCursor();
    // updateaza pozitia selectiei
    void updateSelections();
    // numerele din stanga
    void updateNumbers();

    // ia ce e vizibil si pune in 'text'
    std::string composeStrings();
    // ia textul selectat
    std::string composeSelectedStrings();

    // sterge ceea ce e celectat
    void BIGERASE();

    // cel mai mic nr vizibil pe ecran
    int getLowerBoundFrame();
    // cel mai mare nr vizibil pe ecran
    int getUpperBoundFrame();





    // vvv chestii pt Rope

    void out();
    char at(int pos);
    void insert(int pos, char val);
    void insert(int pos, std::string& str);
    void erase(int pos);
    void erase(int start_pos, int count) { _erase(root, start_pos, count); }
    int lines(); // indexare de la 0, daca nu exista text se va afisa -1
    int getPhrasePosition(int phrase_index);
    // short definitions



    // deletes c
    void destroyNode(nod* c); 

    // recalculated any node values 
    // call this after a change
    void recalculate_node_value(nod* c);

    // recalculates up from (including) c
    void recalculate_node_values_up_from(nod* c);

    // indexing from 0
    nod* get_node_at_pos(int pos, nod* c, int& pos_in_string);

    // splits it in half
    // c is deleted
    // use only when inserting characters 
    void split_node(nod* c);

    // does not delete c
    void deleteSubtree(nod* c); 

    // erases 'count' characters
    void _erase(nod* c, int pos, int count);

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
    /// @param c
    void local_rebalance(nod* c);

    // c included
    void rebalance_nodes_up_from(nod* c);

    // returneaza indexul propozitiei care contine caracterul index
    void get_phrase(nod* c, int index, std::string& str);

    // returneaza frunza din dreapta la fiu
    nod* get_next_node(nod* c, nod* fiu);
    // returneaza frunza din stanga la fiu
    nod* get_prev_node(nod* c, nod* fiu);

    // returneaza lungimea unei functii
    int getLineLength(int line);

    // returneaza TOT textul
    // apeleaza doar cand e 100% necesar
    void get_string(nod* c, std::string& str);

    ;
    int get_phrase_position(nod* c, int phrase_index, int left_positions);
    void insert_string_pos(nod* c, const std::string& str, int pos);
    void out(nod* c, int h);

    /// @brief
    /// One of c's sons was removed.
    /// Makes the node good and recalculates values.
    /// @param c
    /// Returns the new node that's instead of c.
    nod* resolve_removal(nod* c);

    nod* get_leftmost_leaf_of_subtree(nod* r);
    nod* get_rightmost_leaf_of_subtree(nod* r);

    void createRope(std::string& str, int l, int r, nod* c, nod* p);

    /// Will return The n-th phrase that ends with newline.
    std::string getPhrase(int index);

    /// Will return the node to the right of this one
    /// Will return nullptr if there is no node to the right
    nod* getNextNode(nod* c);

    /// Will return the node to the right of this one
    /// Will return nullptr if there is no node to the right
    nod* getPrevNode(nod* c);

    /// Will return the first text node
    nod* getFirstNode(nod* c);

    /// Will return the first text node
    nod* getLastNode(nod* c);
};