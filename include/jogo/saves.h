#ifndef SAVES_H
#define SAVES_H

#include "types.h"

/**
 * Carrega um tabuleiro a partir de um ficheiro especificado
 * @returns 0 se o carregamento for bem-sucedido, 1 caso contrário
 */
int carregar_jogo(Game *game, const char *filename);

/**
 * Guarda o estado atual do tabuleiro num ficheiro
 * @returns 0 se a gravação for bem-sucedida; 1 caso contrário
 */
int salvar_jogo(Game *game, const char *filename);

#endif
