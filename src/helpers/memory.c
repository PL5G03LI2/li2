#include <stdlib.h>
#include "helpers/history.h"
#include "types.h"

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
}
