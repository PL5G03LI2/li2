#include <stdlib.h>
#include "helpers/history.h"
#include "types.h"

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

    if (game->cmd && game->cmd->tokens)
    {
        for (int i = 0; i < 2; i++)
            free(game->cmd->tokens[i]);

        free(game->cmd->tokens);
        free(game->cmd);
        game->cmd = NULL;
    }

    destroy_history(&(game->history));
}
