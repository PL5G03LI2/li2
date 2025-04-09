#include <stdio.h>
#include <stdlib.h>
#include "../helpers/strs.h"
#include "tabuleiro.h"

Tab *initialize_tabuleiro(void)
{
    Tab *tabuleiro = (Tab *)calloc(1, sizeof(Tab));

    if (tabuleiro == NULL)
        return NULL;

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

    // deixa comentado para o comando de ajuda
    // iVec2 positions[4] = {
    //     {x + 1, y},
    //     {x - 1, y},
    //     {x, y + 1},
    //     {x, y - 1}};

    // for (int i = 0; i < 4; i++)
    // {
    //     int nx = positions[i].x;
    //     int ny = positions[i].y;

    //     if (assert_pos(tab, nx, ny))
    //     {
    //         toggle_branco(tab, nx, ny);
    //     }
    // }
}

void print_tab(Tab *tab)
{
    int height = tab->height;
    int width = tab->width;

    printf(" ");

    for (int i = 0; i < width; i++)
    {
        printf(" %c", i + 'a');
    }
    printf("\n--");

    for (int i = 0; i < width; i++)
    {
        printf("--");
    }
    printf("\n");

    for (int i = 0; i < height; i++)
    {
        printf("%c|", i + '1');
        for (int j = 0; j < width; j++)
        {
            bool selected = tab->sel_piece.x == j && tab->sel_piece.y == i;
            Piece piece = tab->data[calc_index(tab, j, i)];
            if (selected)
                printf("\033[1m\033[7m");

            if (piece.marked)
                printf("#");
            else
                printf("%c", piece.c);

            if (selected)
                printf("\033[0m");
            printf(" ");
        }
        printf("\n");
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
