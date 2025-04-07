#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "./jogo/tabuleiro.h"
#include "./helpers/comandos.h"
#include "./helpers/arrays.h"

void free_all(Tab *tabuleiro, char *cmd_str, ParsedCommand *cmd)
{
    free(tabuleiro->data);
    free(tabuleiro);

    free(cmd_str);

    for (int i = 0; i < 2; i++)
        free(cmd->args[i]);

    free(cmd->args);
}

int repl(Tab *tabuleiro, char *cmd_str, ParsedCommand *cmd)
{
    while (cmd->type != CMD_EXIT)
    {
        if (tabuleiro->data == NULL)
        {
            printf("Awaiting load command...\nHint: l <save_file>\n");
        }
        if (await_command(cmd_str))
        {
            printf("Failed to read command\n");
            free_all(tabuleiro, cmd_str, cmd);
            return 1;
        }

        if (parse_command(tabuleiro, cmd_str, cmd))
        {
            printf("Failed to parse command\n");
            free_all(tabuleiro, cmd_str, cmd);
            return 1;
        }

        if (cmd->type == CMD_INVALID || (cmd->type != CMD_LOAD && tabuleiro->data == NULL))
        {
            printf("Invalid command.\n");
            continue;
        }

        if (run_command(cmd, tabuleiro))
        {
            printf("Failed to run command.\n");
            free_all(tabuleiro, cmd_str, cmd);
            return 1;
        }

        print_tab(tabuleiro);
    }

    return 0;
}

int main()
{
    Tab *tabuleiro = initialize_tabuleiro();
    if (tabuleiro == NULL)
        return 1;

    // stores last command as string
    char *cmd_str = calloc(256, sizeof(char));
    if (cmd_str == NULL)
    {
        free_all(tabuleiro, NULL, NULL);
        return 1;
    }

    ParsedCommand cmd = {CMD_INVALID, calloc(2, sizeof(char *))};
    for (int i = 0; i < 2; i++)
    {
        cmd.args[i] = calloc(32, sizeof(char));
        if (cmd.args[i] == NULL)
        {
            free_all(tabuleiro, cmd_str, &cmd);
            return 1;
        }
    }

    if (repl(tabuleiro, cmd_str, &cmd))
        return 1;

    return 0;
}