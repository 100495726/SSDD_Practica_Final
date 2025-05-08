CC = gcc
CFLAGS = -Wall -Wextra -pthread
OBJS = server.o operaciones.o

all: server

server: $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

server.o: server.c operaciones.h
	$(CC) $(CFLAGS) -c server.c

operaciones.o: operaciones.c operaciones.h
	$(CC) $(CFLAGS) -c operaciones.c

clean:
	rm -f server $(OBJS)