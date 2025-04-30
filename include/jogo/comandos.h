#include <stdio.h>
#include "types.h"
#define CMD_MAX_LENGTH 256

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
 * Copies a command to new memory.
 *
 * @param cmd The command to copy.
 *
 * @returns A pointer to the copied command.
 *
 * @note YOU MUST FREE THE GIVEN POINTER BY YOURSELF AFTER YOU'RE DONE WITH IT.
 */
ParsedCommand *deep_copy_cmd(ParsedCommand *cmd);

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
int await_command(char *buffer);

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
 * Analisa o comando lido do jogador na string `game.cmd_str`.
 *
 * @param game Pointer to the game.
 *
 * @return 1 if unable to parse.
 * @return 0 otherwise.
 *
 * @note O comando nessa string tem de ser NULL terminated.
 */
int parse_command(Game *game);

/**
 * Executa um comando analisado.
 *
 * @param game O pointer para o jogo.
 *
 * @returns 0 se o comando for executado com sucesso, 1 caso contrário.
 */
int run_command(Game *game);

int undo_command(ParsedCommand *cmd, Tab *tab);
