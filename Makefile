# Compiler
CC = gcc

# Compiler flags
CFLAGS = -Wall -I/usr/include/json-c

# Linker flags
LDFLAGS = -ljson-c

# Source files
SRCS = main.c \
       Tree/tree.c \
       word_prediction/hash_map.c \
       levenshtein/levenshtein.c \
       metaphone/metaphone.c

# Object files
OBJS = $(SRCS:.c=.o)

# Executable
TARGET = main

# Default target
all: $(TARGET)

# Link the executable
$(TARGET): $(OBJS)
	$(CC) -o $(TARGET) $(OBJS) $(LDFLAGS)

# Compile source files to object files
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Clean target to remove generated files
clean:
	rm -f $(OBJS) $(TARGET)

# Phony targets
.PHONY: all clean
