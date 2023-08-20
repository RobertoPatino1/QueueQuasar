CC = gcc
CFLAGS = -Wall -Wextra

all: productor

productor: productor.c
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm -f productor

.PHONY: clean
