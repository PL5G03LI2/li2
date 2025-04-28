#ifndef TABULEIRO_H
#define TABULEIRO_H

#include "types.h"

/**
 * Allocates and initializes tabuleiro.
 * @returns A pointer to the initialized tabuleiro.
 * @returns NULL in case of allocation failure.
 */
Tab *initialize_tabuleiro(void);

/**
 * @brief Adds two vectors together
 *
 * @returns The resulting vector.
 */
iVec2 add_vec2(iVec2 a, iVec2 b);

/**
 * Calculates the index of the position provided
 * @returns 0 <= index <= height * width;
 * @returns -1 if invalid;
 */
int calc_index(Tab *tabuleiro, int x, int y);

/**
 * Calculates a position, given an index
 *
 * @returns
 *  The calculated position, if valid;
 *  (-1, -1) otherwise;
 */
iVec2 calc_pos(Tab *tabuleiro, int i);

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

// /**
//  * Set a position to white letter (Uppercase)
//  */
void toggle_branco(Tab *tabuleiro, int x, int y);

/**
 * Imprime o tabuleiro para o ecrã
 */
void print_tab(Tab *tabuleiro, iVec2 win_d);

/**
 * Toggle marked status of a piece and toggles branco of all orthogonal neighbours
 */
void toggle_marked(Tab *tabuleiro, int x, int y);

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

/**
 * @brief Checks white piece rule in the row.
 *
 * The white piece rule is defined as:
 *
 * "In every row and column there can only be a
 * single replica of the same symbol in white"
 *
 * @param tabuleiro The pointer to the board
 * @param p A pointer to the current piece
 * @param index The index to start check
 * @param violated_array A boolean array that mirrors the board's violated states
 */
void check_row(Tab *tabuleiro, Piece *p, int index, bool *violated_array);

/**
 * @brief Checks white piece rule in the column.
 *
 * The white piece rule is defined as:
 *
 * "In every row and column there can only be a
 * single replica of the same symbol in white"
 *
 * @param tabuleiro The pointer to the board
 * @param p A pointer to the current piece
 * @param index The index to start check
 * @param violated_array A boolean array that mirrors the board's violated states
 */
void check_column(Tab *tabuleiro, Piece *p, int index, bool *violated_array);

/**
 * @brief Checks marked piece rule.
 *
 * The marked piece rule is defined as:
 *
 * "If a piece is marked, all the adjacent pieces must be white"
 *
 * @param tabuleiro The pointer to the board
 * @param index The index to check
 * @param violated_array A boolean array that mirrors the board's violated states
 */
void check_marked(Tab *tabuleiro, int index, bool *violated);

/**
 * @brief Recursive approach to floodfill
 *
 * This floodfill algorithm will go through every possible path at the same time
 * and counts how many white pieces it has seen. If, by the end, it didn't count as
 * many white pieces as there are in the board, it is up to the caller to detect
 * that and mark the not visited white pieces as violated.
 *
 * @param tabuleiro The pointer to the board
 * @param pos The current position
 * @param visited A boolean array of the visited pieces
 * @param visited_white_pieces The amount of white pieces discovered
 */
void floodfill(Tab *tabuleiro, iVec2 pos, bool *visited, int *visited_white_pieces);

/**
 * @brief Checks white piece connection rule.
 *
 * The white piece connection rule is defined as:
 *
 * "There must be an ortogonal path between white pieces"
 *
 * @param tabuleiro The pointer to the board
 * @param p A pointer to the current piece
 * @param index The index to check
 * @param violated_array A boolean array that mirrors the board's violated states
 */
void check_paths(Tab *tabuleiro, Piece *p, int index, bool *violated_array);

/**
 * Verifica se o tabuleiro é válido segundo as regras do jogo
 * Confirma que não há repetições de caracteres nas linhas e colunas
 * @returns `true` se o tabuleiro for válido, `false` caso contrário
 */
bool validar_tabuleiro(Tab *tab);

#endif