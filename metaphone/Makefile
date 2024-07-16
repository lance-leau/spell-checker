CC = gcc
CFLAGS = -Wall -Wextra -I../levenshtein

# Directories
LEVENSHTEIN_DIR = ../levenshtein

# Source files
SOURCES = metaphone.c $(LEVENSHTEIN_DIR)/levenshtein.c

# Object files
OBJECTS = $(SOURCES:.c=.o)

# Executable
TARGET = metaphone

# Default target
all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f $(OBJECTS) $(TARGET)

.PHONY: all clean

