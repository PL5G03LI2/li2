#ifndef HISTORY_H
#define HISTORY_H
#include "../types/types.h"

/* Linked list to record commands for undo */
typedef struct TabHistory {
    ParsedCommand cmd;
    struct TabHistory *next;
} TabHistory;

/* Function declarations for history manipulation */
TabHistory *push_history(TabHistory *head, ParsedCommand cmd);

ParsedCommand pop_history(TabHistory **head);

ParsedCommand *get_history_element(TabHistory *head, int index);

void destroy_history(TabHistory **head);

#endif // HISTORY_H
