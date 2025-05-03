#ifndef JOGO_RENDER_H
#define JOGO_RENDER_H

#include "types.h"

/**
 * Initializes colors for curses
 */
void init_colors();

/**
 * Calculates the layout of the game's windows
 */
void calculate_layout(Game *game);

/**
 * Applies the layout to the windows
 */
void apply_layout(UI *ui);

/**
 * Prints a command info, for example: Invalid command.
 */
void print_info(WIN cmd_win, const char *str);

/**
 * Render the game board to the main window
 */
void print_tab(Game *game);

/**
 * Renders the main game window
 */
void render_main(Game *game);

/**
 * Renders the help window
 */
void render_help(Game *game);

/**
 * Renders the command window
 */
void render_cmd(Game *game, char *info);

/**
 * Renders the game
 */
void render(Game *game, char *info);

#endif