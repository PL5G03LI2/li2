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

// light‐weight check for row/col duplicates and cross→white adjacency
static bool detect_conflict(Tab *tab)
{
    int h = tab->height, w = tab->width;
    // rows
    for (int y = 0; y < h; y++)
    {
        bool seen[26] = {0};
        for (int x = 0; x < w; x++)
        {
            Piece *p = &tab->data[calc_index(tab, x, y)];
            if (isupper(p->c))
            {
                int idx = p->c - 'A';
                if (seen[idx])
                    return true;
                seen[idx] = true;
            }
        }
    }
    // columns
    for (int x = 0; x < w; x++)
    {
        bool seen[26] = {0};
        for (int y = 0; y < h; y++)
        {
            Piece *p = &tab->data[calc_index(tab, x, y)];
            if (isupper(p->c))
            {
                int idx = p->c - 'A';
                if (seen[idx])
                    return true;
                seen[idx] = true;
            }
        }
    }
    // crossed cell must have all ortho neighbours white
    const iVec2 D[4] = {{1, 0}, {-1, 0}, {0, 1}, {0, -1}};
    for (int i = 0; i < h * w; i++)
    {
        Piece *p = &tab->data[i];
        if (!p->marked)
            continue;
        iVec2 pos = calc_pos(tab, i);
        for (int d = 0; d < 4; d++)
        {
            int nx = pos.x + D[d].x, ny = pos.y + D[d].y;
            if (!assert_pos(tab, nx, ny))
                continue;
            Piece *q = &tab->data[calc_index(tab, nx, ny)];
            if (!isupper(q->c))
                return true;
        }
    }
    return false;
}

// Function to handle the help command
// Helper function to record a command in history
void record_hint_action(Game *game, CommandType cmd_type, int x, int y)
{
    char coordstr[16];
    write_coordinate((iVec2){x, y}, coordstr);

    ParsedCommand *hc = malloc(sizeof *hc);
    hc->type = cmd_type;
    hc->tokens = calloc(2, sizeof(char *));
    hc->tokens[1] = strdup(coordstr);
    hc->track = false;
    game->history = push_history(game->history, hc);
}

// Rule 1: cross duplicate for a white cell
bool apply_rule_cross_duplicate(Game *game)
{
    Tab *tab = game->tabuleiro;
    int max_i = tab->height * tab->width;

    bool *violated = calloc(max_i, sizeof(bool));

    for (int i = 0; i < max_i; i++)
    {
        Piece *p = &tab->data[i];

        if (islower(tab->data[i].c) || tab->data[i].marked)
            continue;

        check_row(tab, p, i, violated);
        check_column(tab, p, i, violated);
    }

    for (int i = 0; i < max_i; i++)
    {
        iVec2 pos = calc_pos(tab, i);

        if (!violated[i])
            continue;

        toggle_marked(tab, pos.x, pos.y);

        record_hint_action(game, CMD_CROSS, pos.x, pos.y);

        char tmp[16];
        write_coordinate(pos, tmp);

        snprintf(game->info_str, 128, "Hint: crossed at %s", tmp);

        return true;
    }

    free(violated);

    return false;
}

// Rule 2: paint neighbor of a marked cell white
bool apply_rule_paint_neighbors(Game *game)
{
    Tab *tab = game->tabuleiro;
    int h = tab->height, w = tab->width;

    for (int i = 0; i < h * w; i++)
    {
        if (!tab->data[i].marked)
            continue;

        iVec2 pos = calc_pos(tab, i);
        const iVec2 dirs[4] = {{1, 0}, {0, 1}, {-1, 0}, {0, -1}};

        for (int d = 0; d < 4; d++)
        {
            iVec2 np = add_vec2(pos, dirs[d]);
            if (!assert_pos(tab, np.x, np.y))
                continue;

            int ni = calc_index(tab, np.x, np.y);
            if (isupper(tab->data[ni].c))
                continue;

            // Apply white and record for undo
            toggle_branco(tab, np.x, np.y);
            record_hint_action(game, CMD_WHITE, np.x, np.y);

            char tmp[16];
            write_coordinate(np, tmp);

            snprintf(game->info_str, 128, "Hint: painted at %s", tmp);

            return true;
        }
    }

    return false;
}

// Function to handle the help command
int handle_help(Game *game)
{
    // Try applying Rule 1 (cross duplicates)
    if (apply_rule_cross_duplicate(game))
        return 0;

    // Try applying Rule 2 (paint neighbors of marked cells)
    if (apply_rule_paint_neighbors(game))
        return 0;

    // No single inference applicable
    snprintf(game->info_str, 128, "No hints available");
    return 1;
}

// Help all command: repeat single hints until none available
int handle_help_all(Game *game)
{
    while (!handle_help(game))
        continue;

    return 0;
}

// Recursive solver: tries white vs cross for first unknown cell
bool backtrack_solve(Game *game)
{
    Tab *tab = game->tabuleiro;
    int max_i = tab->height * tab->width;

    // 1) exhaustively apply your two deterministic rules
    bool changed;
    do
    {
        changed = apply_rule_cross_duplicate(game);
        changed |= apply_rule_paint_neighbors(game);
    } while (changed);

    // 2) if we already violate a simple constraint, backtrack
    if (detect_conflict(tab))
        return false;

    // 3) pick first unassigned cell (lower‐case and not marked) and branch white vs cross
    for (int i = 0; i < max_i; i++)
    {
        Piece *p = &tab->data[i];
        if (islower(p->c) && !p->marked)
        {
            // backup
            Piece *backup = malloc(max_i * sizeof *backup);
            memcpy(backup, tab->data, max_i * sizeof *backup);

            iVec2 pos = calc_pos(tab, i);

            // TRY white
            toggle_branco(tab, pos.x, pos.y);
            if (backtrack_solve(game))
            {
                free(backup);
                return true;
            }

            // restore & TRY cross
            memcpy(tab->data, backup, max_i * sizeof *backup);
            toggle_marked(tab, pos.x, pos.y);
            if (backtrack_solve(game))
            {
                free(backup);
                return true;
            }

            // restore & fail
            memcpy(tab->data, backup, max_i * sizeof *backup);
            free(backup);
            return false;
        }
    }

    // no more unassigned => solved
    return true;
}

// Solve command: apply backtracking solver and report
int handle_solve(Game *game)
{
    if (backtrack_solve(game))
        snprintf(game->info_str, 128, "Solved");
    else
        snprintf(game->info_str, 128, "Could not solve");
    return 0;
}
