#include "../types/types.h"

#ifndef ARRAYS_H
#define ARRAYS_H
/*
 Push a new tabuleiro to the history stack.

 @note To create a new list, just pass NULL as the previous parameter.
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

/*
 Free the memory allocated for the history stack.
*/
void destroy(TabHistory **head);
#endif