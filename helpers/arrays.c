#include <stdlib.h>
#include "tabuleiro.h"

typedef struct node
{
    Tab tab;
    struct node *next;
} TabHistory;

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

    // percorre a lista até ao penúltimo elemto
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