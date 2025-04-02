#include <stdlib.h>
#include "tabuleiro.h"

typedef struct node
{
    Tab *tab;
    struct node *next;
} TabHistory;

TabHistory *push(TabHistory *previous, Tab *tab)
{
    TabHistory *new = (TabHistory *)malloc(sizeof(TabHistory));

    previous->next = new;

    new->tab = tab;
    new->next = NULL;

    return previous;
}