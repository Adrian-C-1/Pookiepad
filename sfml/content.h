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
    
    void scroll(bool direction);
    void onKeyPress(sf::Uint32 code);
    void onKeyPress(sf::Keyboard::Key key);

    void onSelectText(sf::Vector2f mpos);
    void onMousePress();
    void onMouseMove();

    void loadText(std::string str);
    void copy(bool cut);
    void paste();
    void selectAll();
    void updateBlinker();
    void updateResize();
    void draw_content(); // asta va ramane mereu cu case-ul asta xDDD

    int find(std::string str);
    int findPrev(std::string str);
    int findNext(std::string str);

    void zoomIn();
    void zoomOut();

    int getPercentage();
    
    std::string getString();

    inline nod* getRoot() { return root; }
    inline int getCurrentLine() { return diffFrame; }
    inline int getLineCount() { return std::max(lines() - propcount + 1, 1); }
    // line e cea mai de sus linie care trb sa apara (indexare de la 0 !)
    void onScrollBar(int line);


    // cu l mic ca deja se numeste showLines si nu ma pot gandi la nume originale
    inline void showlines() { showLines = 1 - showLines; updateResize(); } // AM MURIT =)))))
public: // private

    nod* root;
    const int leaf_size = 6;

    sf::Text text;
    sf::Text numbers;

    sf::RectangleShape cursor;
    sf::RectangleShape numberRectangle;
    std::vector<sf::RectangleShape> selectionBoxes;
    bool cursorState;

    int currChar; // distanta de la inceputul liniei si pana la cursor | [0, m] (m = numarul de caractere din propzitia curenta, incluzand ultimul \n, daca exista)
    int currLine; // linia curenta la care ne aflam | [0, n] (n = numarul de linii din fisier)
    int currFrame; // frame-ul curent vizibil, fiecare frame avand propcount propozitii pe el | [0, n - propcount] cred (propcount = numarul de propozitii care pot incapea in frame)
    int propcount; // numarul de propozitii care se pot afla pe ecran in orice moment in functie de marimea ferestrei
    int propsize; // cat de mare poate fi o propozitie in functie de marimea ferestrei

    bool isFrameMoved;
    int diffFrame;
    
    time_t date;

    bool selected;
    int selectXleft, selectYleft;
    int selectXright, selectYright;
    int startingPosX, startingPosY;
    

    std::vector<int> zoomstates;
    std::vector<int> zoompercentages;
    int offset;
    float leftsize;
    int state;
    bool showLines;

    void init();

    void moveCursor(sf::Vector2f mpos);
    void left(bool isCtrlPressed, bool selectionDeletion);
    void right(bool isCtrlPressed, bool selectionDeletion);
    void up(bool selectionDeletion);
    void down(bool selectionDeletion);
    void deleteBtn(bool isCtrlPressed);
    void select(int control, bool isCtrlShiftPressed);
    void removeSelection();

    void updateCursor();
    void updateSelections();
    void updateNumbers();
    std::string composeStrings();
    std::string composeSelectedStrings();

    void out();
    char at(int pos);
    void insert(int pos, char val);
    void insert(int pos, std::string& str);
    void erase(int pos);
    void erase(int start_pos, int count) { _erase(root, start_pos, count); }
    void BIGERASE();
    int lines();
    int getPhrasePosition(int phrase_index);
    int getLowerBoundFrame();
    int getUpperBoundFrame();

    void destroyNode(nod* c);

    void recalculate_node_value(nod* c);
    void recalculate_node_values_up_from(nod* c);
    nod* get_node_at_pos(int pos, nod* c, int& pos_in_string);
    void split_node(nod* c);
    void deleteSubtree(nod* c);
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
    /// todo When is it faster to just rebalance a subtree ?
    /// @param c
    void local_rebalance(nod* c);
    void rebalance_nodes_up_from(nod* c);
    // todo cred ca nu trb sa balancez mai sus decat daca acum chiar s-a intamplat balansarea, altfel nare rost ? poate

    void get_phrase(nod* c, int index, std::string& str);

    nod* get_next_node(nod* c, nod* fiu);
    nod* get_prev_node(nod* c, nod* fiu);

    int getLength(nod* c);
    int getLineLength(int line);
    void get_string(nod* c, std::string& str);
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