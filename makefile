# Compiler
CC = gcc

# Compiler flags (normal compilation)
CFLAGS = -Wall -Wextra -pedantic -O1 -fsanitize=address -fno-omit-frame-pointer -g

# Compiler flags (for coverage)
COV_FLAGS = -fprofile-arcs -ftest-coverage

# Include directories for helpers, jogo, and CUnit headers
INC_DIRS = -Ihelpers -Ijogo -I/opt/homebrew/Cellar/cunit/2.1-3/include  # CUnit's header location

# Linker flags for CUnit library
LDFLAGS = -L/opt/homebrew/Cellar/cunit/2.1-3/lib  # CUnit's library location
LIBS = -lcunit

# Output directories
OBJ_DIR = obj
BIN_DIR = out

# Source directories
HELPERS_DIR = helpers
JOGO_DIR = jogo
TESTS_DIR = tests

# Find all source files
MAIN_SRC = main.c
JOGO_SRCS = $(wildcard $(HELPERS_DIR)/*.c $(JOGO_DIR)/*.c)
TEST_SRCS = $(wildcard $(TESTS_DIR)/*.c)

# Object files
MAIN_OBJ = $(patsubst %.c, $(OBJ_DIR)/%.o, $(MAIN_SRC))
JOGO_OBJS = $(patsubst %.c, $(OBJ_DIR)/%.o, $(JOGO_SRCS))
TEST_OBJS = $(patsubst %.c, $(OBJ_DIR)/%.o, $(TEST_SRCS))

# Executable names
JOGO_EXEC = $(BIN_DIR)/jogo
TEST_EXEC = $(BIN_DIR)/testar

# Default target (compiles both executables)
all: $(JOGO_EXEC) $(TEST_EXEC)

# Optional: build just the game
jogo: $(JOGO_EXEC)

# Optional: build just the tests
testar: $(TEST_EXEC)

# Create output directories
$(OBJ_DIR) $(BIN_DIR):
	mkdir -p $@

# Build jogo executable (normal compilation with coverage)
$(JOGO_EXEC): $(JOGO_OBJS) $(MAIN_OBJ) | $(BIN_DIR)
	$(CC) $(CFLAGS) $(COV_FLAGS) -o $@ $(JOGO_OBJS) $(MAIN_OBJ)

# Build testar executable (with coverage enabled)
$(TEST_EXEC): $(TEST_OBJS) $(JOGO_OBJS) | $(BIN_DIR)  # Include JOGO_OBJS here
	$(CC) $(CFLAGS) $(COV_FLAGS) -o $@ $(TEST_OBJS) $(JOGO_OBJS) $(LDFLAGS) $(LIBS)  # Add LDFLAGS and LIBS here

# Compile source files into object files
$(OBJ_DIR)/%.o: %.c | $(OBJ_DIR)
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(COV_FLAGS) $(INC_DIRS) -c $< -o $@

clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR)
	find . -name "*.gcda" -delete
	find . -name "*.gcno" -delete
	find . -name "*.gcov" -delete
