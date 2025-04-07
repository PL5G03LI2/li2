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
    {
        printf("Error: failed fgets.");
        return 1;
    }
    command[strcspn(command, "\n")] = 0;
    return 0;
}

int tokenize_cmd(char *command, char **args)
{
    const int token_capacity = 32;
    int tokenc = 0;
    int len = strlen(command);
    char *current_token = NULL;
    int token_length = 0;

    while (*command == ' ')
    {
        command++;
        len--;
    }

    if (len == 0)
        return 1;

    current_token = (char *)calloc(token_capacity, sizeof(char));
    if (!current_token)
        return 1;

    for (int i = 0; i < len; i++)
    {
        if (command[i] == ' ')
        {
            if (token_length > 0)
            {
                // store finished token
                current_token[token_length] = '\0';
                args[tokenc++] = current_token;

                current_token = (char *)calloc(token_capacity, sizeof(char));
                if (!current_token)
                {
                    while (tokenc--)
                    {
                        free(args[tokenc]);
                        args[tokenc] = NULL;
                    }
                    return 1;
                }
                token_length = 0;
            }

            // skip consecutive spaces
            while (i + 1 < len && command[i + 1] == ' ')
                i++;
        }
        else if (token_length < token_capacity - 1)
        {
            current_token[token_length++] = command[i];
        }
        else
        {
            // too long
            free(current_token);
            while (tokenc--)
            {
                free(args[tokenc]);
                args[tokenc] = NULL;
            }
            return 1;
        }
    }

    // last token
    if (token_length > 0)
    {
        current_token[token_length] = '\0';
        args[tokenc++] = current_token;
    }
    else
    {
        free(current_token);
    }

    return tokenc;
}

int parse_command(Tab *tab, char *command, ParsedCommand *result)
{
    char **tokens = (char **)calloc(32, sizeof(char *));
    int tokenc = tokenize_cmd(command, tokens);
    bool expects_coord = false;

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

        free(tokens);

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
    }

    if (tokenc == 1 && expects_coord)
    {
        char *pos = (char *)calloc(3, sizeof(char));
        sprintf(pos, "%c%d", tab->sel_piece.x + 'a', tab->sel_piece.y);
        result->args[0] = pos;
    }
    else
        result->args[0] = tokens[1];

    free(tokens);

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
        tab->sel_piece.y = cmd->args[0][1] - '0';
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
