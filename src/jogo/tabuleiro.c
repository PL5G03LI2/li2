#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ncurses.h>
#include <ctype.h>

#include "helpers/strs.h"
#include "helpers/history.h"
#include "helpers/memory.h"
#include "helpers/coords.h"

#include "jogo/comandos.h"
#include "jogo/tabuleiro.h"

#include "types.h"

char get_elem(Tab *tab, int x, int y)
{
    if (!assert_pos(tab, x, y))
        return '\0';
    return tab->data[calc_index(tab, x, y)].c;
}

void set_elem(Tab *tab, int x, int y, char c)
{
    if (!assert_pos(tab, x, y))
        return;
    tab->data[calc_index(tab, x, y)].c = c;
}

void toggle_branco(Tab *tab, int x, int y)
{
    if (!assert_pos(tab, x, y))
        return;
    Piece *p = &tab->data[calc_index(tab, x, y)];
    if (islower((unsigned char)p->c))
        p->c = toupper((unsigned char)p->c);
    else if (isupper((unsigned char)p->c))
        p->c = tolower((unsigned char)p->c);
}

void toggle_marked(Tab *tab, int x, int y)
{
    if (!assert_pos(tab, x, y))
        return;
    Piece *p = &tab->data[calc_index(tab, x, y)];
    p->marked = !p->marked;
}

void check_row(Tab *tab, Piece *p, int i, bool *violated)
{
    int r = calc_pos(tab, i).x * tab->width;
    for (int c = 0; c < tab->width; c++)
    {
        int idx = r + c;
        if (idx == i)
            continue;

        Piece check = tab->data[idx];
        if (p->c == check.c && !check.marked && isupper(check.c))
        {
            violated[idx] = true;
            violated[i] = true;
        }
    }
}

void check_column(Tab *tab, Piece *p, int i, bool *violated)
{
    int column = calc_pos(tab, i).y;
    for (int r = 0; r < tab->height; r++)
    {
        int idx = calc_index(tab, r, column);
        if (idx == i)
            continue;

        Piece check = tab->data[idx];
        if (p->c == check.c && !check.marked && isupper(check.c))
        {
            violated[idx] = true;
            violated[i] = true;
        }
    }
}

void check_marked(Tab *tab, int i, bool *violated)
{

    iVec2 pos = calc_pos(tab, i);
    const iVec2 side_deltas[4] = {
        {1, 0},
        {0, 1},
        {-1, 0},
        {0, -1}};

    for (int i = 0; i < 4; i++)
    {
        iVec2 check_pos = add_vec2(pos, side_deltas[i]);

        // check out of bounds
        if (!assert_pos(tab, check_pos.x, check_pos.y))
            continue;

        int ind = calc_index(tab, check_pos.x, check_pos.y);
        if (islower(tab->data[ind].c))
            violated[ind] = true;
    }
}

void floodfill(Tab *tab, iVec2 pos, bool *visited, int *visited_white_pieces)
{
    // Check if this position is valid
    if (!assert_pos(tab, pos.x, pos.y))
        return;

    int curr_idx = calc_index(tab, pos.x, pos.y);

    // If already visited or marked, stop
    if (visited[curr_idx] || tab->data[curr_idx].marked || !isupper(tab->data[curr_idx].c))
        return;

    visited[curr_idx] = true;

    // If it's an uppercase letter, count it
    if (isupper(tab->data[curr_idx].c))
        (*visited_white_pieces)++;

    // Directions: up, right, down, left
    const iVec2 side_deltas[4] = {
        {1, 0},
        {0, 1},
        {-1, 0},
        {0, -1}};

    // Check all four orthogonal directions
    for (int dir = 0; dir < 4; dir++)
    {
        iVec2 new_pos = add_vec2(pos, side_deltas[dir]);
        floodfill(tab, new_pos, visited, visited_white_pieces);
    }
}

void check_paths(Tab *tab, Piece *p, int i, bool *violated)
{
    if (!isupper(p->c) || p->marked)
        return; // Only check paths for uppercase letters that are not marked

    int max_i = tab->height * tab->width;
    bool *visited = (bool *)calloc(max_i, sizeof(bool));
    if (!visited)
        return;

    // Count total uppercase pieces (white pieces)
    int total_white_pieces = 0;
    for (int idx = 0; idx < max_i; idx++)
        if (isupper(tab->data[idx].c) && !tab->data[idx].marked)
            total_white_pieces++;

    // Start flood fill from the current position
    int visited_white_pieces = 0;

    // Start flood fill from the current piece
    iVec2 start_pos = calc_pos(tab, i);
    floodfill(tab, start_pos, visited, &visited_white_pieces);

    // Mark pieces as violated if they are not connected
    if (visited_white_pieces < total_white_pieces)
        for (int idx = 0; idx < max_i; idx++)
            if (isupper(tab->data[idx].c) &&
                !tab->data[idx].marked &&
                !visited[idx])
            {
                violated[idx] = true;
            }

    free(visited);
}

bool validar_tabuleiro(Tab *tab)
{
    int max_i = tab->height * tab->width;

    bool ok = true; // Assume the board is valid until proven otherwise
    bool *violated = (bool *)calloc(max_i, sizeof(bool));
    if (!violated)
        return false;

    // Check rows for duplicate characters
    for (int i = 0; i < max_i; i++)
    {
        Piece *p = &tab->data[i];

        // if it's marked, check for marked rule
        if (p->marked)
        {
            check_marked(tab, i, violated);
            continue;
        }

        // non white pieces do not need to get checked
        if (islower(p->c))
            continue;

        // Compare the current character with the rest of the row
        check_row(tab, p, i, violated);

        // Compare the current character with the rest of the column
        check_column(tab, p, i, violated);

        // check if there is an ortogonal path between white pieces
        check_paths(tab, p, i, violated);
    }

    for (int i = 0; i < max_i; i++)
    {
        tab->data[i].violated = violated[i];
        if (violated[i])
            ok = false;
    }

    free(violated);

    return ok;
}
