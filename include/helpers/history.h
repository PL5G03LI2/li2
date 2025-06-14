#ifndef HISTORY_H
#define HISTORY_H
#include "types.h"

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
TabHistory *push_history(TabHistory *head, ParsedCommand *cmd);

/**
 * @brief Remove the last element and return its command.
 *
 * This function will remove the last element of the history and return a pointer to it.
 * @warning IT IS UP TO YOU TO FREE THE MEMORY OF THIS COMMAND.
 *
 * @param head The pointer to the head of the history.
 *
 * @returns The popped command off the history.
 *
 * @note The function will return CMD_INVALID if the head is NULL (history is empty).
 */
ParsedCommand *pop_history(TabHistory **head);

/**
 * @brief Gets the element from history based on the index.
 *
 * @param head The pointer to the head of the history.
 * @param index The (int) index of the element we're looking for.
 */
ParsedCommand *get_history_element(TabHistory *head, int index);

#endif // HISTORY_H
