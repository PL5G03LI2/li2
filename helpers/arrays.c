#include <stdlib.h>
#include <string.h>
#include "arrays.h"
#include "../jogo/tabuleiro.h"
#include "../types/types.h"



TabHistory *push(TabHistory *previous, ParsedCommand cmd)
{
    TabHistory *new = (TabHistory *)malloc(sizeof(TabHistory));
    new->cmd = cmd;
    new->next = NULL;

    if (previous == NULL) {
        return new;  // new becomes the head of the list
    }

    previous->next = new;
    return previous;
}


ParsedCommand pop(TabHistory **head)
{
    ParsedCommand poppedCmd;

    // if list is empty
    if (*head == NULL)
    {
        // Return empty/default ParsedCommand
        memset(&poppedCmd, 0, sizeof(ParsedCommand));
        return poppedCmd;
    }

    // if list has only one element
    if ((*head)->next == NULL)
    {
        poppedCmd = (*head)->cmd;
        free(*head);
        *head = NULL;
        return poppedCmd;
    }

    // traverse to second-to-last element
    TabHistory *current = *head;
    TabHistory *previous = NULL;
    while (current->next != NULL)
    {
        previous = current;
        current = current->next;
    }

    // current is now the last element
    poppedCmd = current->cmd;

    // remove last element
    previous->next = NULL;
    free(current);

    return poppedCmd;
}

ParsedCommand *get_hist_elem(TabHistory **head, int index)
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

    return &(current->cmd);
}

void destroy(TabHistory **head)
{
    TabHistory *current = *head;
    TabHistory *next;

    while (current != NULL)
    {
        next = current->next;
        free(current);  // Free the current node
        current = next;
    }

    *head = NULL;  // Set the head to NULL after the list is destroyed
}
