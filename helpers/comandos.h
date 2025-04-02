#ifndef TABULEIRO_H
#define TABULEIRO_H
#include "../jogo/tabuleiro.h"

// Definição da estrutura Tabuleiro que representa um jogo
// Esta estrutura contém os dados essenciais para manipular o tabuleiro
typedef struct {
    char *data;     // Array unidimensional que armazena as células do tabuleiro em formato linha-coluna
    int height;     // Número de linhas do tabuleiro
    int width;      // Número de colunas do tabuleiro
} Tabuleiro;

// Verifica se uma posição está fora dos limites do tabuleiro
// Retorna 1 (verdadeiro) se a posição for inválida, 0 se for válida
int assert_pos(Tabuleiro *tab, int x, int y);

// Obtém o carácter na posição (x,y) do tabuleiro
// Retorna o carácter na posição especificada ou '\0' se a posição for inválida
char get_pos(Tabuleiro *tab, int x, int y);

// Define um carácter na posição (x,y) do tabuleiro
// Não faz nada se a posição for inválida
void set_pos(Tabuleiro *tab, int x, int y, char c);

// Converte o carácter na posição (x,y) para maiúscula
// Representa a transformação de uma célula para branca no jogo
void set_branco(Tabuleiro *tab, int x, int y);

// Imprime o tabuleiro para o stdout, formatado com espaços entre caracteres
// Apresenta a visualização completa do estado atual do jogo
void print_tabuleiro(Tabuleiro *tab);

// Carrega um tabuleiro a partir de um ficheiro especificado
// Retorna 1 se o carregamento for bem-sucedido, 0 caso contrário
int carregar_tabuleiro(Tabuleiro **tab, const char *filename);

// Guarda o estado atual do tabuleiro num ficheiro
// Retorna 1 se a gravação for bem-sucedida, 0 caso contrário
int salvar_tabuleiro(Tabuleiro *tab, const char *filename);

// Verifica se o tabuleiro é válido segundo as regras do jogo
// Confirma que não há repetições de caracteres nas linhas e colunas
// Retorna 1 se o tabuleiro for válido, 0 caso contrário
int validar_tabuleiro(Tabuleiro *tab);

#endif