#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "../jogo/tabuleiro.h"
#include "comandos.h"
#include "strings.h"

int assert_pos(Tabuleiro *tab, int x, int y) {
    return (x < 0 || x >= tab->height || y < 0 || y >= tab->width);
}

char get_pos(Tabuleiro *tab, int x, int y) {
    if (assert_pos(tab, x, y)) return '\0';
    return tab->data[x * tab->width + y];
}

void set_pos(Tabuleiro *tab, int x, int y, char c) {
    if (!assert_pos(tab, x, y)) {
        tab->data[x * tab->width + y] = c;
    }
}

void set_branco(Tabuleiro *tab, int x, int y) {
    if (!assert_pos(tab, x, y)) {
        char current = get_pos(tab, x, y);
        set_pos(tab, x, y, toUpper(current));
    }
}

void print_tabuleiro(Tabuleiro *tab) {
    for (int i = 0; i < tab->height; i++) {
        for (int j = 0; j < tab->width; j++) {
            printf("%c ", get_pos(tab, i, j));
        }
        printf("\n");
    }
}

int carregar_tabuleiro(Tabuleiro **tab, const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) return 0;

    int height, width;
    fscanf(file, "%d %d", &height, &width);
    
    *tab = malloc(sizeof(Tabuleiro));
    (*tab)->height = height;
    (*tab)->width = width;
    (*tab)->data = malloc(height * width * sizeof(char));

    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            fscanf(file, " %c", &(*tab)->data[i * width + j]);
        }
    }
    
    fclose(file);
    return 1;
}

int salvar_tabuleiro(Tabuleiro *tab, const char *filename) {
    FILE *file = fopen(filename, "w");
    if (!file) return 0;

    fprintf(file, "%d %d\n", tab->height, tab->width);
    for (int i = 0; i < tab->height; i++) {
        for (int j = 0; j < tab->width; j++) {
            fprintf(file, "%c", get_pos(tab, i, j));
        }
        fprintf(file, "\n");
    }
    
    fclose(file);
    return 1;
}

int validar_tabuleiro(Tabuleiro *tab) {
    for (int i = 0; i < tab->height; i++) {
        for (int j = 0; j < tab->width; j++) {
            char c = get_pos(tab, i, j);
            
            if (c == '#') continue;
            
            for (int k = 0; k < tab->width; k++) {
                if (k != j && get_pos(tab, i, k) == c) return 0;
            }
            
            for (int k = 0; k < tab->height; k++) {
                if (k != i && get_pos(tab, k, j) == c) return 0;
            }
        }
    }
    return 1;
}
