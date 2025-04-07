#include "../types/types.h"

// Aguarda um comando do utilizador
// Lê uma linha de entrada e remove o caractere de nova linha
// Retorna 1 se a leitura for bem-sucedida, 0 caso contrário
// O comando lido é armazenado na variável 'command'
// O tamanho máximo do comando é 256 caracteres
// O comando deve ser terminado com '\n' (caractere de nova linha)
int await_command(char *command);

/*
 Tokenizes the command.

 @param *command The command to tokenize.
 @param **args The target tokens array.

 @returns The token count.
 @returns 0 if anything fails to tokenize.
*/
int tokenize_cmd(char *command, char **args);

/*
 Analisa um comando lido do utilizador
 O comando deve ser uma string terminada em '\0'

 @param *command Pointer to the string command to parse.
 @param *parsed_cmd Pointer to the target parsed command.

 @return 1 if unable to parse.
 @return 0 otherwise.
*/
int parse_command(Tab *tab, char *command, ParsedCommand *parsed_cmd);

/*
 Executa um comando analisado.

 @param *cmd Um comando válido.
 @param **tab Referência do tabuleiro. Permite ajustes pelo comando.

 @returns 0 se o comando for executado com sucesso, 1 caso contrário.
*/
int run_command(ParsedCommand *cmd, Tab *tab);

// Carrega um tabuleiro a partir de um ficheiro especificado
// @returns 0 se o carregamento for bem-sucedido, 1 caso contrário
int carregar_tabuleiro(Tab *tab, const char *filename);

// Guarda o estado atual do tabuleiro num ficheiro
// Retorna 1 se a gravação for bem-sucedida, 0 caso contrário
int salvar_tabuleiro(Tab *tab, const char *filename);

/*
 Verifica se o tabuleiro é válido segundo as regras do jogo
 Confirma que não há repetições de caracteres nas linhas e colunas

 @returns `true` se o tabuleiro for válido, `false` caso contrário
*/
bool validar_tabuleiro(Tab *tab);
