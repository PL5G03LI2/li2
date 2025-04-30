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

#endif