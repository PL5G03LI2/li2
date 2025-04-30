#include "../include/jogo/comandos.h"
#include "../include/jogo/tabuleiro.h"
#include "../include/helpers/history.h"
#include "helpers/memory.h"
#include <CUnit/CUnit.h>
#include <stdlib.h>
#include <ncurses.h>

void populateTab(Tab *tab)
{
    if (!tab->data || tab->height < 1 || tab->width < 1)
        return;

    for (int i = 0; i < tab->height * tab->width; i++)
    {
        tab->data[i].c = i % 26 + 'a';
    }
}

void test_calc_index_valid_position(void)
{
    Tab tab;
    tab.height = 5;
    tab.width = 5;
    tab.data = malloc(sizeof(Piece) * tab.height * tab.width);

    iVec2 valid_pos = {2, 3};
    int valid_index = calc_index(&tab, valid_pos.x, valid_pos.y); // valid position
    CU_ASSERT_TRUE(assert_pos(&tab, valid_pos.x, valid_pos.y));
    CU_ASSERT_EQUAL(valid_index, 2 * tab.height + 3);

    iVec2 invalid_pos = {-1, 2};
    int invalid_index = calc_index(&tab, invalid_pos.x, invalid_pos.y);
    CU_ASSERT_FALSE(assert_pos(&tab, invalid_pos.x, invalid_pos.y));
    CU_ASSERT_EQUAL(invalid_index, -1);

    free(tab.data);
}

void test_calc_index_invalid_position(void)
{
    Tab tab;
    tab.height = 5;
    tab.width = 5;
    tab.data = malloc(sizeof(Piece) * tab.height * tab.width);

    // x out of bounds
    CU_ASSERT_EQUAL(calc_index(&tab, -1, 0), -1);
    CU_ASSERT_EQUAL(calc_index(&tab, 5, 0), -1);

    // y out of bounds
    CU_ASSERT_EQUAL(calc_index(&tab, 0, -1), -1);
    CU_ASSERT_EQUAL(calc_index(&tab, 0, 5), -1);

    free(tab.data);
}

void test_calc_index(void)
{
    test_calc_index_invalid_position();
    test_calc_index_valid_position();
}

void test_assert_index_valid(void)
{
    Tab tab;
    tab.height = 4;
    tab.width = 4;
    tab.data = malloc(sizeof(Piece) * tab.height * tab.width);

    // Valid indices: 0 through 16 (inclusive based on your implementation)
    for (int i = 0; i < 16; i++)
    {
        CU_ASSERT_TRUE(assert_index(&tab, i));
    }

    free(tab.data);
}

void test_assert_index_invalid(void)
{
    Tab tab;
    tab.height = 4;
    tab.width = 4;
    tab.data = malloc(sizeof(Piece) * tab.height * tab.width);

    // Invalid indices: negative and greater than 16
    CU_ASSERT_FALSE(assert_index(&tab, -1));
    CU_ASSERT_FALSE(assert_index(&tab, 17));
    CU_ASSERT_FALSE(assert_index(&tab, 100));

    free(tab.data);
}

void test_assert_index(void)
{
    test_assert_index_invalid();
    test_assert_index_valid();
}

void test_positions_validity(void)
{
    Tab tab;
    tab.data = NULL;
    tab.height = 5;
    tab.width = 5;

    CU_ASSERT_TRUE(assert_pos(&tab, 2, 3));
    CU_ASSERT_TRUE(assert_pos(&tab, 0, 0));
    CU_ASSERT_TRUE(assert_pos(&tab, 4, 4));

    CU_ASSERT_FALSE(assert_pos(&tab, -1, 2));
    CU_ASSERT_FALSE(assert_pos(&tab, 5, 2));
    CU_ASSERT_FALSE(assert_pos(&tab, 2, 5));
}

void test_get_elem(void)
{
    Tab tab;
    tab.height = 5;
    tab.width = 5;
    tab.data = malloc(sizeof(Piece) * tab.height * tab.width);
    populateTab(&tab);

    // Inside bounds
    CU_ASSERT_EQUAL(get_elem(&tab, 1, 2), 'h'); // 'h' is at (1,2)
    CU_ASSERT_EQUAL(get_elem(&tab, 0, 0), 'a'); // Top-left
    CU_ASSERT_EQUAL(get_elem(&tab, 4, 4), 'y'); // Bottom-right

    // Out of bounds checks
    CU_ASSERT_EQUAL(get_elem(&tab, -1, 2), '\0'); // Negative x
    CU_ASSERT_EQUAL(get_elem(&tab, 5, 2), '\0');  // x >= width
    CU_ASSERT_EQUAL(get_elem(&tab, 2, 5), '\0');  // y >= height

    free(tab.data);
}

void test_set_elem(void)
{
    Tab tab;
    tab.height = 5;
    tab.width = 5;
    tab.data = malloc(sizeof(Piece) * tab.height * tab.width);

    populateTab(&tab);

    CU_ASSERT_NOT_EQUAL(tab.data[0].c, 'c');
    set_elem(&tab, 0, 0, 'c');
    CU_ASSERT_EQUAL(tab.data[0].c, 'c');

    free(tab.data);
}

void test_toggle_branco(void)
{
    Tab tab;
    tab.height = 5;
    tab.width = 5;
    tab.data = malloc(sizeof(Piece) * tab.height * tab.width);

    populateTab(&tab);

    CU_ASSERT_EQUAL(tab.data[0].c, 'a');
    toggle_branco(&tab, 0, 0);
    CU_ASSERT_EQUAL(tab.data[0].c, 'A');

    free(tab.data);
}

void test_toggle_marked(void)
{
    Tab tab;
    tab.height = 3;
    tab.width = 3;
    tab.data = malloc(sizeof(Piece) * tab.height * tab.width);

    // Inicializa todas as peças com letras minúsculas e marked = 0
    char c = 'a';
    for (int i = 0; i < tab.height * tab.width; i++)
    {
        tab.data[i].c = c++;
        tab.data[i].marked = 0;
    }

    // Aplica toggle_marked no centro (1, 1)
    toggle_marked(&tab, 1, 1);

    int center_idx = calc_index(&tab, 1, 1);
    CU_ASSERT_TRUE(tab.data[center_idx].marked);

    free(tab.data);
}

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

void test_tabuleiroState_SL(void)
{
    Game game;

    init_game(&game);

    CU_ASSERT_FALSE(carregar_jogo(&game, "j1.txt"));
    CU_ASSERT_TRUE(carregar_jogo(&game, "j2.txt"));
    CU_ASSERT_FALSE(salvar_jogo(&game, "j3.txt"));

    // a brand new board is not invalid by default
    CU_ASSERT_TRUE(validar_tabuleiro(game.tabuleiro));

    free_game(&game);
}

void test_tabuleiroState(void)
{
    test_tabuleiroState_SL();
    test_initialize_tabuleiro();
}

void testes_tabuleiro(void)
{
    test_tabuleiroState();
    // test_print_tabuleiro();
    test_toggle_branco();
    test_toggle_marked();
}

void test_read_coordinate(void)
{
    iVec2 result;

    // Test "a1"
    result = read_coordinate("a1");
    CU_ASSERT_EQUAL(result.x, 0);
    CU_ASSERT_EQUAL(result.y, 0);

    // Test "b2"
    result = read_coordinate("b2");
    CU_ASSERT_EQUAL(result.x, 1);
    CU_ASSERT_EQUAL(result.y, 1);

    // Test "c5"
    result = read_coordinate("c5");
    CU_ASSERT_EQUAL(result.x, 2);
    CU_ASSERT_EQUAL(result.y, 4);

    // Test "aa1" -> (26,0)
    result = read_coordinate("aa1");
    CU_ASSERT_EQUAL(result.x, 26);
    CU_ASSERT_EQUAL(result.y, 0);

    // Test "ab10" -> (25+1,9)
    result = read_coordinate("ab10");
    CU_ASSERT_EQUAL(result.x, 27);
    CU_ASSERT_EQUAL(result.y, 9);

    // Test "z1" -> (25,0)
    result = read_coordinate("z1");
    CU_ASSERT_EQUAL(result.x, 25);
    CU_ASSERT_EQUAL(result.y, 0);

    // Test "a12" -> (0,11)
    result = read_coordinate("a12");
    CU_ASSERT_EQUAL(result.x, 0);
    CU_ASSERT_EQUAL(result.y, 11);

    // Test empty string ""
    result = read_coordinate("");
    CU_ASSERT_EQUAL(result.x, 0);
    CU_ASSERT_EQUAL(result.y, 0);

    // Test no number "b"
    result = read_coordinate("b");
    CU_ASSERT_EQUAL(result.x, 0);
    CU_ASSERT_EQUAL(result.y, 0);

    // Test no letter "3"
    result = read_coordinate("3");
    CU_ASSERT_EQUAL(result.x, 0);
    CU_ASSERT_EQUAL(result.y, 0);

    // Edge Case: Malformed input "a1b2" -> should return default (0,0) or similar
    result = read_coordinate("a1b2");
    CU_ASSERT_EQUAL(result.x, 0);
    CU_ASSERT_EQUAL(result.y, 0);

    // Edge Case: "1a" is malformed -> should return default (0,0) or similar
    result = read_coordinate("1a");
    CU_ASSERT_EQUAL(result.x, 0);
    CU_ASSERT_EQUAL(result.y, 0);

    // Edge Case: "abc" (no numbers, only letters) -> should return default (0,0)
    result = read_coordinate("abc");
    CU_ASSERT_EQUAL(result.x, 0);
    CU_ASSERT_EQUAL(result.y, 0);

    // Edge Case: "123" (only digits, no letters) -> should return default (0,0)
    result = read_coordinate("123");
    CU_ASSERT_EQUAL(result.x, 0);
    CU_ASSERT_EQUAL(result.y, 0);

    // Edge Case: Invalid input "@" -> should return default (0,0)
    result = read_coordinate("@");
    CU_ASSERT_EQUAL(result.x, 0);
    CU_ASSERT_EQUAL(result.y, 0);
}

void test_write_coordinate(void)
{
    char buffer[32]; // Ensure large enough buffer for coordinates
    iVec2 result;

    // Test (0,0) -> expected "a1"
    result.x = 0;
    result.y = 0;
    CU_ASSERT_STRING_EQUAL(write_coordinate(result, buffer), "a1");

    // Test (25,0) -> expected "z1"
    result.x = 25;
    result.y = 0;
    CU_ASSERT_STRING_EQUAL(write_coordinate(result, buffer), "z1");

    // Test (26,0) -> expected "aa1"
    result.x = 26;
    result.y = 0;
    CU_ASSERT_STRING_EQUAL(write_coordinate(result, buffer), "aa1");

    // Test (27,1) -> expected "ab2"
    result.x = 27;
    result.y = 1;
    CU_ASSERT_STRING_EQUAL(write_coordinate(result, buffer), "ab2");

    // Test (51,10) -> expected "bz11"
    result.x = 2 * 26 + 25;
    result.y = 10;
    CU_ASSERT_STRING_EQUAL(write_coordinate(result, buffer), "bz11");

    // Test (52,9) -> expected "ca10"
    result.x = 3 * 26 + 0;
    result.y = 9;
    CU_ASSERT_STRING_EQUAL(write_coordinate(result, buffer), "ca10");

    // Test (26 * 26 + 25, 5) -> expected "zz6"
    result.x = 26 * 26 + 25;
    result.y = 5;
    CU_ASSERT_STRING_EQUAL(write_coordinate(result, buffer), "zz6");

    // Test large y-value (e.g., (0, 100)) -> expected "a101"
    result.x = 0;
    result.y = 100;
    CU_ASSERT_STRING_EQUAL(write_coordinate(result, buffer), "a101");

    // Test large x and y value (e.g., (52, 99)) -> expected "ca100"
    result.x = 3 * 26 + 0;
    result.y = 99;
    CU_ASSERT_STRING_EQUAL(write_coordinate(result, buffer), "ca100");

    // Edge Case: Buffer overflow test - ensure buffer is large enough
    result.x = 1 * 26 * 26 + 1 * 26 + 0;
    result.y = 200;
    CU_ASSERT_STRING_EQUAL(write_coordinate(result, buffer), "aaa201");

    // Edge Case: Test very high x and y (e.g., (1000, 1000)) -> expected "aaa1001"
    result.x = 26 * 26 + 26 + 0;
    result.y = 1000;
    CU_ASSERT_STRING_EQUAL(write_coordinate(result, buffer), "aaa1001");
}

void test_coordinates(void)
{
    test_read_coordinate();
    test_write_coordinate();
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

    populateTab(&tab_white);

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

    populateTab(&tab_cross);

    // Call undo command
    CU_ASSERT_EQUAL(undo_command(&cmd_cross, &tab_cross), 0);

    // Cleanup
    free(cmd_cross.tokens[1]);
    free(cmd_cross.tokens[0]);
    free(cmd_cross.tokens);
    free(tab_cross.data);
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
    // test_parseCommand_load();
    // test_parseCommand_white_without_coords();
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

void test_index(void)
{
    test_calc_index();
    test_assert_index();
}

void test_elem(void)
{
    test_get_elem();
    test_set_elem();
}

void testes_jogo(void)
{
    test_index();
    test_positions_validity();
    test_elem();
    testes_tabuleiro();
    testes_comandos();
    test_coordinates();
}
