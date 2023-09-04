# Compiler and flags
CC = gcc
CFLAGS = -Wall -pthread

# Executable name
CONSUMIDOR_EXEC = consumidor

all: $(CONSUMIDOR_EXEC)

$(CONSUMIDOR_EXEC): consumidor.c connection_utils.c
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm -f $(CONSUMIDOR_EXEC)

.PHONY: all clean
