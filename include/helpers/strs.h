#include "types.h"
#ifndef STRS_H
#define STRS_H

/**
 * @brief Trims leading and trailing whitespace from a string.
 *
 * @param str The string to be trimmed.
 *            It should be a null-terminated character array.
 *
 * @returns A new pointer to the trimmed string.
 *
 * @note If the input string is NULL or an empty string, the function returns
 * without error.
 */
char *trim_str(char *str);

#endif
