CC = gcc
CFLAGS = -Wall -Wextra -std=c99

all: broker

broker: broker.o connection_utils.o
	$(CC) $(CFLAGS) -o $@ $^

broker.o: broker.c connection_utils.h
	$(CC) $(CFLAGS) -c $<

connection_utils.o: connection_utils.c connection_utils.h
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f broker *.o

.PHONY: all clean
