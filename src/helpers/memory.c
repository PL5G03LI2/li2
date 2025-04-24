#include <stdlib.h>
#include <ncurses.h>

#include "helpers/history.h"
#include "helpers/memory.h"
#include "jogo/tabuleiro.h"
#include "types.h"

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

    getmaxyx(stdscr, game->win_d.y, game->win_d.x);

    game->save_to = NULL;

    return 0;
}

void free_command(ParsedCommand **cmd)
{
    if (*cmd != NULL && (*cmd)->tokens)
    {
        for (int i = 0; i < 2; i++)
            free((*cmd)->tokens[i]);

        free((*cmd)->tokens);
        free(*cmd);
        *cmd = NULL;
    }
}

void free_game(Game *game)
{
    if (game->tabuleiro)
    {
        if (game->tabuleiro->data)
            free(game->tabuleiro->data);
        free(game->tabuleiro);
        game->tabuleiro = NULL; // Prevent use-after-free
    }

    if (game->cmd_str)
    {
        free(game->cmd_str);
        game->cmd_str = NULL;
    }

    free_command(&(game->cmd));

    destroy_history(&(game->history));

    free(game->save_to);
}