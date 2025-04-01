#include <stdio.h>
#include "strings.h"

char get_pos(char *tabuleiro, int x, int y, int height, int width)
{
    return tabuleiro[y * 5 + x];
}

void set_pos(char *tabuleiro, int x, int y, char c, int height, int width)
{
    tabuleiro[y * 5 + x] = c;
}

void set_branco(char *tabuleiro, int x, int y, int height, int width)
{
    if (y <= height && x <= width)
        tabuleiro[y * height + x] = toUpper(get_pos(tabuleiro, x, y, height, width));
}

void print_tabuleiro(char *tabuleiro, int height, int width)
{
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            printf("%c", tabuleiro[i * 5 + j]);
        }
        printf("\n");
    }
}