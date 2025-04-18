#include "types.h"

#ifndef TAB_H
#define TAB_H

/**
 * Allocates and initializes tabuleiro.

 * @returns A pointer to the initialized tabuleiro.
 * @returns NULL in case of allocation failure.
*/
Tab *initialize_tabuleiro(void);

/**
 * Calculates the index of the position provided

 * @returns 0 <= index <= height * width;
 * @returns -1 if invalid;
*/
int calc_index(Tab *tabuleiro, int x, int y);

/**
 * Ensures valid string index.

 * @returns true if index is in bounds.
 * @returns false otherwise.
 */
bool assert_index(Tab *tabuleiro, int i);

/**
 * Ensures a valid position.
 * @returns true if position is valid.
 * @returns false otherwise.
 */
bool assert_pos(Tab *tabuleiro, int x, int y);

/**
 * Retorna o char na coordenada (x, y)
 */
char get_elem(Tab *tabuleiro, int x, int y);

/**
 * Set's the given character, c, at the position in the board
 * @note Has no effect if coords are out of bounds.
 */
void set_elem(Tab *tabuleiro, int x, int y, char c);

/**
 * Set a position to white letter (Uppercase)
 */
void toggle_branco(Tab *tabuleiro, int x, int y);

/**
 * Imprime o tabuleiro para o ecrã
 */
void print_tab(Tab *tabuleiro);

/**
 * Toggle marked status of a piece and toggles branco of all ortogonal neighbours
 */
void toggle_marked(Tab *tabuleiro, int x, int y);

/**
 * Carrega um tabuleiro a partir de um ficheiro especificado
 * @returns 0 se o carregamento for bem-sucedido, 1 caso contrário
 */
int carregar_tabuleiro(Tab *tab, const char *filename);

/**
 * Guarda o estado atual do tabuleiro num ficheiro
 * @returns 0 se a gravação for bem-sucedida; 1 caso contrário
 */
int salvar_tabuleiro(Tab *tab, const char *filename);

/**
 * Verifica se o tabuleiro é válido segundo as regras do jogo
 * Confirma que não há repetições de caracteres nas linhas e colunas

 * @returns `true` se o tabuleiro for válido, `false` caso contrário
*/
bool validar_tabuleiro(Tab *tab);
#endif
