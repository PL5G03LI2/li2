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

bool validar_tabuleiro(Tab *tab)
{
    int h = tab->height, w = tab->width;

    // Reset all 'violated' flags before validation
    for (int i = 0; i < h * w; i++)
    {
        tab->data[i].violated = false;
    }

    bool ok = true; // Assume the board is valid until proven otherwise

    // Check rows for duplicate characters
    for (int x = 0; x < h; x++)
    {
        for (int y = 0; y < w; y++)
        {
            char c = tab->data[x * w + y].c;

            if (c == '#')
                continue; // Ignore unused or empty cells

            // Compare the current character with the rest of the row
            for (int y2 = y + 1; y2 < w; y2++)
            {
                if (tab->data[x * w + y2].c == c)
                {
                    // Mark both positions as violated
                    tab->data[x * w + y].violated = true;
                    tab->data[x * w + y2].violated = true;
                    ok = false; // The board is invalid
                }
            }
        }
    }

    // Check columns for duplicate characters
    for (int y = 0; y < w; y++)
    {
        for (int x = 0; x < h; x++)
        {
            char c = tab->data[x * w + y].c;

            if (c == '#')
                continue; // Ignore empty cells

            // Compare the current character with the rest of the column
            for (int x2 = x + 1; x2 < h; x2++)
            {
                if (tab->data[x2 * w + y].c == c)
                {
                    // Mark both positions as violated
                    tab->data[x * w + y].violated = true;
                    tab->data[x2 * w + y].violated = true;
                    ok = false; // The board is invalid
                }
            }
        }
    }

    return ok; // Return true if no violations were found, false otherwise
}

static void init_colors(void)
{
    start_color();
    use_default_colors();
    init_pair(1, COLOR_BLACK, COLOR_WHITE); // selected normal
    init_pair(2, COLOR_RED, COLOR_WHITE);   // selected violated
    init_pair(3, COLOR_WHITE, COLOR_RED);   // violated not selected
    init_pair(4, COLOR_WHITE, -1);          // default
}

void print_tab(Tab *tab, iVec2 win_d)
{
    static bool colors_inited = false;
    if (!colors_inited)
    {
        init_colors();
        colors_inited = true;
    }
    // board dimensions calculation
    int h = tab->height, w = tab->width;
    int board_h = 3 + h;                   // Height: rows + header + separator
    int board_w = 3 + 2 * w;               // Width: 2 chars per cell + padding
    int start_y = (win_d.y - board_h) / 2; // Vertical centering
    int start_x = (win_d.x - board_w) / 2; // Horizontal centering
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

    // Reset file position to start of "violated" line
    fseek(f, -strlen(buffer), SEEK_CUR);

    // Read "violated" marker
    if (fscanf(f, "%s", buffer) != 1)
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

    // Reset file position to start of "history" line
    fseek(f, -strlen(buffer), SEEK_CUR);

    // Read "history" marker
    if (fscanf(f, "%s", buffer) != 1)
    {
        fclose(f);
        return 1;
    }

    // Read command history
    char token1[32], token2[32];
    while (fscanf(f, "%s %s", token1, token2) == 2)
    {
        snprintf(game->cmd_str, 256, "%s %s", token1, token2);
        if (parse_command(game))
        {
            fclose(f);
            return 1;
        }

        game->history = push_history(game->history, game->cmd);
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