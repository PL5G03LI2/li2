#include "../types/types.h"
#include <stdio.h>

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

/**
 * Tokenizes a command.
 *
 * @param *cmd Command to tokenize.
 * @param **tokens Buffer of tokens to assign.
 *
 * @returns Token count.
 *
 * @note The max tokens per command is 2.
 */
int tokenize_cmd(char *cmd, char **tokens);

/**
 * Frees the command tokens.
 */
void reset_cmd_tokens(ParsedCommand *cmd);

/**
 * Fully resets the provided command.
 * This means:
 * - Freeing the tokens.
 * - Reassigning them to NULL for error checking.
 * - Setting it as CMD_INVALID.
 */
void reset_cmd(ParsedCommand *cmd);

/**
 * Aguarda um comando do utilizador.
 * Lê uma linha de entrada e remove o caractere de nova linha
 *
 * @returns 0 se a leitura for bem-sucedida. 1 caso contrário
 *
 * @param *cmd Buffer para guardar o comando, TEM DE SER 256 BYTES.
 */
int await_command(char *command);

/**
 * Tokenizes the command.
 *
 * @param *command The command to tokenize.
 * @param **args The target tokens array.
 *
 * @returns The token count.
 * @returns 0 if anything fails to tokenize.
 */
int tokenize_cmd(char *command, char **args);

/**
 * Analisa um comando lido do utilizador
 * O comando deve ser uma string terminada em '\0'
 *
 * @param *command Pointer to the string command to parse.
 * @param *parsed_cmd Pointer to the target parsed command.
 *
 * @return 1 if unable to parse.
 * @return 0 otherwise.
 */
int parse_command(Tab *tab, char *command, ParsedCommand *parsed_cmd);

/**
 * Executa um comando analisado.
 *
 * @param *cmd Um comando válido.
 * @param **tab Referência do tabuleiro. Permite ajustes pelo comando.
 *
 * @returns 0 se o comando for executado com sucesso, 1 caso contrário.
 */
int run_command(ParsedCommand *cmd, Tab *tab, TabHistory *history);
