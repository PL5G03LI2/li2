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

    // couldn't find alpha char, return default.
    if (!isalpha(coord_tkn[i]))
        return result;

    // Read letters for x-coordinate
    while (isalpha(coord_tkn[i]))
    {
        result.x = result.x * 26 + (toLower(coord_tkn[i]) - 'a' + 1);
        i++;
    }

    // couldn't find a digit next, returns default.
    if (!isdigit(coord_tkn[i]))
    {
        result.x = 0;
        return result;
    }

    // Read numbers for y-coordinate
    while (isdigit(coord_tkn[i]))
    {
        result.y = result.y * 10 + (coord_tkn[i] - '0');
        i++;
    }

    // fix indexing
    result.x -= 1;
    result.y -= 1;

    return result;
}

char *write_coordinate(iVec2 coord, char *buffer)
{
    int pos = 0;
    int x = coord.x + 1;
    char letters[8] = {0};
    int letter_count = 0;

    while (x > 0)
    {
        int remainder = x % 26;
        if (remainder == 0)
        {
            remainder = 26;
            x -= 1;
        }
        letters[letter_count++] = 'a' + remainder - 1;
        x = x / 26;
    }

    for (int i = letter_count - 1; i >= 0; i--)
        buffer[pos++] = letters[i];

    // Write y coordinate
    sprintf(buffer + pos, "%d", coord.y + 1);

    return buffer;
}

ParsedCommand *deep_copy_cmd(ParsedCommand *cmd)
{
    if (cmd == NULL)
        return NULL;

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

int parse_command(Game *game)
{
    reset_cmd(game->cmd);
    char *trimmed_command = trim_str(game->cmd_str);
    char **tokens = (char **)calloc(2, sizeof(char *));
    int tokenc = tokenize_cmd(trimmed_command, tokens);
    free(trimmed_command);
    bool expect_coords = false;

    if (!tokenc)
    {
        reset_cmd(game->cmd);
        game->cmd->type = CMD_CONTINUE;
        free(tokens);

        return 0;
    }

    if (strlen(tokens[0]) > 1) // select command
    {
        game->cmd->type = CMD_SELECT;
        game->cmd->tokens = tokens;
    }
    else
    {
        switch (tokens[0][0])
        {
        case 'g':
            game->cmd->type = CMD_SAVE;
            break;
        case 'l':
            game->cmd->type = CMD_LOAD;
            break;
        case 'b':
            game->cmd->type = CMD_WHITE;
            expect_coords = true;
            game->cmd->track = true;
            break;
        case 'r':
            game->cmd->type = CMD_CROSS;
            expect_coords = true;
            game->cmd->track = true;
            break;
        case 'v':
            game->cmd->type = CMD_VERIFY;
            break;
        case 'a':
            game->cmd->type = CMD_HELP;
            game->cmd->track = true;
            break;
        case 'A':
            game->cmd->type = CMD_HELP_ALL;
            break;
        case 'R':
            game->cmd->type = CMD_SOLVE;
            break;
        case 'd':
            game->cmd->type = CMD_UNDO;
            break;
        case 's':
            game->cmd->type = CMD_EXIT;
            break;
        }

        game->cmd->tokens = tokens;
        if (expect_coords && tokenc == 1)
        {
            game->cmd->tokens[1] = (char *)calloc(32, sizeof(char));
            write_coordinate(game->tabuleiro->sel_piece, game->cmd->tokens[1]);
        }
    }

    return 0;
}

int run_command(Game *game)
{
    switch (game->cmd->type)
    {
    case CMD_SAVE:
        return salvar_jogo(game, game->cmd->tokens[1]);

    case CMD_LOAD:
        return carregar_jogo(game, game->cmd->tokens[1]);

    case CMD_SELECT:
    {
        iVec2 preferred_selection = read_coordinate(game->cmd->tokens[0]);
        if (assert_pos(game->tabuleiro, preferred_selection.x, preferred_selection.y))
            game->tabuleiro->sel_piece = preferred_selection;
        else
            printw("Position out of bounds!\n");
        return 0;
    }

    case CMD_WHITE:
    {
        iVec2 coord = read_coordinate(game->cmd->tokens[1]);
        toggle_branco(game->tabuleiro, coord.x, coord.y);
        return 0;
    }

    case CMD_CROSS:
    {
        iVec2 coord = read_coordinate(game->cmd->tokens[1]);
        toggle_marked(game->tabuleiro, coord.x, coord.y);
        return 0;
    }

    case CMD_VERIFY:
        if (validar_tabuleiro(game->tabuleiro))
            printw("Valid tabuleiro.");
        else
            printw("Invalid.");
        return 0;

    case CMD_UNDO:
    {
        if (game->history == NULL)
        {
            printw("No more moves to undo.");
            return 0;
        }
        ParsedCommand *lastCmd = pop_history(&(game->history));
        if (lastCmd != NULL)
        {
            int res = undo_command(lastCmd, game->tabuleiro);
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