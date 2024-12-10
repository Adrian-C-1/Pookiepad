#pragma once

#include <iostream> // de scos
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
    void update();
    void draw_content(); // asta va ramane mereu cu case-ul asta xDDD


private: // private
    
    /*Old content*/

    sf::Text text;
    sf::Text numbers;
    std::string numbersString;

    sf::RectangleShape cursor;
    bool cursorState;
    int numberCount;
    int offset;
    time_t date;

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
    void get_string(nod* c, std::string& str);
    char charAt(nod* c, int pos);

    void init();

    /*Old public*/

    void out();
    std::string getString();
    void insert(int pos, char val);
    void erase(int pos);
    char at(int pos);

    void left(bool isCtrlPressed);
    void right(bool isCtrlPressed);
    void up();
    void down();

    void updateCursor();

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