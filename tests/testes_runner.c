#include "../tests/testes_jogo.h"
#include "../tests/testes_helpers.h"
#include <CUnit/CUnit.h>
#include <CUnit/Basic.h>
#include "../include/jogo/tabuleiro.h"

void add_tests(void)
{
    CU_pSuite suite = CU_add_suite("Puzzle Game Tests", 0, 0);

    CU_add_test(suite, "Test jogo", testes_jogo);
    CU_add_test(suite, "Test helpers", testes_helpers);
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
