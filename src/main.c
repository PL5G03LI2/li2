#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <curses.h>

#include "jogo/tabuleiro.h"
#include "jogo/comandos.h"
#include "helpers/history.h"
#include "helpers/memory.h"

int init_all(Tab **tabuleiro, char **cmd_str, ParsedCommand **cmd, TabHistory **hist)
{
    *tabuleiro = initialize_tabuleiro();
    if (*tabuleiro == NULL)
        return 1;

    *cmd_str = (char *)calloc(256, sizeof(char));
    if (*cmd_str == NULL)
    {
        free_all(*tabuleiro, NULL, NULL, NULL);
        return 1;
    }

    *cmd = (ParsedCommand *)malloc(sizeof(ParsedCommand));
    if (*cmd == NULL)
    {
        free_all(*tabuleiro, *cmd_str, NULL, NULL);
        return 1;
    }
    reset_cmd(*cmd);

    (*cmd)->tokens = (char **)calloc(2, sizeof(char *));

    if ((*cmd)->tokens == NULL)
    {
        free_all(*tabuleiro, *cmd_str, *cmd, NULL);
        return 1;
    }

    for (int i = 0; i < 2; i++)
    {
        (*cmd)->tokens[i] = (char *)calloc(32, sizeof(char));
        if ((*cmd)->tokens[i] == NULL)
        {
            free_all(*tabuleiro, *cmd_str, *cmd, hist);
            return 1;
        }
    }

    *hist = NULL;

    return 0;
}

int repl(Tab *tabuleiro, char *cmd_str, ParsedCommand *cmd, TabHistory **hist)
{
    while (true)
    {
        if (tabuleiro->data == NULL)
        {
            printf("Awaiting load command...\nHint: l <save_file>\n");
        }

        if (await_command(cmd_str))
        {
            printf("Failed to read command\n");
            free_all(tabuleiro, cmd_str, cmd, hist);
            return 1;
        }

        if (parse_command(tabuleiro, cmd_str, cmd))
        {
            printf("Failed to parse command\n");
            free_all(tabuleiro, cmd_str, cmd, hist);
            return 1;
        }

        if (cmd->track)
        {
            *hist = push_history(*hist, cmd);
        }

        if (cmd->type == CMD_EXIT)
            break;

        if (cmd->type == CMD_INVALID || (cmd->type != CMD_LOAD && tabuleiro->data == NULL))
        {
            printf("Invalid command.\n");
            continue;
        }

        if (run_command(cmd, tabuleiro, hist))
        {
            printf("Failed to run command.\n");
            free_all(tabuleiro, cmd_str, cmd, hist);
            return 1;
        }

        if (cmd->type != CMD_EXIT)
            print_tab(tabuleiro);
    }

    return 0;
}

int main(void)
{
    initscr();

    Tab *tabuleiro;
    TabHistory *hist;
    char *cmd_str;
    ParsedCommand *cmd;

    init_all(&tabuleiro, &cmd_str, &cmd, &hist);

    if (repl(tabuleiro, cmd_str, &cmd, &hist))
        return 1;

    // Nothing more to do, clean up.
    free_all(tabuleiro, cmd_str, &cmd, &hist);
    endwin();
    return 0;
}
