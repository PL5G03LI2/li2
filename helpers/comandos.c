#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "strs.h"
#include "comandos.h"
#include "../types/types.h"
#include "../jogo/tabuleiro.h"

void free_tokens(char **tokens, int count)
{
    if (!tokens)
        return;

    for (int i = 0; i < count; i++)
    {
        if (tokens[i])
        {
            free(tokens[i]);
            tokens[i] = NULL;
        }
    }
    free(tokens);
}

int await_command(char *command)
{
    if (!fgets(command, 256, stdin))
    {
        printf("Error: failed fgets.");
        return 1;
    }
    command[strcspn(command, "\n")] = 0;
    return 0;
}

int tokenize_cmd(char *command, char **args)
{
    int tokenc = 0;
    char *token;

    token = strtok(command, " ");

    while (token && tokenc < 3)
    {
        args[tokenc] = strdup(token);
        token = strtok(NULL, " ");
        tokenc++;
    }

    return tokenc;
}

int parse_command(Tab *tab, char *command, ParsedCommand *result)
{
    char **tokens = (char **)calloc(3, sizeof(char *));
    int tokenc = tokenize_cmd(command, tokens);
    bool expects_coord = false;

    if (!tokenc)
    {
        free(tokens);
        result->type = CMD_INVALID;
        return 1;
    }

    int fst_tok_len = strlen(tokens[0]);

    if (fst_tok_len == 2 && isLower(tokens[0][0]) && isdigit(tokens[0][1])) // SELECT COMMAND
    {
        result->type = CMD_SELECT;
        result->args[0] = strdup(tokens[0]); // Make a copy
        free_tokens(tokens, tokenc);         // Free all tokens
        return 0;
    }

    switch (tokens[0][0])
    {
        // commands with no arguments
    case 'v':
        result->type = CMD_VERIFY;
        break;

    case 'a':
        result->type = CMD_HELP;
        break;

    case 'A':
        result->type = CMD_HELP_ALL;
        break;

    case 'R':
        result->type = CMD_SOLVE;
        break;

    case 'd':
        result->type = CMD_UNDO;
        break;

    case 's':
        result->type = CMD_EXIT;
        break;

    // commands with 1 argument
    case 'g':
        result->type = CMD_SAVE;
        break;

    case 'l':
        result->type = CMD_LOAD;
        break;

    case 'b':
        result->type = CMD_WHITE;
        expects_coord = true;
        break;

    case 'r':
        result->type = CMD_CROSS;
        expects_coord = true;
        break;
    default:
        result->type = CMD_INVALID;
        free_tokens(tokens, tokenc);
        return 1;
    }

    if (tokenc == 1 && expects_coord)
    {
        char *pos = (char *)calloc(3, sizeof(char));
        sprintf(pos, "%c%d", tab->sel_piece.x + 'a', tab->sel_piece.y);
        result->args[0] = pos;
        free_tokens(tokens, tokenc);
    }
    else
        result->args[0] = tokens[1];

    return 0;
}

int run_command(ParsedCommand *cmd, Tab *tab)
{
    switch (cmd->type)
    {
    case CMD_SAVE:
        return salvar_tabuleiro(tab, cmd->args[0]);

    case CMD_LOAD:
        return carregar_tabuleiro(tab, cmd->args[0]);

    case CMD_SELECT:
    {
        tab->sel_piece.x = cmd->args[0][0] - 'a';
        tab->sel_piece.y = cmd->args[0][1] - '1';
        return 0;
    }

    case CMD_WHITE:
    {
        int x = cmd->args[0][0] - 'a';
        int y = cmd->args[0][1] - '0';
        toggle_branco(tab, x, y);
        return 0;
    }

    case CMD_CROSS:
    {
        int x = cmd->args[0][0] - 'a';
        int y = cmd->args[0][1] - '0';
        toggle_marked(tab, x, y);
        return 0;
    }

    case CMD_VERIFY:
        return validar_tabuleiro(tab);

    case CMD_EXIT:
        return 0;

    default:
        return 1;
    }
}

int carregar_tabuleiro(Tab *tab, const char *filename)
{
    FILE *file = fopen(filename, "r");
    if (!file)
        return 1;

    int height, width;
    fscanf(file, "%d %d", &height, &width);

    tab->height = height;
    tab->width = width;
    tab->data = (Piece *)calloc(height * width, sizeof(Piece));

    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            fscanf(file, " %c", &tab->data[calc_index(tab, j, i)].c);
            tab->data[calc_index(tab, j, i)].marked = 0;
        }
    }

    fclose(file);
    return 0;
}

int salvar_tabuleiro(Tab *tab, const char *filename)
{
    FILE *file = fopen(filename, "w");
    if (!file)
        return 1;

    fprintf(file, "%d %d\n", tab->height, tab->width);
    for (int i = 0; i < tab->height; i++)
    {
        for (int j = 0; j < tab->width; j++)
        {
            fprintf(file, "%c", get_elem(tab, i, j));
        }
        fprintf(file, "\n");
    }

    fclose(file);
    return 0;
}

bool validar_tabuleiro(Tab *tab)
{
    for (int i = 0; i < tab->height; i++)
    {
        for (int j = 0; j < tab->width; j++)
        {
            char c = get_elem(tab, i, j);

            if (c == '#')
                continue;

            for (int k = 0; k < tab->width; k++)
            {
                if (k != j && get_elem(tab, i, k) == c)
                    return false;
            }

            for (int k = 0; k < tab->height; k++)
            {
                if (k != i && get_elem(tab, k, j) == c)
                    return false;
            }
        }
    }

    return true;
}
