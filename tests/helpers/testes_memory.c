#include "../include/jogo/comandos.h"
#include "../include/helpers/strs.h"
#include "../include/jogo/tabuleiro.h"
#include "../include/helpers/memory.h"
#include "../include/helpers/history.h"
#include <CUnit/CUnit.h>
#include <stdlib.h>

void test_initialize_tabuleiro(void)
{
    Tab *tab = initialize_tabuleiro();

    CU_ASSERT_PTR_NOT_NULL(tab);
    CU_ASSERT_EQUAL(tab->height, 0);
    CU_ASSERT_EQUAL(tab->width, 0);
    CU_ASSERT_EQUAL(tab->sel_piece.x, 0);
    CU_ASSERT_EQUAL(tab->sel_piece.y, 0);
    CU_ASSERT_PTR_NULL(tab->data);

    free(tab);
}

void test_free_command(void)
{
    // Caso 1: dois tokens.
    ParsedCommand *cmd1 = (ParsedCommand *)malloc(sizeof(ParsedCommand));
    cmd1->tokens = (char **)malloc(sizeof(char *) * 2);
    cmd1->tokens[0] = strdup("l");
    cmd1->tokens[1] = strdup("j1.txt");

    free_command(&cmd1);

    CU_ASSERT_PTR_NULL(cmd1);

    // Caso 2: cmd nulo.
    ParsedCommand *cmd2 = NULL;

    free_command(&cmd2);

    CU_ASSERT_PTR_NULL(cmd2);

    // Caso 3: um token nulo.
    ParsedCommand *cmd3 = (ParsedCommand *)malloc(sizeof(ParsedCommand));
    cmd3->tokens = (char **)malloc(sizeof(char *) * 2);
    cmd3->tokens[0] = NULL;
    cmd3->tokens[1] = strdup("j1.txt");

    free_command(&cmd3);

    CU_ASSERT_PTR_NULL(cmd3);

    // Caso 4: tokens nulos.
    ParsedCommand *cmd4 = (ParsedCommand *)malloc(sizeof(ParsedCommand));
    cmd4->tokens = NULL;
    
    free_command(&cmd4);
    
    CU_ASSERT_PTR_NOT_NULL(cmd4);
}

void test_free_game_basic(void)
{
    // allocate tabuleiro
    Tab *tab = (Tab *)malloc(sizeof(Tab));
    tab->data = (Piece *)malloc(sizeof(Piece) * 4);
    tab->height = 2;
    tab->width = 2;

    // allocate command string
    char *cmd_str = strdup("l file.txt");

    // allocate ParsedCommand with 2 tokens
    ParsedCommand *cmd = (ParsedCommand *)malloc(sizeof(ParsedCommand));
    cmd->track = false;
    cmd->type = CMD_SAVE;
    cmd->tokens = (char **)malloc(sizeof(char *) * 2);
    cmd->tokens[0] = strdup("g");
    cmd->tokens[1] = strdup("j1.txt");

    iVec2 win_d = {80, 45};

    TabHistory *history = NULL;

    Game jogo;
    jogo.tabuleiro = tab;
    jogo.history = history;
    jogo.cmd_str = cmd_str;
    jogo.cmd = cmd;
    jogo.win_d = win_d;

    free_game(&jogo);
    CU_ASSERT_PTR_NULL(jogo.tabuleiro);
    CU_ASSERT_PTR_NULL(jogo.cmd_str);
    CU_ASSERT_PTR_NULL(jogo.cmd);
    CU_ASSERT_PTR_NULL(jogo.history);
}

void test_free_game_no_data(void)
{
    Tab *tab = (Tab *)malloc(sizeof(Tab));
    tab->data = NULL;

    char *cmd_str = NULL;

    ParsedCommand *cmd = (ParsedCommand *)malloc(sizeof(ParsedCommand));
    cmd->tokens = (char **)malloc(sizeof(char *) * 2);
    cmd->tokens[0] = strdup("");
    cmd->tokens[1] = strdup("");

    iVec2 win_d = {80, 45};

    Game jogo;
    jogo.tabuleiro = tab;
    jogo.history = NULL;
    jogo.cmd_str = cmd_str;
    jogo.cmd = cmd;
    jogo.win_d = win_d;

    free_game(&jogo);
    CU_ASSERT_PTR_NULL(jogo.tabuleiro);
    CU_ASSERT_PTR_NULL(jogo.cmd_str);
    CU_ASSERT_PTR_NULL(jogo.cmd);
    CU_ASSERT_PTR_NULL(jogo.history);
}

void test_free_game(void)
{
    test_free_game_basic();
    test_free_game_no_data();
}

void testes_memory(void) {
    test_initialize_tabuleiro();

    test_free_command();
    test_free_game();
}
