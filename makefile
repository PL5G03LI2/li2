# Compiler
CC = gcc

# Compiler flags
CFLAGS = -Wall -Wextra -pedantic -O1 -fsanitize=address -fno-omit-frame-pointer -g

# Source files
SRCS = $(wildcard *.c)

# Header files
HDRS = $(wildcard *.h)

# Output directories
OBJ_DIR = obj
BIN_DIR = out

# Object files
OBJS = $(addprefix $(OBJ_DIR)/, $(SRCS:.c=.o))

# Executable name
EXEC = main

# Default target
all: $(BIN_DIR)/$(EXEC)

# Create output directories if they don't exist
$(OBJ_DIR) $(BIN_DIR):
	mkdir -p $@

# Link object files to create the executable
$(BIN_DIR)/$(EXEC): $(OBJS) | $(BIN_DIR)
	$(CC) $(CFLAGS) -o $@ $(OBJS)

# Compile source files into object files
$(OBJ_DIR)/%.o: %.c $(HDRS) | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Clean object files and executable
clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR)

# Phony targets
.PHONY: all clean