CC = gcc
CFLAGS = -Wall -Wextra

all: producer 

producer: producer.o producer_utils.o connection_utils.o
	$(CC) $(CFLAGS) -o producer producer.o producer_utils.o connection_utils.o



producer.o: producer.c producer_utils.h connection_utils.h
	$(CC) $(CFLAGS) -c producer.c

producer_utils.o: producer_utils.c producer_utils.h connection_utils.h
	$(CC) $(CFLAGS) -c producer_utils.c

connection_utils.o: connection_utils.c connection_utils.h
	$(CC) $(CFLAGS) -c connection_utils.c



clean:
	rm -f producer *.o
