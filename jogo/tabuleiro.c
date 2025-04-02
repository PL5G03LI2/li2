#include <stdio.h>
#include "strings.h"
#include "tabuleiro.h"

typedef struct tabuleiro
{
    char *data;
    int height;
    int width;
} Tab;

int assert_pos(Tab *tabuleiro, int x, int y)
{
    if (x < 0 || x > tabuleiro->height || y < 0 || y > tabuleiro->width)
        return 1;
    return 0;
}

char get_pos(Tab *tabuleiro, int x, int y)
{
    if (!assert_pos(tabuleiro, x, y))
        return NULL;
    return tabuleiro->data[y * tabuleiro->height + x];
}

void set_pos(Tab *tabuleiro, int x, int y, char c)
{
    tabuleiro->data[y * tabuleiro->height + x] = c;
}

void set_branco(Tab *tabuleiro, int x, int y)
{
    if (assert_pos(tabuleiro, x, y))
        tabuleiro->data[y * tabuleiro->height + x] = toUpper(get_pos(tabuleiro, x, y));
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
