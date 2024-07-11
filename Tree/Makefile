CC = gcc

CFLAGS = -Wall -Werror -g

TARGET = tree

SRCS = tree.c

OBJS = $(SRCS:.c=.o)

$(TARGET): ($OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

%.o %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean: 
	rm -f $(TARGET) $(OBJS)

.PHONY: clean
