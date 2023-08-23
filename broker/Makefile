CC = gcc
CFLAGS = -Wall -Wextra -std=c99

all: main

main: main.o multiPartitionQueue.o
	$(CC) $(CFLAGS) -o $@ $^

main.o: main.c multiPartitionQueue.h
	$(CC) $(CFLAGS) -c $<

multiPartitionQueue.o: multiPartitionQueue.c multiPartitionQueue.h
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f main *.o

.PHONY: all clean