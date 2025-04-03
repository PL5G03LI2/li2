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
    return tab->data[calc_index(tab, x, y)];
}

void set_elem(Tab *tab, int x, int y, char c)
{
    if (assert_pos(tab, x, y))
        tab->data[calc_index(tab, x, y)] = c;
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

        tab->data[calc_index(tab, x, y)] = c;
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
