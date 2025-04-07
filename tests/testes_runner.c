#include "testes_jogo.h"
#include "testes_helpers.h"
#include <CUnit/CUnit.h>
#include <CUnit/Basic.h>
#include "../jogo/tabuleiro.h" // Include your game logic header

void add_tests(void)
{
    CU_pSuite suite = CU_add_suite("Puzzle Game Tests", 0, 0);

    CU_add_test(suite, "Test arrays", test_arrays);
    CU_add_test(suite, "Test strings", test_strings);
    CU_add_test(suite, "Test tabuleiro state", test_tabuleiroState);
    CU_add_test(suite, "Test comandos", test_comandos);
}

int main(void)
{
    CU_initialize_registry();
    add_tests();
    CU_basic_set_mode(CU_BRM_VERBOSE);
    CU_basic_run_tests();
    CU_cleanup_registry();
    return 0;
}
