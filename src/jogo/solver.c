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

// Function to handle the help command
// Helper function to record a command in history
static void record_hint_action(Game *game, CommandType cmd_type, int x, int y) {
    char coordstr[16];
    snprintf(coordstr, sizeof(coordstr), "%c%d", 'a' + y, x + 1);
    ParsedCommand *hc = malloc(sizeof *hc);
    hc->type = cmd_type;
    hc->tokens = calloc(2, sizeof(char *));
    hc->tokens[1] = strdup(coordstr);
    hc->track = false;
    game->history = push_history(game->history, hc);
}

// Rule 1: cross duplicate for a white cell
static bool apply_rule_cross_duplicate(Game *game)
{
    Tab *tab = game->tabuleiro;
    int h = tab->height, w = tab->width;
    
    for (int x = 0; x < h; x++)
    {
        for (int y = 0; y < w; y++)
        {
            int idx = calc_index(tab, x, y);
            if (isupper(tab->data[idx].c) && !tab->data[idx].marked)
            {
                char c = tab->data[idx].c;
                
                // Check row for duplicates
                for (int yy = 0; yy < w; yy++)
                {
                    int idx2 = calc_index(tab, x, yy);
                    if (idx2 != idx && tab->data[idx2].c == tolower(c) && !tab->data[idx2].marked)
                    {
                        // Apply cross and record for undo
                        tab->data[idx2].marked = true;
                        record_hint_action(game, CMD_CROSS, x, yy);
                        snprintf(game->info_str, 128, "Hint: crossed at %c%d", 'a' + yy, x + 1);
                        return true;
                    }
                }
                
                // Check column for duplicates
                for (int xx = 0; xx < h; xx++)
                {
                    int idx2 = calc_index(tab, xx, y);
                    if (idx2 != idx && tab->data[idx2].c == tolower(c) && !tab->data[idx2].marked)
                    {
                        // Apply cross and record for undo
                        tab->data[idx2].marked = true;
                        record_hint_action(game, CMD_CROSS, xx, y);
                        snprintf(game->info_str, 128, "Hint: crossed at %c%d", 'a' + y, xx + 1);
                        return true;
                    }
                }
            }
        }
    }
    return false;
}

// Rule 2: paint neighbor of a marked cell white
static bool apply_rule_paint_neighbors(Game *game)
{
    Tab *tab = game->tabuleiro;
    int h = tab->height, w = tab->width;
    
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
                        // Apply white and record for undo
                        tab->data[ni].c = toupper(tab->data[ni].c);
                        record_hint_action(game, CMD_WHITE, np.x, np.y);
                        snprintf(game->info_str, 128, "Hint: painted at %c%d", 'a' + np.y, np.x + 1);
                        return true;
                    }
                }
            }
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
    validar_tabuleiro(game->tabuleiro);
    return 0;
}
