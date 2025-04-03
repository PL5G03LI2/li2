#include "arrays.h"
#include "tabuleiro.h"
#include <CUnit/CUnit.h>
#include <stdlib.h>

void test_positions_validity() {
    Tab tab = {NULL, 5, 5};
    
    CU_ASSERT_EQUAL(assert_pos(&tab, 2, 3), 1);
    CU_ASSERT_EQUAL(assert_pos(&tab, 0, 0), 1);
    CU_ASSERT_EQUAL(assert_pos(&tab, 4, 4), 1);

    CU_ASSERT_EQUAL(assert_pos(&tab, -1, 2), 0);
    CU_ASSERT_EQUAL(assert_pos(&tab, 5, 2), 0);
    CU_ASSERT_EQUAL(assert_pos(&tab, 2, 5), 0);
}

void test_get_positions() {
    Tab tab = {
        .data = "abcde"
                "fghij"
                "klmno"
                "pqrst"
                "uvwxy",
        .height = 5,
        .width = 5
    };

    CU_ASSERT_EQUAL(get_pos(&tab, 2, 3), 'h');  // Inside bounds
    CU_ASSERT_EQUAL(get_pos(&tab, 0, 0), 'a');  // Top-left corner
    CU_ASSERT_EQUAL(get_pos(&tab, 4, 4), 'y');  // Bottom-right corner

    CU_ASSERT_EQUAL(get_pos(&tab, -1, 2), '\0');  // Out of bounds (negative x)
    CU_ASSERT_EQUAL(get_pos(&tab, 5, 2), '\0');   // Out of bounds (x >= height)
    CU_ASSERT_EQUAL(get_pos(&tab, 2, 5), '\0');   // Out of bounds (y >= width)
}

void test_set_positions() {
    Tab tab = {"abcde", 1, 5};

    CU_ASSERT_EQUAL(tab.data[0], 'c');
    set_pos(&tab, 0, 0, 'c');
    CU_ASSERT_EQUAL(tab.data[0], 'c');
}

void test_set_branco() {
    Tab tab = {"abcde", 1, 5};

    CU_ASSERT_EQUAL(tab.data[0], 'a');
    set_branco(&tab, 0, 0);
    CU_ASSERT_EQUAL(tab.data[0], 'A');
}

// Function to redirect stdout and capture printed output
char* capture_output(void (*func)(void)) {
    // Save original stdout
    FILE *original_stdout = stdout;
    
    // Create a memory buffer to capture output
    char *buffer = (char *)malloc(1024 * sizeof(char));
    setvbuf(stdout, buffer, _IOFBF, 1024); // Buffer for capturing stdout
    
    // Call the function that prints to stdout
    func();
    
    // Restore stdout
    stdout = original_stdout;
    
    return buffer; // Return the captured output
}

void test_print_tabuleiro() {
    Tab tab = {"abcdefghij", 2, 5};

    //Not working rn, find alternative later
    //char *captured_output = capture_output(print_tabuleiro(&tab, 2, 5));
}
