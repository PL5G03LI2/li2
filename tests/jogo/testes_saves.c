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

void test_tabuleiroState_SL(void)
{
    Game game;

    init_game(&game);

    CU_ASSERT_FALSE(carregar_jogo(&game, "j1.txt"));
    CU_ASSERT_TRUE(carregar_jogo(&game, "j2.txt"));
    CU_ASSERT_FALSE(salvar_jogo(&game, "j3.txt"));

    // CU_ASSERT_TRUE(validar_tabuleiro(game.tabuleiro));

    free_game(&game);
}

void testes_saves(void) {
    test_tabuleiroState_SL();
}
