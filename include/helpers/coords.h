#ifndef COORDS_H
#define COORDS_H

#include "types.h"
/**
 * @brief Adds two vectors together
 *
 * @returns The resulting vector.
 */
#define add_vec2(a, b) ((iVec2){(a).x + (b).x, (a).y + (b).y});

/**
 * Calculates the index of the position provided
 * @returns 0 <= index <= height * width;
 * @returns -1 if invalid;
 */
#define calc_index(tabuleiro, x, y)                                            \
  ((assert_pos(tabuleiro, x, y)) ? (x * (tabuleiro)->width + y) : -1)

/**
 * Calculates a position, given an index
 *
 * @returns
 *  The calculated position, if valid;
 *  (-1, -1) otherwise;
 */
#define calc_pos(tabuleiro, i)                                                 \
  ((assert_index(tabuleiro, i))                                                \
       ? (iVec2){(i) / (tabuleiro)->width, (i) % (tabuleiro)->width}           \
       : (iVec2){-1, -1})

/**
 * Ensures valid string index.
 * @returns true if index is in bounds.
 * @returns false otherwise.
 */
#define assert_index(tabuleiro, i)                                             \
  ((i) >= 0 && (i) < (tabuleiro)->height * (tabuleiro)->width)

/**
 * Ensures a valid position.
 * @returns true if position is valid.
 * @returns false otherwise.
 */
#define assert_pos(tabuleiro, x, y)                                            \
  ((x) >= 0 && (x) < (tabuleiro)->height && (y) >= 0 &&                        \
   (y) < (tabuleiro)->width)

/**
 * Reads a token and converts it to a Vector.
 * This function will add up the x components (a-z) and then convert y
 * components.
 *
 * Ex: `read_coordinate("aa10") -> x: 27 + 1, y: 10.`
 *
 * @param *coordinate_token The token to convert.
 *
 * @returns iVec2 com as coordenadas convertidas.
 */
iVec2 read_coordinate(char *coordinate_token);

/**
 * Converts an iVec2 to a string, stores it in the provided buffer.
 *
 * @param coord The vector to convert.
 * @param *buffer The buffer to store results.
 *
 * @returns A pointer to the provided buffer, in case you lost it.
 */
char *write_coordinate(iVec2 coord, char *buffer);

#endif
