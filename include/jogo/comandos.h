#include <stdio.h>
#include "types.h"

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
 * @param cmd_win Pointer para a WIN de comandos
 * @param command O buffer para guardar o comando
 *
 * @returns 0 se a leitura for bem-sucedida. 1 caso contrário
 */
int await_command(WIN cmd_win, char *command);

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
