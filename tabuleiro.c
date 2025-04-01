#include <stdio.h>

char get_pos(char *tabuleiro, int x, int y)
{
    return tabuleiro[y * 5 + x];
}

void print_tabuleiro(char *tabuleiro, int height, int width)
{
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            print(tabuleiro[i * 5 + j]);
        }
    }
}