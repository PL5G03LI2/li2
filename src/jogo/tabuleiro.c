#include <stdio.h>
#include <stdlib.h>
#include <ncurses.h>

#include "helpers/strs.h"
#include "jogo/tabuleiro.h"

Tab *initialize_tabuleiro(void)
{
    Tab *tabuleiro = (Tab *)malloc(sizeof(Tab));

    tabuleiro->height = 0;
    tabuleiro->width = 0;
    tabuleiro->sel_piece.x = 0;
    tabuleiro->sel_piece.y = 0;
    tabuleiro->data = NULL;

    return tabuleiro;
}

int calc_index(Tab *tab, int x, int y)
{
    if (assert_pos(tab, x, y))
        return x * tab->width + y;

    return -1;
}

bool assert_index(Tab *tab, int i)
{
    int max_i = tab->height * tab->width;
    return (0 <= i && i <= max_i);
}

bool assert_pos(Tab *tab, int x, int y)
{
    return !(x < 0 || x >= tab->height || y < 0 || y >= tab->width);
}

char get_elem(Tab *tab, int x, int y)
{
    if (!assert_pos(tab, x, y))
        return '\0';
    return tab->data[calc_index(tab, x, y)].c;
}

void set_elem(Tab *tab, int x, int y, char c)
{
    if (assert_pos(tab, x, y))
        tab->data[calc_index(tab, x, y)].c = c;
}

void toggle_branco(Tab *tab, int x, int y)
{
    if (assert_pos(tab, x, y))
    {
        char c = get_elem(tab, x, y);

        if (isUpper(c))
            c = toLower(c);
        else
            c = toUpper(c);

        tab->data[calc_index(tab, x, y)].c = c;
    }
}

void toggle_marked(Tab *tab, int x, int y)
{
    if (!assert_pos(tab, x, y))
        return;

    Piece *piece = &tab->data[calc_index(tab, x, y)];
    piece->marked = !piece->marked;
}

void print_piece(Piece piece)
{
    if (piece.marked)
        printw("#");
    else
        printw("%c", piece.c);
}

void print_tab(Tab *tab)
{
    int height = tab->height;
    int width = tab->width;

    // Print column headers
    printw(" ");
    for (int i = 0; i < width; i++)
    {
        printw(" %d", i + 1);
    }
    printw("\n--");
    for (int i = 0; i < width; i++)
    {
        printw("--");
    }
    printw("\n");

    // Print each row of the grid
    for (int x = 0; x < height; x++)
    {
        printw("%c|", x + 'a');

        // Print each character in the row
        for (int y = 0; y < width; y++)
        {
            int index = calc_index(tab, x, y);
            Piece piece = tab->data[index];
            bool selected = x == tab->sel_piece.x && y == tab->sel_piece.y;

            if (selected)
            {
                attron(COLOR_PAIR(1));
                print_piece(piece);
                attroff(COLOR_PAIR(1));
            }
            else
            {
                print_piece(piece);
            }
            printw(" "); // Add a space after each character
        }

        printw("\n"); // Move to the next line after each row
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
    if (tab->data != NULL)
        free(tab->data);
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
