#include "tabuleiro.h"

typedef struct node
{
    Tab tab;
    struct node *next;
} TabHistory;

/*
 Push a new tabuleiro to the history stack.
*/
TabHistory *push(TabHistory *previous, Tab tab);

/*
 Pop the last tabuleiro from the history stack.
*/
Tab pop(TabHistory **head);

/*
 Get the tabuleiro at the given index
 @returns NULL if the index is out of bounds.
*/
Tab *get_elem(TabHistory **head, int index);