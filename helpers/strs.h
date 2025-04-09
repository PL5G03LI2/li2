/*
 Verifica se um character é uma letra maiúscula.

 @returns 1 se for maiúscula.
 @returns 0 se não for.
*/
int isUpper(char c);

/*
 Verifica se um char é uma letra minúscula.

 @returns 1 se for minúscula.
 @returns 0 otherwise.
 */
int isLower(char c);

/*
 Converte um character para maiúsculo.
*/
char toUpper(char c);

/*
 Converte um character para minúsculo.
*/
char toLower(char c);

/**
 * @brief Trims leading and trailing whitespace from a string.
 *
 * @param str The string to be trimmed.
 *            It should be a null-terminated character array.
 *
 * @returns A new pointer to the trimmed string.
 *
 * @note If the input string is NULL or an empty string, the function returns without error.
 */
char *trim_str(char *str);
