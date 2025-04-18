#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <ncurses.h>

#include "helpers/strs.h"
#include "helpers/history.h"
#include "jogo/comandos.h"
#include "jogo/tabuleiro.h"
#include "types.h"

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
    pos += sprintf(buffer + pos, "%d", coord.y + 1);

    buffer[pos] = '\0';
    return buffer;
}

ParsedCommand *deep_copy_cmd(ParsedCommand *cmd)
{
    ParsedCommand *new_cmd = (ParsedCommand *)malloc(sizeof(ParsedCommand));
    if (!new_cmd)
        return NULL;

    new_cmd->type = cmd->type;
    new_cmd->track = cmd->track;

    new_cmd->tokens = (char **)calloc(2, sizeof(char *));
    if (!new_cmd->tokens)
    {
        free(new_cmd);
        return NULL;
    }

    for (int i = 0; i < 2; i++)
    {
        if (cmd->tokens[i])
            new_cmd->tokens[i] = strdup(cmd->tokens[i]);
        else
            new_cmd->tokens[i] = strdup("");

        if (!new_cmd->tokens[i])
        {
            for (int j = 0; j < i; j++)
                free(new_cmd->tokens[j]);
            free(new_cmd->tokens);
            free(new_cmd);
            return NULL;
        }
    }

    return new_cmd;
}

int tokenize_cmd(char *command, char **tokens)
{
    int tokenc = 0;
    char *token;

    token = strtok(command, " ");

    while (token && tokenc < 2)
    {
        tokens[tokenc] = strdup(token);
        token = strtok(NULL, " ");
        tokenc++;
    }

    return tokenc;
}

void reset_cmd_tokens(ParsedCommand *cmd)
{
    if (!cmd->tokens)
        return;

    for (int i = 0; i < 2; i++)
    {
        free(cmd->tokens[i]);
        cmd->tokens[i] = NULL;
    }
    free(cmd->tokens);
    cmd->tokens = NULL;
}

void reset_cmd(ParsedCommand *cmd)
{
    cmd->type = CMD_INVALID;
    cmd->track = false;
    reset_cmd_tokens(cmd);
}

int await_command(char *command)
{
    if (getnstr(command, 256))
        return 1;

    move(0, 0);
    printw(command);
    return 0;
}

int parse_command(Tab *tab, char *command, ParsedCommand *result)
{
    reset_cmd(result);
    char *trimmed_command = trim_str(command);
    char **tokens = (char **)calloc(2, sizeof(char *));
    int tokenc = tokenize_cmd(trimmed_command, tokens);
    free(trimmed_command);
    bool expect_coords = false;

    if (!tokenc)
    {
        reset_cmd(result);
        result->type = CMD_CONTINUE;
        free(tokens);

        return 0;
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
            result->track = true;
            break;
        case 'r':
            result->type = CMD_CROSS;
            expect_coords = true;
            result->track = true;
            break;
        case 'v':
            result->type = CMD_VERIFY;
            break;
        case 'a':
            result->type = CMD_HELP;
            result->track = true;
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

        result->tokens = tokens;
        if (expect_coords && tokenc == 1)
        {
            result->tokens[1] = (char *)calloc(32, sizeof(char));
            write_coordinate(tab->sel_piece, result->tokens[1]);
        }
    }

    return 0;
}

int run_command(ParsedCommand *cmd, Tab *tab, TabHistory **history)
{
    switch (cmd->type)
    {
    case CMD_SAVE:
        return salvar_tabuleiro(tab, cmd->tokens[1]);

    case CMD_LOAD:
        return carregar_tabuleiro(tab, cmd->tokens[1]);

    case CMD_SELECT:
    {
        iVec2 preferred_selection = read_coordinate(cmd->tokens[0]);
        if (assert_pos(tab, preferred_selection.x, preferred_selection.y))
            tab->sel_piece = preferred_selection;
        else
            printw("Position out of bounds!\n");
        return 0;
    }

    case CMD_WHITE:
    {
        iVec2 coord = read_coordinate(cmd->tokens[1]);
        toggle_branco(tab, coord.x, coord.y);
        return 0;
    }

    case CMD_CROSS:
    {
        iVec2 coord = read_coordinate(cmd->tokens[1]);
        toggle_marked(tab, coord.x, coord.y);
        return 0;
    }

    case CMD_VERIFY:
        if (validar_tabuleiro(tab))
            printw("Valid tabuleiro.");
        else
            printw("Invalid.");
        return 0;

    case CMD_UNDO:
    {
        if (*history == NULL)
        {
            printw("No more moves to undo.\n");
            return 0;
        }
        ParsedCommand *lastCmd = pop_history(history);
        if (lastCmd != NULL)
        {
            int res = undo_command(lastCmd, tab);
            reset_cmd(lastCmd);
            free(lastCmd);
            return res;
        }
        return 0;
    }

    case CMD_EXIT:
        return 0;

    case CMD_HELP:
        return 1;

    case CMD_HELP_ALL:
        return 1;

    case CMD_SOLVE:
        return 1;

    case CMD_CONTINUE:
        return 0;

    default:
        return 1;
    }
}

int undo_command(ParsedCommand *cmd, Tab *tab)
{
    switch (cmd->type)
    {
    case CMD_WHITE:
    {
        iVec2 coord = read_coordinate(cmd->tokens[1]);
        toggle_branco(tab, coord.x, coord.y);
        return 0;
    }
    case CMD_CROSS:
    {
        iVec2 coord = read_coordinate(cmd->tokens[1]);
        toggle_marked(tab, coord.x, coord.y);
        return 0;
    }
    default:
    {
        printw("Command not permitted to undo!");
        return 1;
    }
    }
    return 0;
}