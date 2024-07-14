# Compiler
CC = gcc

# Compiler flags
CFLAGS = -Wall -Werror -g

# Target executable
TARGET = tree

# Source files
SRCS = tree.c

# Object files
OBJS = $(SRCS:.c=.o)

# Paths to json-c library and include directory
JSONC_LIBS = -ljson-c
JSONC_INCLUDE = /usr/include/json-c   # Adjust this path if necessary

# Build target
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS) $(JSONC_LIBS)

# Compile source files
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@ -I$(JSONC_INCLUDE)

# Clean target
clean: 
	rm -f $(TARGET) $(OBJS)

# Declare clean as a phony target
.PHONY: clean

