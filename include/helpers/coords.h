#ifndef COORDS_H
#define COORDS_H

#include "types.h"
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
 * Reads a token and converts it to a Vector.
 * This function will add up the x components (a-z) and then convert y components.
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
