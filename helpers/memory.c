#include <stdlib.h>
#include "../types/types.h"

void free_all(Tab *tabuleiro, char *cmd_str, ParsedCommand *cmd)
{
    if (tabuleiro->data)
        free(tabuleiro->data);
    free(tabuleiro);

    free(cmd_str);

    if (!cmd->tokens)
        return;
    for (int i = 0; i < 2; i++)
        free(cmd->tokens[i]);

    free(cmd->tokens);
}
