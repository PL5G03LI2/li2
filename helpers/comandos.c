#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "strs.h"
#include "comandos.h"
#include "../types/types.h"
#include "../jogo/tabuleiro.h"

int await_command(char *command)
{
    if (!fgets(command, 256, stdin))
        return 1;
    command[strcspn(command, "\n")] = 0;
    return 0;
}

int tokenize_cmd(char *command, char **args)
{
    int token_capacity = 32;
    int argc = 0;

    char *token = (char *)calloc(token_capacity, sizeof(char));
    if (token == NULL)
        return 0;

    int len = strlen(command);
    int token_length_counter = 0;

    for (int i = 0; i < len; i++)
    {

        if (command[i] == ' ')
        {
            args[argc] = token;

            if (args[argc] == NULL)
            {
                while (argc--)
                    free(args[argc]);
                break;
            }
            argc++;

            token = (char *)calloc(token_capacity, sizeof(char));
            token_length_counter = 0;
        }
        else if (token_length_counter < token_capacity - 1)
        {
            token[token_length_counter] = command[i];
            token[token_length_counter + 1] = '\0';
            token_length_counter++;
        }
        else
        {
            while (argc--)
                free(args[argc]);
            free(token);
            return 0;
        }
    }

    free(token);

    return argc;
}

int parse_command(char *command, ParsedCommand *result)
{
    char **tokens = (char **)calloc(32, sizeof(char *));
    int tokenc = tokenize_cmd(command, tokens);

    if (!tokenc)
    {
        result->type = CMD_INVALID;
        return 1;
    }

    int fst_tok_len = strlen(tokens[0]);

    if (fst_tok_len == 2 && isLower(tokens[0][0]) && isdigit(tokens[0][1])) // SELECT COMMAND
    {
        result->type = CMD_SELECT;
        result->args[0] = tokens[0];
    }

    // commands with no arguments
    if (tokenc == 1)
        switch (tokens[0][1])
        {
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
        }

    // commands with 1 argument
    if (tokenc == 2)
    {
        switch (tokens[0][1])
        {
        case 'g':
            result->type = CMD_SAVE;
            break;

        case 'l':
            result->type = CMD_LOAD;
            break;

        case 'b':
            result->type = CMD_WHITE;
            break;

        case 'r':
            result->type = CMD_CROSS;
            break;
        }

        result->args[0] = tokens[1];
    }

    free(tokens);

    return 0;
}

int run_command(ParsedCommand *cmd, Tab **tab)
{
    switch (cmd->type)
    {
    case CMD_SAVE:
        return salvar_tabuleiro(*tab, cmd->args[0]);

    case CMD_LOAD:
        return carregar_tabuleiro(tab, cmd->args[0]);

    case CMD_WHITE:
    {
        int x = cmd->args[0][0] - 'a';
        int y = cmd->args[0][1] - '0';
        toggle_branco(*tab, x, y);
        return 0;
    }

    case CMD_CROSS:
    {
        int x = cmd->args[0][0] - 'a';
        int y = cmd->args[0][1] - '0';
        toggle_marked(*tab, x, y);
        return 0;
    }

    case CMD_VERIFY:
        return validar_tabuleiro(*tab);

    case CMD_EXIT:
        return 0;

    default:
        return 1;
    }
}

int carregar_tabuleiro(Tab **tab, const char *filename)
{
    FILE *file = fopen(filename, "r");
    if (!file)
        return 1;

    int height, width;
    fscanf(file, "%d %d", &height, &width);

    *tab = malloc(sizeof(Tab));
    (*tab)->height = height;
    (*tab)->width = width;
    (*tab)->data = malloc(height * width * sizeof(Piece));

    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            fscanf(file, " %c", &(*tab)->data[i * width + j].c);
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
