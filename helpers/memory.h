#include "../types/types.h"

#ifndef MEMORY_H
#define MEMORY_H

/**
 * Frees everything in the game.
 *
 * @param tabuleiro The pointer to the game's tabuleiro.
 * @param cmd_str The String that stores the last command.
 * @param cmd The pointer to the last parsed command.
 *
 * @note If by the time you need to free memory you haven't initialized or allocated something. Pass NULL to that parameter.
 */
void free_all(Tab *tabuleiro, char *cmd_str, ParsedCommand *cmd);

#endif
