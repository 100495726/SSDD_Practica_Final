CC = gcc
CFLAGS = -Wall -Wextra -pthread
OBJS = server.o usuarios.o

all: server

server: $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

server.o: server.c usuarios.h
	$(CC) $(CFLAGS) -c server.c

usuarios.o: usuarios.c usuarios.h
	$(CC) $(CFLAGS) -c usuarios.c

clean:
	rm -f server $(OBJS)