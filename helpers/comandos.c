#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "strs.h"
#include "comandos.h"
#include "../types/types.h"
#include "../jogo/tabuleiro.h"

void free_all_tokens(char **tokens, int count)
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

int tokenize_cmd(char *command, char **tokens)
{
    int tokenc = 0;
    char *token;

    token = strtok(command, " ");

    while (token && tokenc < 3)
    {
        tokens[tokenc] = strdup(token);
        token = strtok(NULL, " ");
        tokenc++;
    }

    return tokenc;
}

iVec2 read_coordinate(char *coord_tkn)
{
    iVec2 result;
    result.x = 0;
    result.y = 0;
    int i = 0;

    // Read letters for x-coordinate
    while (isalpha(coord_tkn[i]))
    {
        result.x = result.x * 26 + (coord_tkn[i] - 'a');
        i++;
    }

    // Read numbers for y-coordinate
    while (isdigit(coord_tkn[i]))
    {
        result.y = result.y * 10 + (coord_tkn[i] - '1');
        i++;
    }

    return result;
}

char *write_coordinate(iVec2 coord, char *buffer)
{
    int x = coord.x;
    int pos = 0;

    if (x >= 26)
    {
        buffer[pos++] = 'a' + (x / 26 - 1);
        x = x % 26;
    }
    buffer[pos++] = 'a' + x;

    // Handle y-coordinate (convert to numbers)
    pos += sprintf(buffer + pos, "%d", coord.y);

    buffer[pos] = '\0';
    return buffer;
}

void reset_cmd(ParsedCommand *cmd)
{
    cmd->type = CMD_INVALID;
    for (int i = 0; i < 2; i++)
    {
        free(cmd->tokens[i]);
        cmd->tokens[i] = NULL;
    }
    free(cmd->tokens);
    cmd->tokens = NULL;
}

int parse_command(Tab *tab, char *command, ParsedCommand *result)
{
    reset_cmd(result);
    char **tokens = (char **)calloc(2, sizeof(char *));
    int tokenc = tokenize_cmd(command, tokens);
    bool expect_coords = false;

    if (!tokenc)
    {
        free_all_tokens(tokens, 3);

        return 1;
    }

    if (strlen(tokens[0]) > 1) // select command
    {
        result->type = CMD_SELECT;
        result->tokens = tokens;
    }
    else
    {
        switch (tokens[0][0])
        {
        case 'g':
            result->type = CMD_SAVE;
            break;
        case 'l':
            result->type = CMD_LOAD;
            break;
        case 'b':
            result->type = CMD_WHITE;
            expect_coords = true;
            break;
        case 'r':
            result->type = CMD_CROSS;
            expect_coords = true;
            break;
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

        if (expect_coords && tokenc == 1)
        {
            result->tokens[0] = (char *)calloc(32, sizeof(char));
            write_coordinate(tab->sel_piece, result->tokens[0]);
        }

        result->tokens = tokens;
    }

    return 0;
}

int run_command(ParsedCommand *cmd, Tab *tab)
{
    switch (cmd->type)
    {
    case CMD_SAVE:
        return salvar_tabuleiro(tab, cmd->tokens[1]);

    case CMD_LOAD:
        return carregar_tabuleiro(tab, cmd->tokens[1]);

    case CMD_SELECT:
    {
        tab->sel_piece = read_coordinate(cmd->tokens[0]);
        return 0;
    }

    case CMD_WHITE:
    {
        int x = cmd->tokens[1][0] - 'a';
        int y = cmd->tokens[1][1] - '0';
        toggle_branco(tab, x, y);
        return 0;
    }

    case CMD_CROSS:
    {
        int x = cmd->tokens[1][0] - 'a';
        int y = cmd->tokens[1][1] - '0';
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
