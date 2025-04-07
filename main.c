#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "./jogo/tabuleiro.h"
#include "./helpers/comandos.h"

int main()
{
    char **args = calloc(3, sizeof(char *));
    for (int i = 0; i < 3; i++)
        args[i] = calloc(32, sizeof(char));

    printf("%d", tokenize_cmd("g b 4", args));
    return 0;
}