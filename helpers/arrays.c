#include <stdlib.h>
#include "arrays.h"
#include "tabuleiro.h"

TabHistory *push(TabHistory *previous, Tab tab)
{
    TabHistory *new = (TabHistory *)malloc(sizeof(TabHistory));

    previous->next = new;

    new->tab = tab;
    new->next = NULL;

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
    free(current);
    previous->next = NULL;

    return poppedTab;
}

Tab *get_elem(TabHistory **head, int index)
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
        free(current);
        current = next;
    }

    *head = NULL;
}
