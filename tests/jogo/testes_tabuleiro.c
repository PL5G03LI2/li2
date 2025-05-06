#include <CUnit/CUnit.h>
#include <stdlib.h>
#include <ncurses.h>

#include "helpers/history.h"
#include "helpers/memory.h"
#include "helpers/coords.h"
#include "helpers/strs.h"

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
        tab->data[i].marked = false;
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

void test_check_row(void)
{
    Tab *tab = malloc(sizeof(Tab));
    tab->height = 1;
    tab->width = 5;
    tab->data = malloc(sizeof(Piece) * tab->height * tab->width);
    iVec2 vec = {0, 0};
    tab->sel_piece = vec;
    populateTab(tab);
    tab->data[1].c = 'a';
    bool violated[tab->height * tab->width];

    tab->data[0].c = 'A';
    check_row(tab, tab->data, 0, violated);
    CU_ASSERT_EQUAL(violated[0], false);
    CU_ASSERT_EQUAL(violated[1], false);
    tab->data[1].c = 'A';
    check_row(tab, tab->data, 0, violated);
    CU_ASSERT_EQUAL(violated[0], true);
    CU_ASSERT_EQUAL(violated[1], true);

    free(tab->data);
    free(tab);
}

void test_check_column(void)
{
    Tab *tab = malloc(sizeof(Tab));
    tab->height = 5;
    tab->width = 1;
    tab->data = malloc(sizeof(Piece) * tab->height * tab->width);
    iVec2 vec = {0, 0};
    tab->sel_piece = vec;
    populateTab(tab);
    tab->data[1].c = 'a';
    bool violated[tab->height * tab->width];
    for (int i = 0; i < tab->height * tab->width; i++)
    {
        violated[i] = false;
    }

    tab->data[0].c = 'A';
    check_column(tab, tab->data, 0, violated);
    CU_ASSERT_EQUAL(violated[0], false);
    CU_ASSERT_EQUAL(violated[1], false);
    tab->data[1].c = 'A';
    check_column(tab, tab->data, 0, violated);
    CU_ASSERT_EQUAL(violated[0], true);
    CU_ASSERT_EQUAL(violated[1], true);

    free(tab->data);
    free(tab);
}

void test_check_marked(void)
{
    Tab *tab = malloc(sizeof(Tab));
    tab->height = 1;
    tab->width = 5;
    tab->data = malloc(sizeof(Piece) * tab->height * tab->width);
    iVec2 vec = {0, 0};
    tab->sel_piece = vec;
    populateTab(tab);
    bool violated[tab->height * tab->width];
    for (int i = 0; i < tab->height * tab->width; i++)
    {
        violated[i] = false;
    }

    tab->data[4].c = 'E';
    check_marked(tab, 3, violated);
    CU_ASSERT_EQUAL(violated[4], false);
    CU_ASSERT_EQUAL(violated[2], true);

    free(tab->data);
    free(tab);
}

void test_floodfill(void)
{
    Tab *tab = malloc(sizeof(Tab));
    tab->height = 1;
    tab->width = 5;
    tab->data = malloc(sizeof(Piece) * tab->height * tab->width);
    iVec2 vec = {0, 0};
    tab->sel_piece = vec;
    populateTab(tab);
    bool visited[tab->height * tab->width];
    int white_pieces = 0;

    floodfill(tab, tab->sel_piece, visited, &white_pieces);
    CU_ASSERT_EQUAL(visited[4], false);
    CU_ASSERT_EQUAL(white_pieces, 0);
    tab->data[0].c = 'A';
    floodfill(tab, tab->sel_piece, visited, &white_pieces);
    CU_ASSERT_EQUAL(white_pieces, 0);

    free(tab->data);
    free(tab);
}

void test_check_paths(void)
{
    Tab *tab = malloc(sizeof(Tab));
    tab->height = 1;
    tab->width = 5;
    tab->data = malloc(sizeof(Piece) * tab->height * tab->width);
    iVec2 vec = {0, 0};
    tab->sel_piece = vec;
    populateTab(tab);
    bool violated[tab->height * tab->width];
    for (int i = 0; i < tab->height * tab->width; i++)
    {
        violated[i] = false;
    }

    check_paths(tab, tab->data, 0, violated);
    CU_ASSERT_EQUAL(violated[2], false);
    tab->data[0].c = 'A';
    tab->data[2].c = 'C';
    check_paths(tab, tab->data, 0, violated);
    CU_ASSERT_EQUAL(violated[2], true);

    free(tab->data);
    free(tab);
}

void test_validarTab(void)
{
    Game game;

    init_game(&game);

    CU_ASSERT_TRUE(validar_tabuleiro(game.tabuleiro));

    free_game(&game);
}

void testes_tabuleiro(void)
{
    test_elem();
    test_toggle_branco();
    test_toggle_marked();
    test_check_row();
    test_check_column();
    test_check_marked();
    test_floodfill();
    test_check_paths();
    test_validarTab();
}
