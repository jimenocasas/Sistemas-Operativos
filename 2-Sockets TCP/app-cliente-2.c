#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <pthread.h>
#include "claves.h"
#include "real.h"

// Mutex global para evitar condiciones de carrera al imprimir desde múltiples hilos
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

// Función auxiliar para imprimir los datos de una operación en formato JSON
void imprimir_json(int op, int key, const char* value1, int N, double* V, struct Coord v3, int resultado) {
    printf("json_str: {\n");
    printf("        \"operation\":    %d,\n", op);
    printf("        \"key\":  %d,\n", key);
    printf("        \"value1\":       \"%s\",\n", value1);
    printf("        \"N_value2\":     %d,\n", N);
    printf("        \"V_value2\":     [");
    for (int i = 0; i < N; i++) {
        printf("%.2f", V[i]);
        if (i < N - 1) printf(", ");
    }
    printf("],\n");
    printf("        \"value3\":       {\n");
    printf("                \"x\":    %d,\n", v3.x);
    printf("                \"y\":    %d\n", v3.y);
    printf("        },\n");
    printf("        \"resultado\":    %d\n", resultado);
    printf("}\n");
}

// Función principal que realiza todas las operaciones del cliente 2
void pruebas_cliente_2() {
    pid_t pid = getpid();
    int base_key = 1000 + (pid % 1000);  // Clave única basada en PID para evitar colisiones
    int res;

    // Valores de prueba
    const char *value1_init = "Valor Inicial";
    const char *value1_mod = "Valor Modificado";
    double valores1[3] = {1.1, 2.2, 3.3};
    double valores2[3] = {9.9, 8.8, 7.7};
    char value1_out[256];
    int N;
    double valores_out[32];
    struct Coord val3 = {10, 20};
    struct Coord val3_mod = {30, 40};
    struct Coord val3_out;

    // Imprime identificador de cliente
    pthread_mutex_lock(&mutex);
    printf("\n========== CLIENTE %d ==========\n", pid);
    fflush(stdout);
    pthread_mutex_unlock(&mutex);

    // Operación SET
    pthread_mutex_lock(&mutex);
    printf("\n=== SET_VALUE ===\n");
    res = set_value(base_key, (char *)value1_init, 3, valores1, val3);
    imprimir_json(1, base_key, value1_init, 3, valores1, val3, res);
    printf("Cliente %d - set_value: %d\n", pid, res);
    fflush(stdout);
    pthread_mutex_unlock(&mutex);

    // Operación GET
    pthread_mutex_lock(&mutex);
    printf("\n=== GET_VALUE ===\n");
    res = get_value(base_key, value1_out, &N, valores_out, &val3_out);
    imprimir_json(2, base_key, value1_out, N, valores_out, val3_out, res);
    printf("Cliente %d - get_value: %d\n", pid, res);
    fflush(stdout);
    pthread_mutex_unlock(&mutex);

    // Operación MODIFY
    pthread_mutex_lock(&mutex);
    printf("\n=== MODIFY_VALUE ===\n");
    res = modify_value(base_key, (char *)value1_mod, 3, valores2, val3_mod);
    imprimir_json(3, base_key, value1_mod, 3, valores2, val3_mod, res);
    printf("Cliente %d - modify_value: %d\n", pid, res);
    fflush(stdout);
    pthread_mutex_unlock(&mutex);

    // GET tras modificar
    pthread_mutex_lock(&mutex);
    printf("\n=== GET_VALUE (tras modificar) ===\n");
    res = get_value(base_key, value1_out, &N, valores_out, &val3_out);
    imprimir_json(2, base_key, value1_out, N, valores_out, val3_out, res);
    printf("Cliente %d - get_value tras modificar: %d\n", pid, res);
    fflush(stdout);
    pthread_mutex_unlock(&mutex);

    // Operación EXIST
    pthread_mutex_lock(&mutex);
    printf("\n=== EXIST ===\n");
    res = exist(base_key);
    imprimir_json(5, base_key, "-", 0, NULL, (struct Coord){0,0}, res);
    printf("Cliente %d - exist: %d\n", pid, res);
    fflush(stdout);
    pthread_mutex_unlock(&mutex);

    // Operación DELETE
    pthread_mutex_lock(&mutex);
    printf("\n=== DELETE_KEY ===\n");
    res = delete_key(base_key);
    imprimir_json(4, base_key, "-", 0, NULL, (struct Coord){0,0}, res);
    printf("Cliente %d - delete_key: %d\n", pid, res);
    fflush(stdout);
    pthread_mutex_unlock(&mutex);

    // EXIST tras eliminar
    pthread_mutex_lock(&mutex);
    printf("\n=== EXIST (tras delete) ===\n");
    res = exist(base_key);
    imprimir_json(5, base_key, "-", 0, NULL, (struct Coord){0,0}, res);
    printf("Cliente %d - exist tras delete: %d\n", pid, res);
    fflush(stdout);
    pthread_mutex_unlock(&mutex);

    // Fin del cliente
    pthread_mutex_lock(&mutex);
    printf("\n========== FIN CLIENTE %d ==========\n", pid);
    fflush(stdout);
    pthread_mutex_unlock(&mutex);
}

int main() {
    pruebas_cliente_2(); // Ejecuta las pruebas
    return 0;
}