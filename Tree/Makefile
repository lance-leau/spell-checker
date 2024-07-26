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


# Build target
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS) 

# Compile source files
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@ 

# Clean target
clean: 
	rm -f $(TARGET) $(OBJS)

# Declare clean as a phony target
.PHONY: clean

