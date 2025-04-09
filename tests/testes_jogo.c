#include "arrays.h"
#include "tabuleiro.h"
#include <CUnit/CUnit.h>
#include <stdlib.h>

void test_calc_index_valid_position(void)
{
    Tab tab;
    tab.height = 5;
    tab.width = 5;
    tab.data = malloc(sizeof(Piece) * tab.height * tab.width);

    int index = calc_index(&tab, 2, 3); // valid position
    CU_ASSERT_EQUAL(index, 3 * tab.height + 2);

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
    for (int i = 0; i <= 16; i++)
    {
        CU_ASSERT_EQUAL(assert_index(&tab, i), 1);
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
    CU_ASSERT_EQUAL(assert_index(&tab, -1), 0);
    CU_ASSERT_EQUAL(assert_index(&tab, 17), 0);
    CU_ASSERT_EQUAL(assert_index(&tab, 100), 0);

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

    CU_ASSERT_EQUAL(assert_pos(&tab, 2, 3), 1);
    CU_ASSERT_EQUAL(assert_pos(&tab, 0, 0), 1);
    CU_ASSERT_EQUAL(assert_pos(&tab, 4, 4), 1);

    CU_ASSERT_EQUAL(assert_pos(&tab, -1, 2), 0);
    CU_ASSERT_EQUAL(assert_pos(&tab, 5, 2), 0);
    CU_ASSERT_EQUAL(assert_pos(&tab, 2, 5), 0);
}

void test_get_elem(void)
{
    //.data = "abcde"
    //        "fghij"
    //        "klmno"
    //        "pqrst"
    //        "uvwxy",

    Tab tab;
    tab.height = 5;
    tab.width = 5;
    tab.data = malloc(sizeof(Piece) * tab.height * tab.width);

    int i, j;
    for(i = 0; i < tab.height; i++) {
        for(j = 0; j < tab.width; j++) {
            char letra = (i*tab.width) + j + 97;
            if (letra > 122) {
                letra = '0';
            }
            tab.data[(i*tab.width) + j].c = letra;
        }
    }

    CU_ASSERT_EQUAL(get_elem(&tab, 1, 2), 'h'); // Inside bounds
    CU_ASSERT_EQUAL(get_elem(&tab, 0, 0), 'a'); // Top-left corner
    CU_ASSERT_EQUAL(get_elem(&tab, 4, 4), 'y'); // Bottom-right corner

    CU_ASSERT_EQUAL(get_elem(&tab, -1, 2), '\0'); // Out of bounds (negative x)
    CU_ASSERT_EQUAL(get_elem(&tab, 5, 2), '\0');  // Out of bounds (x >= height)
    CU_ASSERT_EQUAL(get_elem(&tab, 2, 5), '\0');  // Out of bounds (y >= width)

    free(tab.data);
}

void test_set_elem(void)
{
    Tab tab;
    tab.height = 5;
    tab.width = 5;
    tab.data = malloc(sizeof(Piece) * tab.height * tab.width);

    int i, j;
    for(i = 0; i < tab.height; i++) {
        for(j = 0; j < tab.width; j++) {
            char letra = (i*tab.width) + j + 97;
            if (letra > 122) {
                letra = '0';
            }
            tab.data[(i*tab.width) + j].c = letra;
        }
    }

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

    int i, j;
    for(i = 0; i < tab.height; i++) {
        for(j = 0; j < tab.width; j++) {
            char letra = (i*tab.width) + j + 97;
            if (letra > 122) {
                letra = '0';
            }
            tab.data[(i*tab.width) + j].c = letra;
        }
    }

    CU_ASSERT_EQUAL(tab.data[0].c, 'a');
    toggle_branco(&tab, 0, 0);
    CU_ASSERT_EQUAL(tab.data[0].c, 'A');

    free(tab.data);
}

void test_print_tabuleiro(void) {
    Tab tab;
    tab.height = 5;
    tab.width = 5;
    tab.data = malloc(sizeof(Piece) * tab.height * tab.width);

    int i, j;
    for(i = 0; i < tab.height; i++) {
        for(j = 0; j < tab.width; j++) {
            char letra = (i*tab.width) + j + 97;
            if (letra > 122) {
                letra = '0';
            }
            tab.data[(i*tab.width) + j].c = letra;
        }
    }

    // Prepare a buffer to capture the output
    char buffer[100];  // Make sure this is large enough to hold the output
    memset(buffer, 0, sizeof(buffer));  // Clear the buffer

    // Call the modified print_tabuleiro function
    print_tab(&tab);

    // Expected output
    const char *expected_output = "abcde\n"
                                  "fghij\n"
                                  "klmno\n"
                                  "pqrst\n"
                                  "uvwxy\n";

    // Assert: Compare the captured output with the expected output
    CU_ASSERT_STRING_EQUAL(buffer, expected_output);

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
    for (int i = 0; i < tab.height * tab.width; i++) {
        tab.data[i].c = c++;
        tab.data[i].marked = 0;
    }

    // Aplica toggle_marked no centro (1, 1)
    toggle_marked(&tab, 1, 1);

    int center_idx = calc_index(&tab, 1, 1);
    CU_ASSERT_EQUAL(tab.data[center_idx].marked, 1);

    // Como center foi marcado, o char deveria virar '#'
    CU_ASSERT_EQUAL(tab.data[center_idx].c, '#');

    // Verifica os vizinhos ortogonais
    iVec2 neighbors[] = {
        {0, 1}, // esquerda
        {2, 1}, // direita
        {1, 0}, // cima
        {1, 2}  // baixo
    };

    for (int i = 0; i < 4; i++) {
        int idx = calc_index(&tab, neighbors[i].x, neighbors[i].y);
        char ch = tab.data[idx].c;
        CU_ASSERT(ch >= 'A' && ch <= 'Z'); // letra maiúscula
    }

    free(tab.data);
}

void testes_jogo(void) {
    test_calc_index();
    test_assert_index();
    test_positions_validity();
    test_get_elem();
    test_set_elem();
    test_toggle_branco();
    test_print_tabuleiro();
    test_toggle_marked();
}
