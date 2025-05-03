#include "../include/jogo/comandos.h"
#include "../include/helpers/strs.h"
#include "../include/jogo/tabuleiro.h"
#include "../include/helpers/memory.h"
#include "../include/helpers/history.h"
#include <CUnit/CUnit.h>
#include <stdlib.h>

void test_isUpper(void)
{
    CU_ASSERT_FALSE(isUpper('c'));
    CU_ASSERT_NOT_EQUAL(isUpper('A'), 0);
}

void test_toUpper(void)
{
    CU_ASSERT_EQUAL(toUpper('c'), 'C');
    CU_ASSERT_EQUAL(toUpper('A'), 'A');
}

void test_isLower(void)
{
    CU_ASSERT_FALSE(isLower('A'));
    CU_ASSERT_TRUE(isLower('c'));
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

void testes_strings(void)
{
    test_isUpper();
    test_toUpper();
    test_isLower();
    test_toLower();
    test_trim_str();
}
