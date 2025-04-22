#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ncurses.h>
#include <ctype.h>
#include "types.h"
#include "helpers/strs.h"

// Forward declarations
bool assert_pos(Tab *tab, int x, int y);

/** Allocates and initializes a Tab structure */
Tab *initialize_tabuleiro(void) {
    Tab *tab = malloc(sizeof(Tab));
    if (!tab) return NULL;
    tab->height = 0;
    tab->width = 0;
    tab->sel_piece.x = 0;
    tab->sel_piece.y = 0;
    tab->data = NULL;
    return tab;
}

/** Checks if (x,y) is within board bounds */
bool assert_pos(Tab *tab, int x, int y) {
    return (x >= 0 && x < tab->height && y >= 0 && y < tab->width);
}

/** Calculates linear index for (x=row, y=col) */
int calc_index(Tab *tab, int x, int y) {
    if (!assert_pos(tab, x, y)) return -1;
    return x * tab->width + y;
}

/** Checks if a linear index is valid */
bool assert_index(Tab *tab, int i) {
    return (i >= 0 && i < tab->height * tab->width);
}

/** Returns character at (x,y), or ' ' if invalid */
char get_elem(Tab *tab, int x, int y) {
    if (!assert_pos(tab, x, y)) return ' ';
    return tab->data[calc_index(tab, x, y)].c;
}

/** Sets character at (x,y) if valid */
void set_elem(Tab *tab, int x, int y, char c) {
    if (!assert_pos(tab, x, y)) return;
    tab->data[calc_index(tab, x, y)].c = c;
}

/** Toggles a cell between lowercase and uppercase (white) */
void toggle_branco(Tab *tab, int x, int y) {
    if (!assert_pos(tab, x, y)) return;
    Piece *p = &tab->data[calc_index(tab, x, y)];
    if (islower((unsigned char)p->c)) p->c = toupper((unsigned char)p->c);
    else if (isupper((unsigned char)p->c)) p->c = tolower((unsigned char)p->c);
}

/** Toggles marked status and forces orthogonal neighbors white */
void toggle_marked(Tab *tab, int x, int y) {
    if (!assert_pos(tab, x, y)) return;
    Piece *p = &tab->data[calc_index(tab, x, y)];
    p->marked = !p->marked;
    // For each orthogonal neighbor:
    const int dx[4] = {1, -1, 0, 0};
    const int dy[4] = {0, 0, 1, -1};
    for (int i = 0; i < 4; i++) {
        int nx = x + dx[i], ny = y + dy[i];
        if (assert_pos(tab, nx, ny)) {
            toggle_branco(tab, nx, ny);
        }
    }
}


// Checks whether the board is valid according to game rules.

bool validar_tabuleiro(Tab *tab) {
    int h = tab->height, w = tab->width;

    // Reset all 'violated' flags before validation
    for (int i = 0; i < h * w; i++) {
        tab->data[i].violated = false;
    }

    bool ok = true; // Assume the board is valid until proven otherwise

    // Check rows for duplicate characters
    for (int x = 0; x < h; x++) {
        for (int y = 0; y < w; y++) {
            char c = tab->data[x * w + y].c;

            if (c == '#') continue; // Ignore unused or empty cells

            // Compare the current character with the rest of the row
            for (int y2 = y + 1; y2 < w; y2++) {
                if (tab->data[x * w + y2].c == c) {
                    // Mark both positions as violated
                    tab->data[x * w + y].violated = true;
                    tab->data[x * w + y2].violated = true;
                    ok = false; // The board is invalid
                }
            }
        }
    }

    // Check columns for duplicate characters
    for (int y = 0; y < w; y++) {
        for (int x = 0; x < h; x++) {
            char c = tab->data[x * w + y].c;

            if (c == '#') continue; // Ignore empty cells

            // Compare the current character with the rest of the column
            for (int x2 = x + 1; x2 < h; x2++) {
                if (tab->data[x2 * w + y].c == c) {
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


/** Initialize ncurses color pairs once */
static void init_colors(void) {
    start_color();
    use_default_colors();
    init_pair(1, -1, COLOR_WHITE);   // selected normal
    init_pair(2, COLOR_WHITE, COLOR_RED );  // selected violated
    init_pair(3, COLOR_RED, COLOR_WHITE );    // violated not selected
    init_pair(4, COLOR_WHITE, -1 );           // default
}

/** Prints the board centered in window dimensions win_d */
void print_tab(Tab *tab, iVec2 win_d) {
    static bool colors_inited = false;
    if (!colors_inited) {
        init_colors();
        colors_inited = true;
    }
    //board dimensions calculation
    int h = tab->height, w = tab->width;
    int board_h = 3 + h;          // Height: rows + header + separator
    int board_w = 3 + 2*w;        // Width: 2 chars per cell + padding
    int start_y = (win_d.y - board_h) / 2;  // Vertical centering
    int start_x = (win_d.x - board_w) / 2;  // Horizontal centering
    // Column headers
    attron(COLOR_PAIR(4));
    mvprintw(start_y, start_x+1," ");
    for (int y = 0; y < w; y++) printw("%d ", y+1);
    attroff(COLOR_PAIR(4));

    // Separator (uses default color)
    mvprintw(start_y+1, start_x, "--");
    for (int y = 0; y < w; y++) printw("--");

    // Rows
    for (int x = 0; x < h; x++) {
        // Row Header (e.g., "a|")
        attron(COLOR_PAIR(4));
        mvprintw(start_y+2+x, start_x, "%c|", 'a'+x); // "a|", "b|", etc.
        attroff(COLOR_PAIR(4));
    
        // Cells in the Row
        for (int y = 0; y < w; y++) {
            int idx = x*w + y;
            Piece p = tab->data[idx];
            bool sel = (x == tab->sel_piece.x && y == tab->sel_piece.y);
    
            // Color Logic
            int cp = 4;  // Default: white text, default background
            if (p.violated) cp = sel ? 2 : 3;  // Violated cells
            else if (sel) cp = 1;               // Selected cell
    
            // Print Cell
            attron(COLOR_PAIR(cp));
            mvprintw(start_y+2+x, start_x+2+2*y, "%c", p.marked ? '#' : p.c);
            attroff(COLOR_PAIR(cp));
        }
    }
    refresh(); //Flushes all changes to the terminal screen.
}

/** Loads board from file */
int carregar_tabuleiro(Tab *tab, const char *filename) {
    FILE *f = fopen(filename, "r");
    if (!f) return 1;
    int h, w;
    if (fscanf(f, "%d %d", &h, &w) != 2) { fclose(f); return 1; }
    tab->height = h;
    tab->width  = w;
    free(tab->data);
    tab->data = calloc(h*w, sizeof(Piece));
    for (int x = 0; x < h; x++) {
        for (int y = 0; y < w; y++) {
            Piece *p = &tab->data[x*w + y];
            fscanf(f, " %c", &p->c);
            p->marked = false;
            p->violated = false;
        }
    }
    fclose(f);
    return 0;
}

/** Saves board to file */
int salvar_tabuleiro(Tab *tab, const char *filename) {
    FILE *f = fopen(filename, "w");
    if (!f) return 1;
    fprintf(f, "%d %d\n", tab->height, tab->width);
    for (int x = 0; x < tab->height; x++) {
        for (int y = 0; y < tab->width; y++) {
            fprintf(f, "%c", tab->data[x*tab->width + y].c);
        }
        fprintf(f, "\n");
    }
    fclose(f);
    return 0;
}

/* End of tabuleiro.c */