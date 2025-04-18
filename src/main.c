#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <stdbool.h>
#include <ncurses.h>

#include "jogo/tabuleiro.h"
#include "jogo/comandos.h"
#include "helpers/history.h"
#include "helpers/memory.h"
#include "helpers/strs.h"

int init_game(Game *game)
{
    game->tabuleiro = initialize_tabuleiro();
    if (game->tabuleiro == NULL)
        return 1;

    game->cmd_str = (char *)calloc(256, sizeof(char));
    if (game->cmd_str == NULL)
    {
        free_game(game);
        return 1;
    }

    game->cmd = (ParsedCommand *)malloc(sizeof(ParsedCommand));
    if (game->cmd == NULL)
    {
        free_game(game);
        return 1;
    }

    game->cmd->type = CMD_INVALID;
    game->cmd->track = false;

    game->cmd->tokens = (char **)calloc(2, sizeof(char *));
    if (game->cmd->tokens == NULL)
    {
        free_game(game);
        return 1;
    }

    for (int i = 0; i < 2; i++)
    {
        game->cmd->tokens[i] = (char *)calloc(32, sizeof(char));
        if (game->cmd->tokens[i] == NULL)
        {
            free_game(game);
            return 1;
        }
    }

    game->history = NULL;

    return 0;
}

int repl(Game *game)
{
    while (true)
    {
        getmaxyx(stdscr, game->win_d.y, game->win_d.x);

        clear();

        if (game->tabuleiro->data)
        {
            iVec2 min_d = {game->tabuleiro->width * 2 + 2, game->tabuleiro->height + 4};
            if (game->win_d.x < min_d.x || game->win_d.y < min_d.y)
            {
                print_info("Window too small.", game->win_d);
            }
            else
                print_tab(game->tabuleiro, game->win_d);
        }
        else
        {
            move(game->win_d.y / 2, game->win_d.x / 2 - 22);
            printw("Awaiting load command... Hint: l <save_file>");
        }

        move(game->win_d.y - 1, 0);
        addch(':');

        if (await_command(game->cmd_str))
        {
            print_info("Failed to read command", game->win_d);
            continue;
        }

        if (parse_command(game))
        {
            print_info("Failed to parse command", game->win_d);
            continue;
        }

        if (game->cmd->track)
        {
            game->history = push_history(game->history, game->cmd);
        }

        if (game->cmd->type == CMD_EXIT)
            break;

        if (game->cmd->type == CMD_INVALID || (game->cmd->type != CMD_LOAD && game->tabuleiro->data == NULL))
        {
            print_info("Invalid command.", game->win_d);
            continue;
        }

        if (run_command(game))
        {
            print_info("Failed to run command.", game->win_d);
            continue;
        }
    }

    return 0;
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
    start_color();
    init_pair(1, COLOR_BLACK, COLOR_WHITE);

    Game game = {NULL, NULL, NULL, NULL, {0, 0}};
    getmaxyx(stdscr, game.win_d.y, game.win_d.x);

    init_game(&game);

    if (repl(&game))
    {
        free_game(&game);
        endwin();
        return 1;
    }

    // Nothing more to do, clean up.
    free_game(&game);
    endwin();
    return 0;
}
