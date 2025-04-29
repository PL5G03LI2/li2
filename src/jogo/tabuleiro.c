#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ncurses.h>
#include <ctype.h>

#include "helpers/strs.h"
#include "helpers/history.h"
#include "helpers/memory.h"
#include "jogo/comandos.h"
#include "jogo/tabuleiro.h"

#include "types.h"

Tab *initialize_tabuleiro(void)
{
    Tab *tab = malloc(sizeof(Tab));
    if (!tab)
        return NULL;
    tab->height = 0;
    tab->width = 0;
    tab->sel_piece.x = 0;
    tab->sel_piece.y = 0;
    tab->data = NULL;
    return tab;
}

iVec2 add_vec2(iVec2 a, iVec2 b)
{
    return (iVec2){a.x + b.x, a.y + b.y};
}

bool assert_pos(Tab *tab, int x, int y)
{
    return (x >= 0 && x < tab->height && y >= 0 && y < tab->width);
}

int calc_index(Tab *tab, int x, int y)
{
    if (!assert_pos(tab, x, y))
        return -1;
    return x * tab->width + y;
}

iVec2 calc_pos(Tab *tab, int i)
{
    iVec2 pos = {-1, -1};
    if (!assert_index(tab, i))
        return pos;
    pos.x = i / tab->width;
    pos.y = i % tab->width;
    return pos;
}

bool assert_index(Tab *tab, int i)
{
    return (i >= 0 && i < tab->height * tab->width);
}

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
    if (visited[curr_idx] || tab->data[curr_idx].marked)
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

void print_tab(Tab *tab, iVec2 win_d)
{
    int h = tab->height, w = tab->width;
    int board_h = 3 + h;                   // Height: rows + header + separator
    int board_w = 3 + 2 * w;               // Width: 2 chars per cell + padding
    int start_y = (win_d.y - board_h) / 2; // Vertical centering
    int start_x = (win_d.x - board_w) / 2; // Horizontal centering

    if (win_d.x < board_w || win_d.y - 2 < board_h)
    {
        print_info("Window too small.", win_d);
        return;
    }

    // Column headers
    attron(COLOR_PAIR(4));
    mvprintw(start_y, start_x + 1, " ");
    for (int y = 0; y < w; y++)
        printw("%d ", y + 1);
    attroff(COLOR_PAIR(4));

    // Separator (uses default color)
    mvprintw(start_y + 1, start_x, "--");
    for (int y = 0; y < w; y++)
        printw("--");

    // Rows
    for (int x = 0; x < h; x++)
    {
        // Row Header (e.g., "a|")
        attron(COLOR_PAIR(4));
        mvprintw(start_y + 2 + x, start_x, "%c|", 'a' + x); // "a|", "b|", etc.
        attroff(COLOR_PAIR(4));

        // Cells in the Row
        for (int y = 0; y < w; y++)
        {
            int idx = x * w + y;
            Piece p = tab->data[idx];
            bool sel = (x == tab->sel_piece.x && y == tab->sel_piece.y);

            // Color Logic
            int cp = 4; // Default: white text, default background
            if (p.violated)
                cp = sel ? 2 : 3; // Violated cells
            else if (sel)
                cp = 1; // Selected cell

            // Print Cell
            attron(COLOR_PAIR(cp));
            mvprintw(start_y + 2 + x, start_x + 2 + 2 * y, "%c", p.marked ? '#' : p.c);
            attroff(COLOR_PAIR(cp));
        }
    }
    refresh(); // Flushes all changes to the terminal screen.
}

int carregar_jogo(Game *game, const char *filename)
{
    FILE *f = fopen(filename, "r");
    if (!f)
        return 1;

    // Read dimensions
    int h, w;
    if (fscanf(f, "%d %d", &h, &w) != 2)
    {
        fclose(f);
        return 1;
    }

    free_game(game);
    init_game(game);

    Tab *tab = game->tabuleiro;

    // Initialize board
    tab->height = h;
    tab->width = w;
    free(tab->data);
    tab->data = calloc(h * w, sizeof(Piece));
    if (!tab->data)
    {
        fclose(f);
        return 1;
    }

    // Read board state
    for (int i = 0; i < h * w; i++)
    {
        Piece *p = &tab->data[i];
        if (fscanf(f, " %c", &p->c) != 1)
        {
            fclose(f);
            return 1;
        }
        p->marked = false;
        p->violated = false;
    }

    // Read "selected" marker
    char buffer[32];
    if (fscanf(f, "%s", buffer) != 1)
    {
        fclose(f);
        return 1;
    }

    // Read selected piece coords
    if (fscanf(f, "%d %d", &tab->sel_piece.x, &tab->sel_piece.y) != 2)
    {
        fclose(f);
        return 1;
    }

    // Read "marked" marker
    if (fscanf(f, "%s", buffer) != 1)
    {
        fclose(f);
        return 1;
    }

    // Read marked pieces until "violated" marker
    int x, y;
    while (fscanf(f, "%d %d", &x, &y) == 2)
    {
        int idx = calc_index(tab, x, y);
        if (idx >= 0)
            tab->data[idx].marked = true;
    }

    if (fscanf(f, "%s", buffer) != 1 || strcmp(buffer, "violated") != 0)
    {
        fclose(f);
        return 1;
    }

    // Read violated pieces until "history" marker
    while (fscanf(f, "%d %d", &x, &y) == 2)
    {
        int idx = calc_index(tab, x, y);
        if (idx >= 0)
            tab->data[idx].violated = true;
    }

    // Read "history" marker
    if (fscanf(f, "%s", buffer) != 1 || strcmp(buffer, "history") != 0)
    {
        fclose(f);
        return 1;
    }

    // Read command history - skip empty lines and verify format
    char token1[32], token2[32];
    char line[256];
    while (fgets(line, sizeof(line), f))
    {
        // Skip empty lines
        if (line[0] == '\n' || line[0] == '\0')
            continue;

        if (sscanf(line, "%s %s", token1, token2) == 2)
        {
            snprintf(game->cmd_str, 256, "%s %s", token1, token2);
            if (parse_command(game))
            {
                fclose(f);
                return 1;
            }
            game->history = push_history(game->history, game->cmd);
        }
    }

    fclose(f);
    return 0;
}

int salvar_jogo(Game *game, const char *filename)
{
    FILE *f = fopen(filename, "w");
    if (!f)
        return 1;

    Tab *tab = game->tabuleiro;

    fprintf(f, "%d %d\n", tab->height, tab->width);

    int total_size = tab->height * tab->width;

    for (int i = 0; i < total_size; i++)
    {
        Piece p = tab->data[i];
        fprintf(f, "%c", p.c);
        if ((i + 1) % tab->width == 0)
        {
            fprintf(f, "\n");
        }
    }
    fprintf(f, "\n");

    fprintf(f, "selected\n");
    fprintf(f, "%d %d\n", tab->sel_piece.x, tab->sel_piece.y);

    fprintf(f, "marked\n");
    for (int i = 0; i < total_size; i++)
    {
        Piece p = tab->data[i];

        iVec2 pos = calc_pos(tab, i);
        if (p.marked)
            fprintf(f, "%d %d\n", pos.x, pos.y);
    }

    fprintf(f, "violated\n");
    for (int i = 0; i < total_size; i++)
    {
        Piece p = tab->data[i];

        iVec2 pos = calc_pos(tab, i);
        if (p.violated)
            fprintf(f, "%d %d\n", pos.x, pos.y);
    }

    fprintf(f, "history\n");
    int i = 0;
    ParsedCommand *current = get_history_element(game->history, i++);
    while (current != NULL)
    {
        for (int i = 0; i < 2; i++)
            fprintf(f, "%s ", current->tokens[i]);
        fprintf(f, "\n");
        current = get_history_element(game->history, i++);
    }

    fclose(f);
    return 0;
}