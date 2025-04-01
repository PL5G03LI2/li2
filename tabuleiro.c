#include <stdio.h>

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