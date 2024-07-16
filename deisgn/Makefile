CC = gcc
CFLAGS = `pkg-config --cflags gtk+-3.0`
LDFLAGS = `pkg-config --libs gtk+-3.0`
SRC = design.c ../Tree/tree.c ../word_prediction/hash_map.c ../levenshtein/levenshtein.c ../metaphone/metaphone.c
OBJ = $(SRC:.c=.o)
TARGET = gtk_example

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) -o $@ $^ $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGET)

