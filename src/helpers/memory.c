#include <stdlib.h>
#include "helpers/history.h"
#include "types.h"

void free_game(Game *game)
{
    if (game->tabuleiro && game->tabuleiro->data)
        free(game->tabuleiro->data);
    free(game->tabuleiro);

    if (game->cmd_str)
        free(game->cmd_str);

    if (game->cmd && game->cmd->tokens)
    {
        for (int i = 0; i < 2; i++)
            free(game->cmd->tokens[i]);

        free(game->cmd->tokens);
        free(game->cmd);
    }

    destroy_history(&(game->history));
}
