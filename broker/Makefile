CC = gcc
CFLAGS = -Wall -Wextra -std=c99
LDFLAGS = -pthread

all: broker

broker: broker.o connection_utils.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^

broker.o: broker.c connection_utils.h
	$(CC) $(CFLAGS) -c $<

connection_utils.o: connection_utils.c connection_utils.h
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f broker *.o

.PHONY: all clean
