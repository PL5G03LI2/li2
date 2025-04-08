#include "arrays.h"
#include "comandos.h"
#include "strs.h"
#include "tabuleiro.h"
#include <CUnit/CUnit.h>
#include <stdlib.h>

void test_toUpper(void)
{
    CU_ASSERT_EQUAL(toUpper('c'), 'C');
    CU_ASSERT_EQUAL(toUpper('A'), 'A'); // Uppercase remains unchanged
}

void test_strings(void)
{
    test_toUpper();
}

void test_tabuleiroState(void)
{
    Tab tab;
    tab.height = 1;
    tab.width = 3;
    tab.data = malloc(sizeof(Piece) * tab.height * tab.width);

    CU_ASSERT(carregar_tabuleiro(&tab, "j1.txt"));
    CU_ASSERT(carregar_tabuleiro(&tab, "j2.txt"));
    CU_ASSERT(salvar_tabuleiro(&tab, "j3.txt"));
    CU_ASSERT(validar_tabuleiro(&tab));
}

void test_comandos(void)
{
    test_tabuleiroState();
}

void test_push_single(void)
{
    TabHistory *history = NULL;
    Tab tab;
    tab.height = 1;
    tab.width = 3;
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
    
    history = push(history, tab);

    CU_ASSERT_PTR_NOT_NULL(history);
    CU_ASSERT_EQUAL(history->tab.data[0].c, 'a');
    CU_ASSERT_EQUAL(history->tab.data[1].c, 'b');
    CU_ASSERT_EQUAL(history->tab.data[2].c, 'c');
    CU_ASSERT_EQUAL(history->tab.height, 1);
    CU_ASSERT_EQUAL(history->tab.width, 3);
    CU_ASSERT_PTR_NULL(history->next);

    free(history);
    free(tab.data);
}

void test_push_multiple(void)
{
    TabHistory *history = NULL;
    Tab tab1;
    Tab tab2;
    tab1.height = 1;
    tab1.width = 3;
    tab2.height = 2;
    tab2.width = 3;
    tab1.data = malloc(sizeof(Piece) * tab1.height * tab1.width);
    tab2.data = malloc(sizeof(Piece) * tab2.height * tab2.width);

    int i, j;
    for(i = 0; i < tab1.height; i++) {
        for(j = 0; j < tab1.width; j++) {
            char letra = (i*tab1.width) + j + 97;
            if (letra > 122) {
                letra = '0';
            }
            tab1.data[(i*tab1.width) + j].c = letra;
        }
    }
    for(i = 0; i < tab2.height; i++) {
        for(j = 0; j < tab2.width; j++) {
            char letra = (i*tab2.width) + j + 97;
            if (letra > 122) {
                letra = '0';
            }
            tab2.data[(i*tab2.width) + j].c = letra;
        }
    }

    history = push(history, tab1);
    history = push(history, tab2);

    CU_ASSERT_PTR_NOT_NULL(history);
    CU_ASSERT_EQUAL(history->tab.data[0].c, 'a');
    CU_ASSERT_EQUAL(history->tab.data[1].c, 'b');
    CU_ASSERT_EQUAL(history->tab.data[2].c, 'c');
    
    CU_ASSERT_PTR_NOT_NULL(history->next);
    CU_ASSERT_EQUAL(history->next->tab.data[3].c, 'd');
    CU_ASSERT_EQUAL(history->next->tab.data[4].c, 'e');
    CU_ASSERT_EQUAL(history->next->tab.data[5].c, 'f');

    CU_ASSERT_PTR_NULL(history->next->next);


    CU_ASSERT_PTR_NULL(history->next->next);

    free(history->next);
    free(history);
    free(tab1.data);
    free(tab2.data);
}

void test_pop_empty(void)
{
    TabHistory *history = NULL;
    Tab popped = pop(&history);

    CU_ASSERT_PTR_NULL(popped.data);
    CU_ASSERT_PTR_NULL(history); // List should remain empty
}

void test_pop_single(void)
{
    TabHistory *history = malloc(sizeof(TabHistory));
    history->tab.data = malloc(sizeof(Piece) * 3);
    history->tab.data[0].c = 'a';
    history->tab.data[1].c = 'b';
    history->tab.data[2].c = 'c';
    history->tab.height = 1;
    history->tab.width = 3;
    history->next = NULL;

    Tab popped = pop(&history);

    CU_ASSERT_EQUAL(popped.data[0].c, 'a'); // Ensure popped.data is correctly populated
    CU_ASSERT_EQUAL(popped.data[1].c, 'b');
    CU_ASSERT_EQUAL(popped.data[2].c, 'c');
    CU_ASSERT_EQUAL(popped.height, 1);
    CU_ASSERT_EQUAL(popped.width, 3);
    CU_ASSERT_PTR_NULL(history); // List should be empty after pop

    free(popped.data); // Free the memory that was allocated for popped data
    free(history); // Free the history pointer, which was previously set to NULL in pop
}

void test_pop_multi(void)
{
    // Create first node
    TabHistory *history = malloc(sizeof(TabHistory));
    history->tab.data = malloc(sizeof(Piece) * 5);
    history->tab.data[0].c = 'f';
    history->tab.data[1].c = 'i';
    history->tab.data[2].c = 'r';
    history->tab.data[3].c = 's';
    history->tab.data[4].c = 't';
    history->tab.height = 1;
    history->tab.width = 5;
    history->next = NULL;

    Tab tab;
    tab.height = 1;
    tab.width = 6;
    tab.data = malloc(sizeof(Piece) * tab.height * tab.width);

    tab.data[0].c = 's';
    tab.data[1].c = 'e';
    tab.data[2].c = 'c';
    tab.data[3].c = 'o';
    tab.data[4].c = 'n';
    tab.data[5].c = 'd';

    // Create second node
    history = push(history, tab);

    // Pop last element
    Tab popped = pop(&history);

    CU_ASSERT_EQUAL(popped.data[0].c, 's');
    CU_ASSERT_EQUAL(popped.data[1].c, 'e');
    CU_ASSERT_EQUAL(popped.data[2].c, 'c');
    CU_ASSERT_EQUAL(popped.data[3].c, 'o');
    CU_ASSERT_EQUAL(popped.data[4].c, 'n');
    CU_ASSERT_EQUAL(popped.data[5].c, 'd');
    CU_ASSERT_EQUAL(popped.height, 1);
    CU_ASSERT_EQUAL(popped.width, 6);
    CU_ASSERT_PTR_NOT_NULL(history); // List should still contain "first"
    CU_ASSERT_EQUAL(history->tab.data[0].c, 'f');
    CU_ASSERT_EQUAL(history->tab.data[1].c, 'i');
    CU_ASSERT_EQUAL(history->tab.data[2].c, 'r');
    CU_ASSERT_EQUAL(history->tab.data[3].c, 's');
    CU_ASSERT_EQUAL(history->tab.data[4].c, 't');

    // Only free the 'tab.data' pointers as required
    free(tab.data);      // Free the allocated memory for "second"
    free(popped.data);   // Free the memory that was popped
    free(history->tab.data);  // Free the remaining "first" node's data
    free(history);       // Free the remaining node ("first")
}

void test_get_hist_elem_empty(void)
{
    TabHistory *history = NULL;
    Tab *result = get_hist_elem(&history, 0);

    CU_ASSERT_PTR_NULL(result);
}

void test_get_hist_elem_negativeIndex(void)
{
    TabHistory *history = malloc(sizeof(TabHistory));
    history->tab.data = malloc(sizeof(Piece) * 4);
    history->tab.data[0].c = 't';
    history->tab.data[1].c = 'e';
    history->tab.data[2].c = 's';
    history->tab.data[3].c = 't';
    history->tab.height = 5;
    history->tab.width = 5;
    history->next = NULL;

    Tab *result = get_hist_elem(&history, -1);

    CU_ASSERT_PTR_NULL(result);

    free(history->tab.data);
    free(history);
}

void test_get_hist_elem_first(void)
{
    TabHistory *history = malloc(sizeof(TabHistory));
    history->tab.data = malloc(sizeof(Piece) * 4);
    history->tab.data[0].c = 't';
    history->tab.data[1].c = 'e';
    history->tab.data[2].c = 's';
    history->tab.data[3].c = 't';
    history->tab.height = 1;
    history->tab.width = 4;
    history->next = NULL;

    Tab *result = get_hist_elem(&history, 0);

    CU_ASSERT_PTR_NOT_NULL(result);
    CU_ASSERT_EQUAL(result->data[0].c, 't');
    CU_ASSERT_EQUAL(result->data[1].c, 'e');
    CU_ASSERT_EQUAL(result->data[2].c, 's');
    CU_ASSERT_EQUAL(result->data[3].c, 't');
    CU_ASSERT_EQUAL(result->height, 1);
    CU_ASSERT_EQUAL(result->width, 4);

    free(history->tab.data);
    free(history);
}

void test_get_hist_elem_last(void)
{
    TabHistory *history = malloc(sizeof(TabHistory));
    history->tab.data = malloc(sizeof(Piece) * 5);
    history->tab.data[0].c = 'f';
    history->tab.data[1].c = 'i';
    history->tab.data[2].c = 'r';
    history->tab.data[3].c = 's';
    history->tab.data[4].c = 't';
    history->tab.height = 1;
    history->tab.width = 5;
    history->next = NULL;

    Tab tab;
    tab.height = 1;
    tab.width = 6;
    tab.data = malloc(sizeof(Piece) * tab.height * tab.width);

    tab.data[0].c = 's';
    tab.data[1].c = 'e';
    tab.data[2].c = 'c';
    tab.data[3].c = 'o';
    tab.data[4].c = 'n';
    tab.data[5].c = 'd';
    

    history = push(history, tab);

    Tab *result = get_hist_elem(&history, 1);

    CU_ASSERT_PTR_NOT_NULL(result);
    CU_ASSERT_EQUAL(result->data[0].c, 's');
    CU_ASSERT_EQUAL(result->data[1].c, 'e');
    CU_ASSERT_EQUAL(result->data[2].c, 'c');
    CU_ASSERT_EQUAL(result->data[3].c, 'o');
    CU_ASSERT_EQUAL(result->data[4].c, 'n');
    CU_ASSERT_EQUAL(result->data[5].c, 'd');
    CU_ASSERT_EQUAL(result->height, 1);
    CU_ASSERT_EQUAL(result->width, 6);

    free(tab.data);
    free(history->next->tab.data);
    free(history->next);
    free(history->tab.data);
    free(history);
}

void test_get_hist_elem_OOB(void)
{
    TabHistory *history = malloc(sizeof(TabHistory));
    history->tab.data = malloc(sizeof(Piece) * 4);
    history->tab.data[0].c = 'o';
    history->tab.data[1].c = 'n';
    history->tab.data[2].c = 'l';
    history->tab.data[3].c = 'y';
    history->tab.height = 2;
    history->tab.width = 2;
    history->next = NULL;

    Tab *result = get_hist_elem(&history, 5); // Out of bounds

    CU_ASSERT_PTR_NULL(result);

    free(history->tab.data);
    free(history);
}

void test_destroy_empty(void)
{
    TabHistory *history = NULL;
    destroy(&history);

    CU_ASSERT_PTR_NULL(history); // The head should remain NULL
}

void test_destroy_single(void)
{
    TabHistory *history = malloc(sizeof(TabHistory));
    history->tab.data = malloc(sizeof(Piece) * 6);
    history->tab.data[0].c = 's';
    history->tab.data[1].c = 'i';
    history->tab.data[2].c = 'n';
    history->tab.data[3].c = 'g';
    history->tab.data[4].c = 'l';
    history->tab.data[5].c = 'e';
    history->tab.height = 1;
    history->tab.width = 6;
    history->next = NULL;

    destroy(&history);
    CU_ASSERT_PTR_NULL(history); // The head should be NULL after destruction
}

void test_destroy_multi(void)
{
    TabHistory *history = malloc(sizeof(TabHistory));
    history->tab.data = malloc(sizeof(Piece) * 5);
    history->tab.data[0].c = 'f';
    history->tab.data[1].c = 'i';
    history->tab.data[2].c = 'r';
    history->tab.data[3].c = 's';
    history->tab.data[4].c = 't';
    history->tab.height = 1;
    history->tab.width = 5;
    history->next = malloc(sizeof(TabHistory));

    history->next->tab.data = malloc(sizeof(Piece) * 6);  // Allocate for the next node's tab data
    history->next->tab.data[0].c = 's';
    history->next->tab.data[1].c = 'e';
    history->next->tab.data[2].c = 'c';
    history->next->tab.data[3].c = 'o';
    history->next->tab.data[4].c = 'n';
    history->next->tab.data[5].c = 'd';
    history->next->tab.height = 1;
    history->next->tab.width = 6;
    history->next->next = NULL;

    // Call destroy to free the entire list and its data
    destroy(&history);

    // After destruction, history should be NULL
    CU_ASSERT_PTR_NULL(history); // The head should be NULL after destruction
}

void test_push(void)
{
    test_push_single();
    test_push_multiple();
}

void test_pop_tabuleiro(void)
{
    test_pop_empty();
    test_pop_single();
    test_pop_multi();
}

void test_get_hist_elem(void)
{
    test_get_hist_elem_empty();
    test_get_hist_elem_negativeIndex();
    test_get_hist_elem_first();
    test_get_hist_elem_last();
    test_get_hist_elem_OOB();
}

void test_destroy(void)
{
    test_destroy_empty();
    test_destroy_single();
    test_destroy_multi();
}

void test_arrays(void)
{
    test_push();
    test_pop_tabuleiro();
    test_get_hist_elem();
    test_destroy();
}
