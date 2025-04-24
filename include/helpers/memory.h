#include "types.h"

#ifndef MEMORY_H
#define MEMORY_H

/**
 * Initializes all fields of Game
 *
 * @param game A pointer to the game to initialize.
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
 *
 * @note If by the time you need to free memory you haven't initialized or allocated something. Pass NULL to that parameter.
 */
void free_game(Game *game);

#endif
