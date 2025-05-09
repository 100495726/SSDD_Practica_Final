CC = gcc
CFLAGS = -Wall -Wextra -pthread
RPCGEN = rpcgen
RPCFLAGS = -NM

# Objetos para el servidor principal
SERVER_OBJS = server.o operaciones.o

# Objetos para el servicio RPC
RPC_OBJS = print_request_svc.o print_request_server.o print_request_xdr.o

# Targets principales
all: server print_request_svc

# Servidor principal
server: $(SERVER_OBJS)
	$(CC) $(CFLAGS) -o $@ $^ -lnsl

server.o: server.c operaciones.h print_request.h
	$(CC) $(CFLAGS) -c server.c

operaciones.o: operaciones.c operaciones.h
	$(CC) $(CFLAGS) -c operaciones.c

# Servicio RPC
print_request_svc: $(RPC_OBJS)
	$(CC) $(CFLAGS) -o $@ $^ -lnsl

# Archivos generados por rpcgen
print_request.h print_request_clnt.c print_request_svc.c print_request_xdr.c: print_request.x
	$(RPCGEN) $(RPCFLAGS) $<

print_request_clnt.o: print_request_clnt.c print_request.h
	$(CC) $(CFLAGS) -c $<

print_request_svc.o: print_request_svc.c print_request.h
	$(CC) $(CFLAGS) -c $<

print_request_server.o: print_request_server.c print_request.h
	$(CC) $(CFLAGS) -c $<

print_request_xdr.o: print_request_xdr.c print_request.h
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f server print_request_svc *.o print_request.h print_request_clnt.c print_request_svc.c print_request_xdr.c