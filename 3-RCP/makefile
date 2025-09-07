CC = gcc
CFLAGS = -Wall -fPIC -g -I/usr/include/tirpc -Wno-unused-variable
LDFLAGS = -ltirpc -lpthread
LIBFLAGS = -shared

RPC_X = claves_rpc.x
RPC_HDR = claves_rpc.h
RPC_CLNT = claves_rpc_clnt.c
RPC_SVC = claves_rpc_svc.c
RPC_XDR = claves_rpc_xdr.c

# Objetivo principal
all: $(RPC_CLNT) $(RPC_SVC) $(RPC_XDR) $(RPC_HDR) libclaves.so servidor app-cliente-1 app-cliente-2

# Generar archivos RPC si no existen
$(RPC_CLNT) $(RPC_SVC) $(RPC_XDR) $(RPC_HDR): $(RPC_X)
	rpcgen -C $(RPC_X)

# Biblioteca compartida
libclaves.so: claves.o proxy-rpc.o $(RPC_CLNT) $(RPC_XDR)
	$(CC) $(CFLAGS) -o $@ claves.o proxy-rpc.o $(RPC_CLNT) $(RPC_XDR) $(LIBFLAGS) $(LDFLAGS)

# Compilar servidor RPC
servidor: servidor-rpc.o claves.o $(RPC_SVC) $(RPC_XDR)
	$(CC) $(CFLAGS) -o $@ servidor-rpc.o claves.o $(RPC_SVC) $(RPC_XDR) $(LDFLAGS)

# Compilar clientes
app-cliente-1: app-cliente-1.c libclaves.so
	$(CC) $(CFLAGS) -o $@ app-cliente-1.c -L. -lclaves -Wl,-rpath,.

app-cliente-2: app-cliente-2.c libclaves.so
	$(CC) $(CFLAGS) -o $@ app-cliente-2.c -L. -lclaves -Wl,-rpath,.

# Reglas generales
%.o: %.c
	$(CC) $(CFLAGS) -c $<

# Limpiar binarios y generados
clean:
	rm -f *.o *.so servidor app-cliente-1 app-cliente-2 \
		claves_rpc_clnt.c claves_rpc_svc.c claves_rpc_xdr.c claves_rpc.h

# Limpiar tuplas desde cliente 1
clean_tuplas:
	@echo "Limpieza de tuplas..."
	@IP_TUPLAS=127.0.0.1 ./app-cliente-1

# Ejecutar 5 clientes en paralelo
lanzar_pruebas:
	@echo "Ejecutando clientes 2 en paralelo..."
	@for i in 1 2 3 4 5; do IP_TUPLAS=127.0.0.1 ./app-cliente-2 & done; wait
