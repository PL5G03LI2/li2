#include <stdlib.h>
#include "helpers/history.h"
#include "types.h"

void free_all(Tab *tabuleiro, char *cmd_str, ParsedCommand *cmd, TabHistory **head)
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
    destroy_history(head);
}
