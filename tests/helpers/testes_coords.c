#include <CUnit/CUnit.h>
#include <stdlib.h>
#include <ncurses.h>

#include "helpers/history.h"
#include "helpers/memory.h"
#include "helpers/coords.h"

#include "jogo/comandos.h"
#include "jogo/tabuleiro.h"
#include "jogo/saves.h"

#include "types.h"

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
    test_calc_index_valid_position();
    test_calc_index_invalid_position();
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
    test_assert_index_valid();
    test_assert_index_invalid();
}

void test_index(void)
{
    test_calc_index();
    test_assert_index();
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

void testes_coords(void) {
    test_index();
    test_coordinates();
}
