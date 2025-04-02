#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "../jogo/tabuleiro.h"
#include "comandos.h"
#include "strings.h"

int carregar_tabuleiro(Tabuleiro **tab, const char *filename)
{
    FILE *file = fopen(filename, "r");
    if (!file)
        return 0;

    int height, width;
    fscanf(file, "%d %d", &height, &width);

    *tab = malloc(sizeof(Tabuleiro));
    (*tab)->height = height;
    (*tab)->width = width;
    (*tab)->data = malloc(height * width * sizeof(char));

    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            fscanf(file, " %c", &(*tab)->data[i * width + j]);
        }
    }

    fclose(file);
    return 1;
}

int salvar_tabuleiro(Tabuleiro *tab, const char *filename)
{
    FILE *file = fopen(filename, "w");
    if (!file)
        return 0;

    fprintf(file, "%d %d\n", tab->height, tab->width);
    for (int i = 0; i < tab->height; i++)
    {
        for (int j = 0; j < tab->width; j++)
        {
            fprintf(file, "%c", get_pos(tab, i, j));
        }
        fprintf(file, "\n");
    }

    fclose(file);
    return 1;
}

int validar_tabuleiro(Tabuleiro *tab)
{
    for (int i = 0; i < tab->height; i++)
    {
        for (int j = 0; j < tab->width; j++)
        {
            char c = get_pos(tab, i, j);

            if (c == '#')
                continue;

            for (int k = 0; k < tab->width; k++)
            {
                if (k != j && get_pos(tab, i, k) == c)
                    return 0;
            }

            for (int k = 0; k < tab->height; k++)
            {
                if (k != i && get_pos(tab, k, j) == c)
                    return 0;
            }
        }
    }
    return 1;
}
