#ifndef HISTORY_H
#define HISTORY_H
#include "../types/types.h"

/* Linked list to record commands for undo */
typedef struct TabHistory
{
    ParsedCommand cmd;
    struct TabHistory *next;
} TabHistory;

/**
 * @brief Insert at the end of the list (stack).
 *
 * This function will allocate memory and add a new node to the TabHistory
 *
 * @param head A pointer to the head of the list (stack).
 * @param cmd A ParsedCommand to push to the list.
 *
 * @returns A pointer to the head of the list. This is useful for creating a new list.
 *
 * @note To create a new list, just pass NULL as the head and take the return value.
 *
 * Otherwise, the return value is quite useless.
 */
TabHistory *push_history(TabHistory *head, ParsedCommand cmd);

/**
 * @brief Remove the last element and return its command.
 *
 * This function will remove the last element of the hsitory and return its value.
 *
 * @param head The pointer to the head of the history.
 *
 * @returns The popped command off the history.
 *
 * @note The function will return CMD_INVALID if the head is NULL (history is empty).
 */
ParsedCommand pop_history(TabHistory *head);

/**
 * @brief Get's the element from history based on the index.
 *
 * @param head The pointer to the head of the history.
 * @param index The (int) index of the element we're looking for.
 */
ParsedCommand *get_history_element(TabHistory *head, int index);

/**
 * Free the whole history list
 *
 * @param head The pointer to the head of the list (stack).
 *
 * @note This will clear the whole history, including free() all of the allocated memory.
 */
void destroy_history(TabHistory *head);

#endif // HISTORY_H
