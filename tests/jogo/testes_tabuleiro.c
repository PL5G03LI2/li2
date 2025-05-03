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

void populateTab(Tab *tab)
{
    if (!tab->data || tab->height < 1 || tab->width < 1)
        return;

    for (int i = 0; i < tab->height * tab->width; i++)
    {
        tab->data[i].c = i % 26 + 'a';
    }
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

void test_elem(void)
{
    test_get_elem();
    test_set_elem();
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

void test_validarTab(void) {
    Game game;

    init_game(&game);

    CU_ASSERT_TRUE(validar_tabuleiro(game.tabuleiro));

    free_game(&game);
}

void testes_tabuleiro(void) {
    test_elem();
    test_toggle_branco();
    test_toggle_marked();
    test_validarTab();
}
