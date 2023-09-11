CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -g  
LDFLAGS = -pthread

all: broker

broker: broker.o connection_utils.o multiPartitionQueue.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^

broker.o: broker.c connection_utils.h multiPartitionQueue.h
	$(CC) $(CFLAGS) -c $<

connection_utils.o: connection_utils.c connection_utils.h multiPartitionQueue.h
	$(CC) $(CFLAGS) -c $<

multiPartitionQueue.o: multiPartitionQueue.c multiPartitionQueue.h
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f broker *.o persistence.txt

.PHONY: all clean
