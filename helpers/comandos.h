#include "../jogo/tabuleiro.h"

typedef struct
{
    int code;
    char **args;
} Command;

typedef enum {
    CMD_INVALID = 0,
    CMD_SAVE,     // g <filename>
    CMD_LOAD,     // l <filename>
    CMD_SELECT,   // <coord>
    CMD_WHITE,    // b <coord>
    CMD_CROSS,    // r <coord>
    CMD_VERIFY,   // v
    CMD_HELP,     // a
    CMD_HELP_ALL, // A
    CMD_SOLVE,    // R
    CMD_UNDO,     // d
    CMD_EXIT      // s
} CommandType;

typedef struct {
    CommandType type;
    char args[2][32];
    int valid;
} ParsedCommand;

// Aguarda um comando do utilizador
// Lê uma linha de entrada e remove o caractere de nova linha
// Retorna 1 se a leitura for bem-sucedida, 0 caso contrário
// O comando lido é armazenado na variável 'command'
// O tamanho máximo do comando é 256 caracteres
// O comando deve ser terminado com '\n' (caractere de nova linha)

int await_command(char *command);

// Analisa um comando lido do utilizador
// Retorna um ParsedCommand com o tipo de comando e os argumentos
// O comando deve ser uma string terminada em '\0'
ParsedCommand parse_command(char *command);

// Executa um comando analisado
// O comando é executado com base no tipo e argumentos fornecidos
// Retorna 1 se o comando for executado com sucesso, 0 caso contrário
// O tabuleiro é passado como referência para permitir modificações
// O comando pode ser um comando de salvar, carregar, selecionar, marcar, verificar, etc.
// O comando deve ser um ParsedCommand com o tipo e os argumentos
int run_command(ParsedCommand cmd, Tab **tab); 
    

// Carrega um tabuleiro a partir de um ficheiro especificado
// Retorna 1 se o carregamento for bem-sucedido, 0 caso contrário
int carregar_tabuleiro(Tab **tab, const char *filename);

// Guarda o estado atual do tabuleiro num ficheiro
// Retorna 1 se a gravação for bem-sucedida, 0 caso contrário
int salvar_tabuleiro(Tab *tab, const char *filename);

// Verifica se o tabuleiro é válido segundo as regras do jogo
// Confirma que não há repetições de caracteres nas linhas e colunas
// Retorna 1 se o tabuleiro for válido, 0 caso contrário
int validar_tabuleiro(Tab *tab);

