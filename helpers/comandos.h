#include "../jogo/tabuleiro.h"

typedef struct
{
    int code;
    char **args;
} Command;

int await_command(Command *c);

int parse_command(char *c);

int run_command(Command *c);

// Carrega um tabuleiro a partir de um ficheiro especificado
// Retorna 1 se o carregamento for bem-sucedido, 0 caso contrário
int carregar_tabuleiro(Tab **tab, const char *filename);

// Guarda o estado atual do tabuleiro num ficheiro
// Retorna 1 se a gravação for bem-sucedida, 0 caso contrário
int salvar_tabuleiro(Tab *tab, const char *filename);

// Verifica se o tabuleiro é válido segundo as regras do jogo
// Confirma que não há repetições de caracteres nas linhas e colunas
// Retorna 1 se o tabuleiro for válido, 0 caso contrário
int validar_tabuleiro(Tab *tab);

// Seleciona uma célula no tabuleiro e executa uma ação
// Se action é 'b': converte o caractere da célula para maiúsculo (bloqueio)
// Se action é 'r': reinicia a célula para '#' (vazia)
// A função primeiro valida se a posição existe no tabuleiro
// Se a posição for válida, executa a ação e retorna 1
int selecionar_casa(Tab *tab, int x, int y, char action) {