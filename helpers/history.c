#include <stdlib.h>
#include <string.h>
#include "history.h"

TabHistory *push_history(TabHistory *head, ParsedCommand cmd)
{
    TabHistory *new_node = (TabHistory *)malloc(sizeof(TabHistory));
    new_node->cmd = cmd;
    new_node->next = NULL;

    if (head == NULL)
    {
        return new_node; // New node becomes head
    }

    TabHistory *current = head;
    while (current->next != NULL)
        current = current->next;

    current->next = new_node;
    return head;
}

ParsedCommand pop_history(TabHistory *head)
{
    ParsedCommand poppedCmd;
    memset(&poppedCmd, 0, sizeof(ParsedCommand));

    if (head == NULL)
    {
        return poppedCmd;
    }

    if (head->next == NULL)
    {
        poppedCmd = head->cmd;
        free(head);
        head = NULL;
        return poppedCmd;
    }

    TabHistory *current = head;
    TabHistory *prev = NULL;
    while (current->next != NULL)
    {
        prev = current;
        current = current->next;
    }

    poppedCmd = current->cmd;
    prev->next = NULL;
    free(current);
    return poppedCmd;
}

ParsedCommand *get_history_element(TabHistory *head, int index)
{
    if (index < 0)
        return NULL;
    int i = 0;
    TabHistory *current = head;
    while (current != NULL && i < index)
    {
        current = current->next;
        i++;
    }
    return (current ? &(current->cmd) : NULL);
}

void destroy_history(TabHistory *head)
{
    TabHistory *current = head;
    TabHistory *next;
    while (current != NULL)
    {
        next = current->next;
        free(current);
        current = next;
    }
    head = NULL;
}
