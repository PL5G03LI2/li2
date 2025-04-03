#include <stdio.h>
#include "../helpers/strs.h"
#include "tabuleiro.h"

int calc_index(Tab *tab, int x, int y)
{
    if (assert_pos(tab, x, y))
        return y * tab->height + x;

    return -1;
}

int assert_index(Tab *tab, int i)
{
    int max_i = tab->height * tab->width;
    return (0 <= i && i <= max_i);
}

int assert_pos(Tab *tab, int x, int y)
{
    if (x < 0 || x > tab->height || y < 0 || y > tab->width)
        return 0;
    return 1;
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

    iVec2 positions[4] = {
        {x + 1, y},
        {x - 1, y},
        {x, y + 1},
        {x, y - 1}};

    for (int i = 0; i < 4; i++)
    {
        int nx = positions[i].x;
        int ny = positions[i].y;

        if (assert_pos(tab, nx, ny))
        {
            toggle_branco(tab, nx, ny);
        }
    }
}

void print_tab(Tab *tab, int height, int width)
{
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            printf("%c", tab->data[i * 5 + j]);
        }
        printf("\n");
    }
}
