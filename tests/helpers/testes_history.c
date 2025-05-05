#include "../include/jogo/comandos.h"
#include "../include/helpers/strs.h"
#include "../include/jogo/tabuleiro.h"
#include "../include/helpers/memory.h"
#include "../include/helpers/history.h"
#include <CUnit/CUnit.h>
#include <stdlib.h>

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

void testes_history(void)
{
    test_push_history();
    test_pop_history();
    test_get_history_element();
}
