CC=gcc
CFLAGS=-Wall -lrt -fPIC -g
LDFLAGS=-lrt

# Nombres de los ejecutables y biblioteca
SERVER=servidor
CLIENT=app-cliente-1
CLIENT_LIMIT=app-cliente-2
LIBRARY=libclaves.so

# Archivos fuente
SERVER_SRC=servidor-mq.c claves.c
PROXY_SRC=proxy-mq.c
CLIENT_SRC=app-cliente-1.c
CLIENT_LIMIT_SRC=app-cliente-2.c

# Archivos objeto
SERVER_OBJ=$(SERVER_SRC:.c=.o)
PROXY_OBJ=$(PROXY_SRC:.c=.o)
CLIENT_OBJ=$(CLIENT_SRC:.c=.o)
CLIENT_LIMIT_OBJ=$(CLIENT_LIMIT_SRC:.c=.o)

# Regla por defecto: compilar todo
all: $(SERVER) $(LIBRARY) $(CLIENT) $(CLIENT_LIMIT)

# Compilar el servidor
$(SERVER): $(SERVER_OBJ) 
	$(CC) -o $(SERVER) $(SERVER_OBJ) $(LDFLAGS)

# Crear la biblioteca dinámica
$(LIBRARY): $(PROXY_OBJ)
	$(CC) -shared -o $(LIBRARY) $(PROXY_OBJ) $(LDFLAGS)

# Compilar el cliente normal enlazándolo con la biblioteca compartida
$(CLIENT): $(CLIENT_SRC)
	$(CC) -o $(CLIENT) $(CLIENT_SRC) -L. -lclaves -Wl,-rpath=.

# Compilar el cliente de pruebas límite enlazándolo con la biblioteca compartida
$(CLIENT_LIMIT): $(CLIENT_LIMIT_SRC)
	$(CC) -o $(CLIENT_LIMIT) $(CLIENT_LIMIT_SRC) -L. -lclaves -Wl,-rpath=.

# Reglas para compilar archivos .c a .o
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Limpiar archivos generados
clean:
	rm -f $(SERVER) $(CLIENT) $(CLIENT_LIMIT) $(LIBRARY) *.o

# Ejecutar el servidor
run-server: $(SERVER)
	./$(SERVER)

# Ejecutar el cliente normal
run-client: $(CLIENT)
	./$(CLIENT)

# Ejecutar el cliente de pruebas límite
run-client-limit: $(CLIENT_LIMIT)
	./$(CLIENT_LIMIT)
