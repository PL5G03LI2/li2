#include "../include/jogo/comandos.h"
#include "../include/helpers/strs.h"
#include "../include/jogo/tabuleiro.h"
#include "../include/helpers/memory.h"
#include "../include/helpers/history.h"
#include <CUnit/CUnit.h>
#include <stdlib.h>
#include <ncurses.h>

void test_isUpper(void)
{
    CU_ASSERT_EQUAL(isUpper('c'), 0);
    CU_ASSERT_NOT_EQUAL(isUpper('A'), 0);
}

void test_toUpper(void)
{
    CU_ASSERT_EQUAL(toUpper('c'), 'C');
    CU_ASSERT_EQUAL(toUpper('A'), 'A');
}

void test_isLower(void)
{
    CU_ASSERT_EQUAL(isLower('A'), 0);
    CU_ASSERT_NOT_EQUAL(isLower('c'), 0);
}

void test_toLower(void)
{
    CU_ASSERT_EQUAL(toLower('c'), 'c');
    CU_ASSERT_EQUAL(toLower('A'), 'a');
}

void test_trim_str_basic(void)
{
    char *input = strdup("   hello world   ");
    char *result = trim_str(input);
    CU_ASSERT_STRING_EQUAL(result, "hello world");
    free(input);
    free(result);
}

void test_trim_str_no_spaces(void)
{
    char *input = strdup("Hello");
    char *result = trim_str(input);
    CU_ASSERT_STRING_EQUAL(result, "Hello");
    free(input);
    free(result);
}

void test_trim_str_all_spaces(void)
{
    char *input = strdup("     ");
    char *result = trim_str(input);
    CU_ASSERT_STRING_EQUAL(result, "");
    free(input);
    free(result);
}

void test_trim_str_empty(void)
{
    char *input = strdup("");
    char *result = trim_str(input);
    CU_ASSERT_STRING_EQUAL(result, "");
    free(input);
    free(result);
}

void test_trim_str_null(void)
{
    char *result = trim_str(NULL);
    CU_ASSERT_PTR_NULL(result);
}

void test_trim_str(void)
{
    test_trim_str_basic();
    test_trim_str_no_spaces();
    test_trim_str_all_spaces();
    test_trim_str_empty();
    test_trim_str_null();
}

void test_print_info(void)
{
    initscr(); // Start curses mode
    noecho();  // Don't echo input
    cbreak();  // Disable line buffering

    const char *test_str = "Hello, world!";
    iVec2 win_size = {.x = 80, .y = 24};

    print_info(test_str, win_size);

    endwin(); // End curses mode
}

void test_strings(void)
{
    test_isUpper();
    test_toUpper();
    test_isLower();
    test_toLower();
    test_trim_str();
    test_print_info();
}

void test_push_history(void)
{
    TabHistory *history = NULL;
    // memset(history, 0, sizeof(TabHistory));

    // Create a dummy ParsedCommand
    ParsedCommand *cmd1 = malloc(sizeof(ParsedCommand));
    cmd1->type = CMD_SAVE;
    cmd1->track = true;
    cmd1->tokens = (char **)malloc(sizeof(char *) * 2);
    cmd1->tokens[0] = strdup("xyz");
    cmd1->tokens[1] = strdup("");

    // Push first command
    history = push_history(history, cmd1);
    CU_ASSERT_PTR_NOT_NULL(history);
    CU_ASSERT_PTR_NOT_NULL(history->cmd);
    CU_ASSERT_EQUAL(history->cmd->type, CMD_SAVE);
    CU_ASSERT_EQUAL(history->cmd->track, true);
    CU_ASSERT_PTR_NULL(history->next);

    // Push a second command
    ParsedCommand *cmd2 = malloc(sizeof(ParsedCommand));
    cmd2->type = CMD_LOAD;
    cmd2->track = false;
    cmd2->tokens = (char **)malloc(sizeof(char *) * 2);
    cmd2->tokens[0] = strdup("g");
    cmd2->tokens[1] = strdup("j1.txt");

    history = push_history(history, cmd2);

    // Find second node
    TabHistory *second = history->next;
    CU_ASSERT_PTR_NOT_NULL(second);
    CU_ASSERT_PTR_NOT_NULL(second->cmd);
    CU_ASSERT_EQUAL(second->cmd->type, CMD_LOAD);
    CU_ASSERT_EQUAL(second->cmd->track, false);
    CU_ASSERT_PTR_NULL(second->next);

    // Clean up
    free_command(&cmd1);
    free_command(&cmd2);

    destroy_history(&history);
}

void test_pop_history(void)
{
    TabHistory *history = NULL;
    ParsedCommand *removed;

    // Pop from empty history
    removed = pop_history(&history);
    CU_ASSERT_PTR_NULL(removed);

    // Push first command
    ParsedCommand *cmd1 = malloc(sizeof(ParsedCommand));
    cmd1->type = CMD_SAVE;
    cmd1->track = true;
    cmd1->tokens = (char **)malloc(sizeof(char *) * 2);
    cmd1->tokens[0] = strdup("g");
    cmd1->tokens[1] = strdup("j1.txt");
    history = push_history(history, cmd1);

    // Pop when only one element
    removed = pop_history(&history);
    CU_ASSERT_PTR_NOT_NULL(removed);
    CU_ASSERT_EQUAL(removed->type, CMD_SAVE);
    CU_ASSERT_EQUAL(removed->track, true);
    CU_ASSERT_PTR_NULL(history);
    free_command(&removed);
    free_command(&cmd1);

    // Push two commands
    ParsedCommand *cmd2 = malloc(sizeof(ParsedCommand));
    ParsedCommand *cmd3 = malloc(sizeof(ParsedCommand));

    cmd2->type = CMD_LOAD;
    cmd2->track = false;
    cmd2->tokens = (char **)malloc(sizeof(char *) * 2);
    cmd2->tokens[0] = strdup("l");
    cmd2->tokens[1] = strdup("j1.txt");
    history = push_history(history, cmd2);

    cmd3->type = CMD_VERIFY;
    cmd3->track = true;
    cmd3->tokens = (char **)malloc(sizeof(char *) * 2);
    cmd3->tokens[0] = strdup("v");
    cmd3->tokens[1] = strdup("");
    history = push_history(history, cmd3);

    // Pop last command (cmd3)
    removed = pop_history(&history);
    CU_ASSERT_PTR_NOT_NULL(removed);
    CU_ASSERT_EQUAL(removed->type, CMD_VERIFY);
    CU_ASSERT_EQUAL(removed->track, true);
    free_command(&removed);

    // Check remaining head is cmd2
    CU_ASSERT_PTR_NOT_NULL(history);
    CU_ASSERT_PTR_NULL(history->next);
    CU_ASSERT_EQUAL(history->cmd->type, CMD_LOAD);

    // Pop last remaining
    removed = pop_history(&history);
    CU_ASSERT_PTR_NOT_NULL(removed);
    CU_ASSERT_EQUAL(removed->type, CMD_LOAD);
    free_command(&removed);
    free_command(&cmd2);
    free_command(&cmd3);

    destroy_history(&history);
    CU_ASSERT_PTR_NULL(history);
}

void test_get_history_element(void)
{
    TabHistory *history = NULL;
    ParsedCommand *result;

    // Test empty history
    result = get_history_element(history, 0);
    CU_ASSERT_PTR_NULL(result);

    // Push commands
    ParsedCommand *cmd1 = malloc(sizeof(ParsedCommand));
    ParsedCommand *cmd2 = malloc(sizeof(ParsedCommand));
    ParsedCommand *cmd3 = malloc(sizeof(ParsedCommand));

    cmd1->type = CMD_SAVE;
    cmd1->track = false;
    cmd1->tokens = (char **)malloc(sizeof(char *) * 2);
    cmd1->tokens[0] = strdup("g");
    cmd1->tokens[1] = strdup("j1.txt");
    history = push_history(history, cmd1);

    cmd2->type = CMD_LOAD;
    cmd2->track = true;
    cmd2->tokens = (char **)malloc(sizeof(char *) * 2);
    cmd2->tokens[0] = strdup("l");
    cmd2->tokens[1] = strdup("j1.txt");
    history = push_history(history, cmd2);

    cmd3->type = CMD_VERIFY;
    cmd3->track = false;
    cmd3->tokens = (char **)malloc(sizeof(char *) * 2);
    cmd3->tokens[0] = strdup("v");
    cmd3->tokens[1] = strdup("");
    history = push_history(history, cmd3);

    // Valid indices
    result = get_history_element(history, 0);
    CU_ASSERT_PTR_NOT_NULL(result);
    CU_ASSERT_EQUAL(result->type, CMD_SAVE);

    result = get_history_element(history, 1);
    CU_ASSERT_PTR_NOT_NULL(result);
    CU_ASSERT_EQUAL(result->type, CMD_LOAD);

    result = get_history_element(history, 2);
    CU_ASSERT_PTR_NOT_NULL(result);
    CU_ASSERT_EQUAL(result->type, CMD_VERIFY);

    // Out-of-bounds index
    result = get_history_element(history, 3);
    CU_ASSERT_PTR_NULL(result);

    // Negative index
    result = get_history_element(history, -1);
    CU_ASSERT_PTR_NULL(result);

    // Clean up
    destroy_history(&history);
    free_command(&cmd1);
    free_command(&cmd2);
    free_command(&cmd3);
}

void test_destroy_history(void)
{
    TabHistory *history = NULL;

    // Case 1: destroy empty list
    destroy_history(&history);
    CU_ASSERT_PTR_NULL(history);

    // Case 2: destroy populated list
    ParsedCommand *cmd1 = malloc(sizeof(ParsedCommand));
    ParsedCommand *cmd2 = malloc(sizeof(ParsedCommand));

    cmd1->type = CMD_SAVE;
    cmd1->track = false;
    cmd1->tokens = (char **)malloc(sizeof(char *) * 2);
    cmd1->tokens[0] = strdup("g");
    cmd1->tokens[1] = strdup("j1.txt");

    cmd2->type = CMD_LOAD;
    cmd2->track = true;
    cmd2->tokens = (char **)malloc(sizeof(char *) * 2);
    cmd2->tokens[0] = strdup("l");
    cmd2->tokens[1] = strdup("j1.txt");

    history = push_history(history, cmd1);
    history = push_history(history, cmd2);

    CU_ASSERT_PTR_NOT_NULL(history);

    destroy_history(&history);
    CU_ASSERT_PTR_NULL(history);

    free_command(&cmd1);
    free_command(&cmd2);
}

void test_history(void)
{
    test_push_history();
    test_pop_history();
    test_get_history_element();
    test_destroy_history();
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

void testes_helpers(void)
{
    test_strings();
    test_history();
    test_free_game();
}
