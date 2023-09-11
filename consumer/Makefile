# Compiler and flags
CC = gcc
CFLAGS = -Wall -pthread

# Executable name
CONSUMER_EXEC = consumer

all: $(CONSUMER_EXEC)

$(CONSUMER_EXEC): consumer.c 
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm -f $(CONSUMER_EXEC)

.PHONY: all clean
