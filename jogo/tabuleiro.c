#include <stdio.h>
#include "strings.h"
#include "tabuleiro.h"

int calc_index(Tab *tabuleiro, int x, int y)
{
    if (assert_pos(tabuleiro, x, y))
        return y * tabuleiro->height + x;

    return -1;
}

int assert_index(Tab *tabuleiro, int i)
{
    int max_i = tabuleiro->height * tabuleiro->width;
    return (0 <= i && i <= max_i);
}

int assert_pos(Tab *tabuleiro, int x, int y)
{
    if (x < 0 || x > tabuleiro->height || y < 0 || y > tabuleiro->width)
        return 0;
    return 1;
}

char get_elem(Tab *tabuleiro, int x, int y)
{
    if (!assert_pos(tabuleiro, x, y))
        return '\0';
    return tabuleiro->data[calc_index(tabuleiro, x, y)];
}

void set_elem(Tab *tabuleiro, int x, int y, char c)
{
    if (assert_pos(tabuleiro, x, y))
        tabuleiro->data[calc_index(tabuleiro, x, y)] = c;
}

void toggle_branco(Tab *tabuleiro, int x, int y)
{
    if (assert_pos(tabuleiro, x, y))
        tabuleiro->data[calc_index(tabuleiro, x, y)] = toUpper(get_elem(tabuleiro, x, y));
}

void print_tabuleiro(Tab *tabuleiro, int height, int width)
{
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            printf("%c", tabuleiro->data[i * 5 + j]);
        }
        printf("\n");
    }
}
