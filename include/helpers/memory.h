#include "types.h"

#ifndef MEMORY_H
#define MEMORY_H

/**
 * Allocates and initializes tabuleiro.
 * @returns A pointer to the initialized tabuleiro.
 * @returns NULL in case of allocation failure.
 */
Tab *initialize_tabuleiro(void);

/**
 * @brief Initializes all fields of Game
 *
 * @param game A pointer to the game to initialize.
 *
 * @returns 1 if fail, 0 if not.
 */
int init_game(Game *game);

/**
 * @brief Frees a command and it's tokens.
 *
 * @param cmd Double pointer to the command.
 *
 * @note The command will be set to NULL.
 */
void free_command(ParsedCommand **cmd);

/**
 * @brief Frees and sets tab (and its data) to NULL.
 *
 * @param tab The tab pointer
 */
void free_tabuleiro(Tab **tab);

/**
 * Free the whole history list
 *
 * @param head The pointer to the head of the list (stack).
 *
 * @note
 * This will clear the whole history, including free() all of the allocated memory.
 */
void destroy_history(TabHistory **head);

/**
 * Frees everything in the game.
 *
 * @param tabuleiro The pointer to the game's tabuleiro.
 * @param cmd_str The String that stores the last command.
 * @param cmd The pointer to the last parsed command.
 * @param head The head of the history stack.
 *
 * @note This will not free the game pointer itself, as it shouldn't be malloc-ed.
 */
void free_game(Game *game);

#endif
