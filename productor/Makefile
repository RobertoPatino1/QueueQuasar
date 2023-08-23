CC = gcc
CFLAGS = -Wall -Wextra

all: productor consumer

productor: productor.o productor_utils.o connection_utils.o
	$(CC) $(CFLAGS) -o productor productor.o productor_utils.o connection_utils.o

consumer: consumer.o connection_utils.o
	$(CC) $(CFLAGS) -o consumer consumer.o connection_utils.o

productor.o: productor.c productor_utils.h connection_utils.h
	$(CC) $(CFLAGS) -c productor.c

productor_utils.o: productor_utils.c productor_utils.h connection_utils.h
	$(CC) $(CFLAGS) -c productor_utils.c

connection_utils.o: connection_utils.c connection_utils.h
	$(CC) $(CFLAGS) -c connection_utils.c

consumer.o: consumer.c connection_utils.h
	$(CC) $(CFLAGS) -c consumer.c

clean:
	rm -f productor consumer *.o
