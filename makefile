# Compiler and flags
CC       = gcc
CFLAGS   = -Wall -Wextra -pedantic -O1 -fsanitize=address -fno-omit-frame-pointer -g
COVFLAGS = -fprofile-arcs -ftest-coverage
INC_DIRS = -Iinclude -I/opt/homebrew/Cellar/cunit/2.1-3/include
LDFLAGS  = -L/opt/homebrew/Cellar/cunit/2.1-3/lib
LIBS     = -lcunit -lncurses

# Directories
SRC_DIR   = src
TESTS_DIR = tests
OBJ_DIR   = obj
BIN_DIR   = out

VPATH = $(SRC_DIR) $(TESTS_DIR)

# Source and object files
SRCS      := $(shell find $(SRC_DIR) -name '*.c')
TEST_SRCS := $(shell find $(TESTS_DIR) -name '*.c')
OBJS      := $(patsubst $(SRC_DIR)/%.c,   $(OBJ_DIR)/%.o, $(SRCS))
TEST_OBJS := $(patsubst $(TESTS_DIR)/%.c, $(OBJ_DIR)/%.o, $(TEST_SRCS))
JOGO_OBJS := $(filter-out $(OBJ_DIR)/main.o, $(OBJS))
MAIN_OBJ  := $(OBJ_DIR)/main.o

# Executables
JOGO_EXEC = $(BIN_DIR)/jogo
TEST_EXEC = $(BIN_DIR)/testar

.PHONY: all jogo testar clean

all: $(JOGO_EXEC) $(TEST_EXEC)

jogo: $(JOGO_EXEC)
testar: $(TEST_EXEC)

# Output directories
$(OBJ_DIR) $(BIN_DIR):
	mkdir -p $@

# Pattern rules for object files
$(OBJ_DIR)/%.o: %.c | $(OBJ_DIR)
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(COVFLAGS) $(INC_DIRS) -c $< -o $@

# Link targets
$(JOGO_EXEC): $(JOGO_OBJS) $(MAIN_OBJ) | $(BIN_DIR)
	$(CC) $(CFLAGS) $(COVFLAGS) -o $@ $(JOGO_OBJS) $(MAIN_OBJ) $(LDFLAGS) $(LIBS)

$(TEST_EXEC): $(TEST_OBJS) $(JOGO_OBJS) | $(BIN_DIR)
	$(CC) $(CFLAGS) $(COVFLAGS) -o $@ $(TEST_OBJS) $(JOGO_OBJS) $(LDFLAGS) $(LIBS)

clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR)
	find . -name "*.gcda" -delete
	find . -name "*.gcno" -delete
	find . -name "*.gcov" -delete
