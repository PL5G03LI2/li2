#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "jogo/tabuleiro.h"
#include "helpers/comandos.h"


    
int main()
{
    char *tabuleiro = "abcdeabcdefhijklmnophsjcw";
    print_tabuleiro(tabuleiro, 5, 5);
    return 0;
}