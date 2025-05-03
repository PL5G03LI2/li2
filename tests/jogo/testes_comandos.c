#include <CUnit/CUnit.h>
#include <stdlib.h>
#include <ncurses.h>

#include "helpers/history.h"
#include "helpers/memory.h"
#include "helpers/coords.h"

#include "jogo/comandos.h"
#include "jogo/tabuleiro.h"
#include "jogo/saves.h"
#include "testes_tabuleiro.h"

#include "types.h"

void populateTabuleiro(Tab *tab)
{
    if (!tab->data || tab->height < 1 || tab->width < 1)
        return;

    for (int i = 0; i < tab->height * tab->width; i++)
    {
        tab->data[i].c = i % 26 + 'a';
    }
}

void test_undo_command(void)
{
    // Test 1: Undo CMD_WHITE
    ParsedCommand cmd_white;
    cmd_white.type = CMD_WHITE;
    cmd_white.tokens = (char **)malloc(sizeof(char *) * 2);
    cmd_white.tokens[0] = strdup("b");
    cmd_white.tokens[1] = strdup("a1");

    Tab tab_white;
    tab_white.height = 5;
    tab_white.width = 5;
    tab_white.data = malloc(sizeof(Piece) * tab_white.height * tab_white.width);

    populateTabuleiro(&tab_white);

    // Call undo command
    CU_ASSERT_EQUAL(undo_command(&cmd_white, &tab_white), 0);

    // Cleanup
    free(cmd_white.tokens[1]);
    free(cmd_white.tokens[0]);
    free(cmd_white.tokens);
    free(tab_white.data);

    // Test 2: Undo CMD_CROSS
    ParsedCommand cmd_cross;
    cmd_cross.type = CMD_CROSS;
    cmd_cross.tokens = (char **)malloc(sizeof(char *) * 2);
    cmd_cross.tokens[0] = strdup("r");
    cmd_cross.tokens[1] = strdup("b2");

    Tab tab_cross;
    tab_cross.height = 5;
    tab_cross.width = 5;
    tab_cross.data = malloc(sizeof(Piece) * tab_cross.height * tab_cross.width);

    populateTabuleiro(&tab_cross);

    // Call undo command
    CU_ASSERT_EQUAL(undo_command(&cmd_cross, &tab_cross), 0);

    // Cleanup
    free(cmd_cross.tokens[1]);
    free(cmd_cross.tokens[0]);
    free(cmd_cross.tokens);
    free(tab_cross.data);
}

void test_reset_cmd(void)
{
    // Test 1: Normal case with initialized command
    ParsedCommand cmd;
    cmd.type = CMD_LOAD;
    cmd.track = true;
    cmd.tokens = (char **)malloc(sizeof(char *) * 2);
    cmd.tokens[0] = strdup("token1");
    cmd.tokens[1] = strdup("token2");

    // Call the reset function
    reset_cmd(&cmd);

    // Assert that the command is reset properly
    CU_ASSERT_EQUAL(cmd.type, CMD_INVALID);
    CU_ASSERT_FALSE(cmd.track);
    CU_ASSERT_PTR_NULL(cmd.tokens); // reset_cmd_tokens should set this to NULL

    // Test 2: Edge case with already reset command
    ParsedCommand cmd_reset;
    cmd_reset.type = CMD_INVALID;
    cmd_reset.track = false;
    cmd_reset.tokens = NULL;

    // Call the reset function again, should do nothing
    reset_cmd(&cmd_reset);

    // Assert that the command remains in the reset state
    CU_ASSERT_EQUAL(cmd_reset.type, CMD_INVALID);
    CU_ASSERT_FALSE(cmd_reset.track);
    CU_ASSERT_PTR_NULL(cmd_reset.tokens);

    // Test 3: Edge case with NULL tokens initially
    ParsedCommand cmd_null_tokens;
    cmd_null_tokens.type = CMD_SAVE;
    cmd_null_tokens.track = true;
    cmd_null_tokens.tokens = NULL;

    // Call the reset function, should handle NULL tokens gracefully
    reset_cmd(&cmd_null_tokens);

    // Assert that the command is reset properly even with NULL tokens
    CU_ASSERT_EQUAL(cmd_null_tokens.type, CMD_INVALID);
    CU_ASSERT_FALSE(cmd_null_tokens.track);
    CU_ASSERT_PTR_NULL(cmd_null_tokens.tokens);
}

void test_reset_cmd_tokens(void)
{
    // Test 1: Normal case with valid tokens
    ParsedCommand cmd;
    cmd.tokens = (char **)malloc(sizeof(char *) * 2);
    cmd.tokens[0] = strdup("token1");
    cmd.tokens[1] = strdup("token2");

    // Call function to reset tokens
    reset_cmd_tokens(&cmd);

    // Assert that the tokens are freed and set to NULL
    CU_ASSERT_PTR_NULL(cmd.tokens);

    // Test 2: Edge case with NULL tokens
    ParsedCommand cmd_null;
    cmd_null.tokens = NULL;

    // Call function, should do nothing
    reset_cmd_tokens(&cmd_null);

    // Assert that the tokens are still NULL
    CU_ASSERT_PTR_NULL(cmd_null.tokens);

    // Test 3: Edge case with empty token array (already allocated with size 0)
    ParsedCommand cmd_empty;
    // cmd_empty.tokens = (char **)malloc(0); // Allocating 0 size should leave the pointer NULL
    cmd_empty.tokens = NULL; // manually setting it to NULL for this case

    // Call function, should handle gracefully
    reset_cmd_tokens(&cmd_empty);

    // Assert that the tokens are NULL
    CU_ASSERT_PTR_NULL(cmd_empty.tokens);
}

void test_tokenize(void)
{
    char input[] = "l j1.txt";
    char *tokens[2];

    int count = tokenize_cmd(input, tokens);

    CU_ASSERT_EQUAL(count, 2);
    CU_ASSERT_STRING_EQUAL(tokens[0], "l");
    CU_ASSERT_STRING_EQUAL(tokens[1], "j1.txt");

    for (int i = 0; i < count; i++)
        free(tokens[i]);
}

void test_parseCommand_save(void)
{
    Game jogo;
    Tab tab = {.height = 5, .width = 5, .data = NULL};
    ParsedCommand cmd;
    memset(&cmd, 0, sizeof(ParsedCommand));

    char cmd_str[] = "g j1.txt";
    jogo.tabuleiro = &tab;
    jogo.cmd_str = cmd_str;
    jogo.cmd = &cmd;

    CU_ASSERT_EQUAL(parse_command(&jogo), 0);
    CU_ASSERT_EQUAL(jogo.cmd->type, CMD_SAVE);

    // cleanup
    for (int i = 0; i < 2; i++)
        free(jogo.cmd->tokens[i]);
    free(jogo.cmd->tokens);
}

void test_parseCommand_select(void)
{
    Game jogo;
    Tab tab = {.height = 5, .width = 5, .data = NULL};
    ParsedCommand cmd;
    memset(&cmd, 0, sizeof(ParsedCommand));

    char cmd_str[] = "xyz"; // > 1 char, so it triggers CMD_SELECT
    jogo.tabuleiro = &tab;
    jogo.cmd_str = cmd_str;
    jogo.cmd = &cmd;

    CU_ASSERT_EQUAL(parse_command(&jogo), 0);
    CU_ASSERT_EQUAL(jogo.cmd->type, CMD_SELECT);
    CU_ASSERT_STRING_EQUAL(jogo.cmd->tokens[0], "xyz");

    // cleanup
    for (int i = 0; jogo.cmd->tokens[i]; i++)
        free(jogo.cmd->tokens[i]);
    free(jogo.cmd->tokens);
}

void test_parseCommand_empty(void)
{
    Game jogo;
    Tab tab = {.height = 5, .width = 5, .data = NULL};
    ParsedCommand cmd;
    memset(&cmd, 0, sizeof(ParsedCommand));

    char cmd_str[] = "";
    jogo.tabuleiro = &tab;
    jogo.cmd_str = cmd_str;
    jogo.cmd = &cmd;

    CU_ASSERT_EQUAL(parse_command(&jogo), 0);
    CU_ASSERT_EQUAL(jogo.cmd->type, CMD_CONTINUE);
}

void test_parseCommand(void)
{
    test_parseCommand_save();
    test_parseCommand_select();
    test_parseCommand_empty();
}

void test_runCommand(void)
{
    Game jogo;
    init_game(&jogo);

    // allocate command string
    strcpy(jogo.cmd_str, "l j1.txt");
    parse_command(&jogo);

    jogo.win_d = (iVec2){80, 45};

    CU_ASSERT_EQUAL(run_command(&jogo), carregar_jogo(&jogo, "j1.txt"));

    // Cleanup
    free_game(&jogo);
}

void testes_comandos(void)
{
    test_undo_command();
    test_reset_cmd();
    test_reset_cmd_tokens();
    test_tokenize();
    test_parseCommand();
    test_runCommand();
}
