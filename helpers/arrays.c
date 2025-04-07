#include <stdlib.h>
#include <string.h>
#include "arrays.h"
#include "../jogo/tabuleiro.h"

TabHistory *push(TabHistory *previous, Tab tab)
{
    TabHistory *new = (TabHistory *)malloc(sizeof(TabHistory));
    new->tab.height = tab.height;
    new->tab.width = tab.width;

    // Deep copy the data array
    new->tab.data = malloc(sizeof(Piece) * tab.height * tab.width);
    memcpy(new->tab.data, tab.data, sizeof(Piece) * tab.height * tab.width);
    
    new->next = NULL;

    if (previous == NULL) {
        return new;  // new becomes the head of the list
    }

    previous->next = new;
    return previous;
}


Tab pop(TabHistory **head)
{
    Tab poppedTab;

    // a lista tá vazia
    if (*head == NULL)
    {
        poppedTab.data = NULL;
        return poppedTab;
    }

    // a lista só tem um elemento
    if ((*head)->next == NULL)
    {
        poppedTab = (*head)->tab;
        free(*head);
        *head = NULL;
        return poppedTab;
    }

    // percorre a lista até ao penúltimo elemento
    TabHistory *current = *head;
    TabHistory *previous = NULL;
    while (current->next != NULL)
    {
        previous = current;
        current = current->next;
    }

    // 'current' a este ponto deve ser o último elemento,
    // livra a memória e retorna.
    poppedTab = current->tab;

    // Make sure to free the last element correctly
    previous->next = NULL;
    free(current);  // Free current node, not the data in `poppedTab` itself

    return poppedTab;
}

Tab *get_hist_elem(TabHistory **head, int index)
{
    if (index < 0)
        return NULL;

    TabHistory *current = *head;
    while (index--)
    {
        if (current == NULL || current->next == NULL)
            return NULL;
        current = current->next;
    }

    if (current == NULL)
        return NULL;

    return &(current->tab);
}

void destroy(TabHistory **head)
{
    TabHistory *current = *head;
    TabHistory *next;

    while (current != NULL)
    {
        next = current->next;

        // Free the tab.data for each node before freeing the node itself
        free(current->tab.data);
        
        free(current);  // Now free the node
        current = next;
    }

    *head = NULL;  // Set the head to NULL after the list is destroyed
}
