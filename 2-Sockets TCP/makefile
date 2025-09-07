# Compilador y banderas
CC = gcc
CFLAGS = -Wall -g -pthread -fPIC
LDFLAGS = -pthread -L/path/to/cjson -lcjson

# Nombres de los ejecutables y biblioteca
SERVER = servidor-sock
PROXY = proxy-sock
CLIENT1 = app-cliente-1
CLIENT2 = app-cliente-2
LIBRARY = libclaves.so
# Archivos fuente
SERVER_SRC = servidor-sock.c claves.c
LIBRARY_SRC = proxy-sock.c claves.c
CLIENT1_SRC = app-cliente-1.c
CLIENT2_SRC = app-cliente-2.c

# Archivos objeto
SERVER_OBJ = $(SERVER_SRC:.c=.o)
LIBRARY_OBJ = $(LIBRARY_SRC:.c=.o)
CLIENT1_OBJ = $(CLIENT1_SRC:.c=.o)
CLIENT2_OBJ = $(CLIENT2_SRC:.c=.o)

# Regla por defecto: compilar todo y exportar variables
all: export IP_TUPLAS=127.0.0.1
all: export PORT_TUPLAS=4500
all: build

# Compilar todo
build: $(SERVER) $(LIBRARY) $(CLIENT1) $(CLIENT2)
	@echo "Variables de entorno configuradas: IP_TUPLAS=$(IP_TUPLAS), PORT_TUPLAS=$(PORT_TUPLAS)"

# Regla por defecto: compilar todo
all: $(SERVER) $(LIBRARY) $(CLIENT1) $(CLIENT2)

# Compilar el servidor
$(SERVER): $(SERVER_OBJ)
	$(CC) -o $(SERVER) $(SERVER_OBJ) $(LDFLAGS)

# Crear la biblioteca dinámica
$(LIBRARY): $(LIBRARY_OBJ)
	$(CC) -shared -o $(LIBRARY) $(LIBRARY_OBJ) -L/path/to/cjson -lcjson

# Compilar el cliente 1 enlazándolo con la biblioteca compartida
$(CLIENT1): $(CLIENT1_SRC) $(LIBRARY)
	$(CC) -o $(CLIENT1) $(CLIENT1_SRC) -L. -lclaves -Wl,-rpath=.

# Compilar el cliente 2 enlazándolo con la biblioteca compartida
$(CLIENT2): $(CLIENT2_SRC) $(LIBRARY)
	$(CC) -o $(CLIENT2) $(CLIENT2_SRC) -L. -lclaves -Wl,-rpath=.

# Reglas para compilar archivos .c a .o
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Limpiar archivos generados
clean:
	rm -f $(SERVER) $(CLIENT1) $(CLIENT2) $(LIBRARY) *.o

# Limpiar tuplas ejecutando el cliente 1 (destroy)
clean_tuplas:
	@echo "Limpieza de tuplas con Cliente 1 (destroy)..."
	@IP_TUPLAS=127.0.0.1 PORT_TUPLAS=4500 ./app-cliente-1

# Lanzar 5 clientes 2 en paralelo
lanzar_pruebas:
	@echo "Ejecutando 5 instancias de Cliente 2 en paralelo tras limpiar tuplas..."
	@IP_TUPLAS=127.0.0.1 PORT_TUPLAS=4500 ./app-cliente-2 & \
	IP_TUPLAS=127.0.0.1 PORT_TUPLAS=4500 ./app-cliente-2 & \
	IP_TUPLAS=127.0.0.1 PORT_TUPLAS=4500 ./app-cliente-2 & \
	IP_TUPLAS=127.0.0.1 PORT_TUPLAS=4500 ./app-cliente-2 & \
	IP_TUPLAS=127.0.0.1 PORT_TUPLAS=4500 ./app-cliente-2

