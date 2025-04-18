#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ncurses.h>

#include "jogo/tabuleiro.h"
#include "jogo/comandos.h"
#include "helpers/history.h"
#include "helpers/memory.h"

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
        if (game->tabuleiro->data == NULL)
        {
            move(game->win_d.y - 2, 0);
            printw("Awaiting load command... Hint: l <save_file>");
            refresh();
        }

        move(game->win_d.y - 1, 0);

        if (await_command(game->cmd_str))
        {
            printw("Failed to read command");
            free_game(game);
            return 1;
        }

        if (parse_command(game))
        {
            printw("Failed to parse command");
            free_game(game);
            return 1;
        }

        if (game->cmd->track)
        {
            game->history = push_history(game->history, game->cmd);
        }

        if (game->cmd->type == CMD_EXIT)
            break;

        if (game->cmd->type == CMD_INVALID || (game->cmd->type != CMD_LOAD && game->tabuleiro->data == NULL))
        {
            printw("Invalid command.");
            continue;
        }

        if (run_command(game))
        {
            printw("Failed to run command.");
            free_game(game);
            return 1;
        }

        clear();
        if (game->cmd->type != CMD_EXIT)
            print_tab(game->tabuleiro);
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
