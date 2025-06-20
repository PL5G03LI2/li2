#include "../tests/helpers/testes_coords.h"
#include "../tests/helpers/testes_history.h"
#include "../tests/helpers/testes_memory.h"
#include "../tests/helpers/testes_strs.h"
#include "../tests/jogo/testes_comandos.h"
#include "../tests/jogo/testes_saves.h"
#include "../tests/jogo/testes_tabuleiro.h"
#include <CUnit/CUnit.h>
#include <CUnit/Basic.h>
#include "../include/jogo/tabuleiro.h" // Include your game logic header

void add_tests(void)
{
    CU_pSuite suite = CU_add_suite("Game Tests", 0, 0);

    CU_add_test(suite, "Test coords", testes_coords);
    CU_add_test(suite, "Test history", testes_history);
    CU_add_test(suite, "Test memory", testes_memory);
    CU_add_test(suite, "Test strings", testes_strings);

    CU_add_test(suite, "Test comandos", testes_comandos);
    CU_add_test(suite, "Test saves", testes_saves);
    CU_add_test(suite, "Test tabuleiro", testes_tabuleiro);
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
