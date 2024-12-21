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
    
    void onKeyPress(sf::Uint32 code);
    void onKeyPress(sf::Keyboard::Key key);
    void onMousePress();

    void loadText(std::string str);
    void updateBlinker();
    void updateResize();
    void draw_content(); // asta va ramane mereu cu case-ul asta xDDD

    void zoomIn();
    void zoomOut();

    int getPercentage();
    
    std::string getString();

    inline void destroyTree() { destroyNode(root); }
    // cu l mic ca deja se numeste showLines si nu ma pot gandi la nume originale
    inline void showlines() { showLines = 1 - showLines; updateNumbers(); }
private: // private
    bool showLines;
    /*Old content*/

    sf::Text text;
    sf::Text numbers;

    sf::RectangleShape cursor;
    bool cursorState;

    int offset; // distanta de la inceputul liniei si pana la cursor | [0, m] (m = numarul de caractere din propozitie, incluzand ultimul \n, daca exista)
    int lineoffset; // distanta de la sfarsitul textului la cursor | [0, n] (n = numarul de linii din tot fisierul)
    int frameoffset; // diferenta de frame-uri vizibile de la ultimul si pana la cel vizibil | [0, n - propcount] cred (propcount = numarul de propozitii care pot incapea in frame)
    int localoffset; // distanta de la cea mai de sus linie vizibila pe ecran la linia pe care se afla cursorul | [0, propcount]
    int propcount; // numarul de propozitii care se pot afla pe ecran in orice moment in functie de marimea ferestrei
    int propsize; // cat de mare poate fi o propozitie in functie de marimea ferestrei
    std::vector<int> linesizes; // lungimea tuturor liniilor vizibile pe ecran (cam in orice moment)
    time_t date;

    std::vector<int> zoomstates;
    std::vector<int> zoompercentages;
    int state;

    // now
	void destroyNode(nod* c);

    /*Old private*/

    nod* root;
    const int leaf_size = 6;

    void recalculate_node_value(nod* c);
    void recalculate_node_values_up_from(nod* c);
    nod* get_node_at_pos(int pos, nod* c, int& pos_in_string);
    void split_node(nod* c);

    /// @brief
    /// One of c's sons was removed.
    /// Makes the node good and recalculates values.
    /// @param c
    /// Returns the new node that's instead of c.
    nod* resolve_removal(nod* c);

    nod* get_leftmost_leaf_of_subtree(nod* r);
    nod* get_rightmost_leaf_of_subtree(nod* r);

    void createRope(std::string& str, int l, int r, nod* c, nod* p);
    void out(nod* c, int h);

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

    std::string composeStrings();

    void init();

    /*Old public*/

    void out();
    void insert(int pos, char val);
    void insert(int pos, std::string text);
    void erase(int pos);
    char at(int pos);
    int lines();
    int getPhrasePosition(int phrase_index);

    void left(bool isCtrlPressed);
    void right(bool isCtrlPressed);
    void up();
    void down();

    void updateCursor();
    void updateNumbers();
    void updateSizes();

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