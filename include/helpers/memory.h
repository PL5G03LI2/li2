#include "types.h"

#ifndef MEMORY_H
#define MEMORY_H

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
 * Frees everything in the game.
 *
 * @param tabuleiro The pointer to the game's tabuleiro.
 * @param cmd_str The String that stores the last command.
 * @param cmd The pointer to the last parsed command.
 * @param head The head of the history stack.
 */
void free_game(Game *game);

#endif
