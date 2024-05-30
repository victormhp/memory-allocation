CC=gcc
INCDIRS=-I.
CFLAGS=-Wall -Wextra -g $(INCDIRS)

SRCS=main.c mem_allocation.c
OBJS=$(SRCS:.c=.o)

BINARY=main

all: $(BINARY)

$(BINARY): $(OBJS)
	$(CC) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $^

clean:
	rm -rf $(BINARY) $(OBJS)

.PHONY: all clean

