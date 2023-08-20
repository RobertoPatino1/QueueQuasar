CC = gcc
CFLAGS = -Wall -Wextra

productor: productor.o productor_utils.o
	$(CC) $(CFLAGS) -o productor productor.o productor_utils.o

productor.o: productor.c productor_utils.h
	$(CC) $(CFLAGS) -c productor.c

productor_utils.o: productor_utils.c productor_utils.h
	$(CC) $(CFLAGS) -c productor_utils.c

clean:
	rm -f productor *.o
