#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ncurses.h>

#include "jogo/tabuleiro.h"
#include "jogo/comandos.h"
#include "helpers/history.h"
#include "helpers/memory.h"
#include "helpers/strs.h"

static void init_colors(void)
{
    start_color();
    use_default_colors();
    init_pair(1, COLOR_BLACK, COLOR_WHITE); // selected normal
    init_pair(2, COLOR_RED, COLOR_WHITE);   // selected violated
    init_pair(3, COLOR_WHITE, COLOR_RED);   // violated not selected
    init_pair(4, COLOR_WHITE, -1);          // default
}

void render(Game *game, char *info)
{
    clear_info(game->win_d);
    print_info(info, game->win_d);

    // clear the whole board area
    for (int i = 0; i < game->win_d.y - 2; i++)
    {
        move(i, 0);
        clrtoeol();
    }

    if (game->tabuleiro->data)
    {
        print_tab(game->tabuleiro, game->win_d);
    }
    else
    {
        move(game->win_d.y / 2, game->win_d.x / 2 - 22);
        printw("Awaiting load command... Hint: l <save_file>");
    }

    move(game->win_d.y - 1, 0);
    clrtoeol(); // clear the line before input.
    addch(':');
}

void repl(Game *game)
{
    char info[256];
    while (game->cmd->type != CMD_EXIT)
    {
        getmaxyx(stdscr, game->win_d.y, game->win_d.x);

        render(game, info);

        if (await_command(game->cmd_str))
        {
            strcpy(info, "Failed to read command.");
            continue;
        }

        if (parse_command(game))
        {
            strcpy(info, "Failed to parse command");
            continue;
        }

        if (game->cmd->track)
        {
            game->history = push_history(game->history, game->cmd);
        }

        bool cmd_invalid = game->cmd->type == CMD_INVALID;
        bool no_load_intent_while_no_tab = (game->cmd->type != CMD_LOAD && game->tabuleiro->data == NULL);
        if (cmd_invalid || no_load_intent_while_no_tab)
        {
            strcpy(info, "Invalid command.");
            continue;
        }

        if (run_command(game))
        {
            strcpy(info, "Failed to run command.");
            continue;
        }
        strcpy(info, "");
    }
}

int main(void)
{
    initscr();
    if (!has_colors())
    {
        endwin();
        printf("Your terminal does not support colors...\n");
        return 1;
    }

    init_colors();

    Game game;
    init_game(&game);

    repl(&game);

    // Nothing more to do, clean up.
    free_game(&game);
    endwin();
    return 0;
}
