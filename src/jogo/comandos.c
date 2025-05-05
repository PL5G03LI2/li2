#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <ncurses.h>
#include <stdbool.h>

#include "helpers/strs.h"
#include "helpers/history.h"
#include "helpers/coords.h"

#include "jogo/saves.h"
#include "jogo/comandos.h"
#include "jogo/tabuleiro.h"
#include "types.h"

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

int await_command(WIN cmd_win, char *command)
{
    if (wgetnstr(cmd_win.win, command, 16))
        return 1;

    return 0;
}

int parse_command(Game *game)
{
    // reset to default values
    reset_cmd(game->cmd);

    char *trimmed_command = trim_str(game->cmd_str);

    char **tokens = (char **)calloc(2, sizeof(char *));
    int tokenc = tokenize_cmd(trimmed_command, tokens);

    // free after tokenization
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

        return 0;
    }
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
        game->cmd->track = false; // hint operations themselves are recorded
        break;
    case 'A':
        game->cmd->type = CMD_HELP_ALL;
        game->cmd->track = false; // hint operations themselves are recorded
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

    return 0;
}

int handle_save(Game *game)
{
    return salvar_jogo(game, game->cmd->tokens[1] ? game->cmd->tokens[1] : game->save_to);
}

int handle_load(Game *game)
{
    char *new_save = strdup(game->cmd->tokens[1]);
    if (!new_save)
        return 1;

    if (!carregar_jogo(game, game->cmd->tokens[1]))
    {
        if (game->save_to)
            free(game->save_to);
        game->save_to = new_save;
        return 0;
    }

    free(new_save);
    return 1;
}

int handle_select(Game *game)
{
    iVec2 preferred_selection = read_coordinate(game->cmd->tokens[0]);
    if (assert_pos(game->tabuleiro, preferred_selection.x, preferred_selection.y))
        game->tabuleiro->sel_piece = preferred_selection;
    else
        snprintf(game->info_str, 128, "Out of bounds!");

    return 0;
}

int handle_white(Game *game)
{
    iVec2 coord = read_coordinate(game->cmd->tokens[1]);
    if (assert_pos(game->tabuleiro, coord.x, coord.y))
        toggle_branco(game->tabuleiro, coord.x, coord.y);
    else
        snprintf(game->info_str, 128, "Out of bounds!");

    return 0;
}

int handle_cross(Game *game)
{
    iVec2 coord = read_coordinate(game->cmd->tokens[1]);
    if (assert_pos(game->tabuleiro, coord.x, coord.y))
        toggle_marked(game->tabuleiro, coord.x, coord.y);
    else
        snprintf(game->info_str, 128, "Out of bounds!");

    return 0;
}

int handle_verify(Game *game)
{
    if (validar_tabuleiro(game->tabuleiro))
        snprintf(game->info_str, 128, "Valid board");
    else
        snprintf(game->info_str, 128, "Invalid board");

    return 0;
}

int handle_undo(Game *game)
{
    if (game->history == NULL)
    {
        snprintf(game->info_str, 128, "No more moves to undo.");
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

int handle_exit(Game *game)
{
    salvar_jogo(game, game->save_to);
    return 0;
}

// Hint command: apply a single inference step
int handle_help(Game *game)
{
    Tab *tab = game->tabuleiro;
    int h = tab->height, w = tab->width;
    // Rule 1: cross duplicate for a white cell
    for (int x = 0; x < h; x++)
    {
        for (int y = 0; y < w; y++)
        {
            int idx = calc_index(tab, x, y);
            if (isupper(tab->data[idx].c) && !tab->data[idx].marked)
            {
                char c = tab->data[idx].c;
                // row
                for (int yy = 0; yy < w; yy++)
                {
                    int idx2 = calc_index(tab, x, yy);
                    if (idx2 != idx && tab->data[idx2].c == tolower(c) && !tab->data[idx2].marked)
                    {
                        // apply and record as a cross command for undo
                        tab->data[idx2].marked = true;
                        {
                            char coordstr[16];
                            snprintf(coordstr, sizeof(coordstr), "%c%d", 'a' + yy, x + 1);
                            ParsedCommand *hc = malloc(sizeof *hc);
                            hc->type = CMD_CROSS;
                            hc->tokens = calloc(2, sizeof(char *));
                            hc->tokens[1] = strdup(coordstr);
                            hc->track = false;
                            game->history = push_history(game->history, hc);
                        }
                        snprintf(game->info_str, 128, "Hint: crossed at %c%d", 'a' + yy, x + 1);
                        return 0;
                    }
                }
                // column
                for (int xx = 0; xx < h; xx++)
                {
                    int idx2 = calc_index(tab, xx, y);
                    if (idx2 != idx && tab->data[idx2].c == tolower(c) && !tab->data[idx2].marked)
                    {
                        // apply and record as a cross command for undo
                        tab->data[idx2].marked = true;
                        {
                            char coordstr[16];
                            snprintf(coordstr, sizeof(coordstr), "%c%d", 'a' + y, xx + 1);
                            ParsedCommand *hc = malloc(sizeof *hc);
                            hc->type = CMD_CROSS;
                            hc->tokens = calloc(2, sizeof(char *));
                            hc->tokens[1] = strdup(coordstr);
                            hc->track = false;
                            game->history = push_history(game->history, hc);
                        }
                        snprintf(game->info_str, 128, "Hint: crossed at %c%d", 'a' + y, xx + 1);
                        return 0;
                    }
                }
            }
        }
    }
    // Rule 2: paint neighbor of a marked cell white
    for (int i = 0; i < h * w; i++)
    {
        if (tab->data[i].marked)
        {
            iVec2 pos = calc_pos(tab, i);
            const iVec2 dirs[4] = {{1, 0}, {0, 1}, {-1, 0}, {0, -1}};
            for (int d = 0; d < 4; d++)
            {
                iVec2 np = add_vec2(pos, dirs[d]);
                if (assert_pos(tab, np.x, np.y))
                {
                    int ni = calc_index(tab, np.x, np.y);
                    if (islower(tab->data[ni].c))
                    {
                        // apply and record as a white command for undo
                        tab->data[ni].c = toupper(tab->data[ni].c);
                        {
                            char coordstr[16];
                            snprintf(coordstr, sizeof(coordstr), "%c%d", 'a' + np.y, np.x + 1);
                            ParsedCommand *hc = malloc(sizeof *hc);
                            hc->type = CMD_WHITE;
                            hc->tokens = calloc(2, sizeof(char *));
                            hc->tokens[1] = strdup(coordstr);
                            hc->track = false;
                            game->history = push_history(game->history, hc);
                        }
                        snprintf(game->info_str, 128, "Hint: painted at %c%d", 'a' + np.y, np.x + 1);
                        return 0;
                    }
                }
            }
        }
    }
    // No single inference applicable
    snprintf(game->info_str, 128, "No hints available");
    return 0;
}

// Help all command: repeat single hints until none available
int handle_help_all(Game *game)
{
    while (1)
    {
        handle_help(game);
        if (strncmp(game->info_str, "Hint:", 5) != 0)
            break;
    }
    return 0;
}

// Recursive solver: tries white vs cross for first unknown cell
static bool backtrack_solve(Tab *tab)
{
    int h = tab->height, w = tab->width, total = h * w;
    // Check if all cells are decided
    bool complete = true;
    for (int i = 0; i < total; i++)
    {
        if (islower(tab->data[i].c))
        {
            complete = false;
            break;
        }
    }
    if (complete && validar_tabuleiro(tab))
        return true;
    // Find first undecided cell
    for (int i = 0; i < total; i++)
    {
        if (islower(tab->data[i].c) && !tab->data[i].marked)
        {
            char orig_c = tab->data[i].c;
            bool orig_m = tab->data[i].marked;
            // Try painting white
            tab->data[i].c = toupper(orig_c);
            tab->data[i].marked = false;
            if (validar_tabuleiro(tab) && backtrack_solve(tab))
                return true;
            // Revert
            tab->data[i].c = orig_c;
            tab->data[i].marked = orig_m;
            // Try crossing
            tab->data[i].marked = true;
            if (validar_tabuleiro(tab) && backtrack_solve(tab))
                return true;
            // Revert
            tab->data[i].marked = orig_m;
            return false;
        }
    }
    return false;
}

// Solve command: apply backtracking solver and report
int handle_solve(Game *game)
{
    if (backtrack_solve(game->tabuleiro))
        snprintf(game->info_str, 128, "Solved");
    else
        snprintf(game->info_str, 128, "Could not solve");
    return 0;
}

int run_command(Game *game)
{
    switch (game->cmd->type)
    {
    case CMD_SAVE:
        return handle_save(game);
    case CMD_LOAD:
        return handle_load(game);
    case CMD_SELECT:
        return handle_select(game);
    case CMD_WHITE:
        return handle_white(game);
    case CMD_CROSS:
        return handle_cross(game);
    case CMD_VERIFY:
        return handle_verify(game);
    case CMD_UNDO:
        return handle_undo(game);
    case CMD_EXIT:
        return handle_exit(game);
    case CMD_HELP:
        return handle_help(game);
    case CMD_HELP_ALL:
        return handle_help_all(game);
    case CMD_SOLVE:
        return handle_solve(game);
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
        return 1;
    }
    }
    return 0;
}