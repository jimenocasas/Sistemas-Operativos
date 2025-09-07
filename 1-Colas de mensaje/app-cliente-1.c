#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "claves.h"

// Función para imprimir el contenido del vector
void imprimir_vector(double *vector, int N) {
    printf("[");
    for (int i = 0; i < N; i++) {
        printf("%.2f", vector[i]);
        if (i < N - 1) printf(", ");
    }
    printf("]\n");
}

// Función para imprimir una línea separadora
void imprimir_separador(const char *operacion) {
    printf("\n=====================================\n");
    printf(">>> %s\n", operacion);
    printf("=====================================\n");
}

int main() {
    struct Coord coord;
    double V_value2[32];
    char value1[256];
    int N_value2;
    int resultado;

    printf("\n=== Prueba del Sistema de Tuplas <key-value1-value2-value3> ===\n\n");

    // 1. Inicializar el sistema
    imprimir_separador("INICIALIZANDO EL SISTEMA");
    resultado = destroy();
    printf("Resultado destroy: %d\n\n", resultado);

    // 2. Insertar valores con set_value
    imprimir_separador("INSERTANDO VALORES");
    coord.x = 10; coord.y = 20;
    double vector1[] = {1.1, 2.2, 3.3};
    printf("Insertando clave 1 con value1='Valor 1', value2=");
    imprimir_vector(vector1, 3);
    resultado = set_value(1, "Valor 1", 3, vector1, coord);
    printf("Resultado set_value (clave 1): %d\n\n", resultado);

    // 3. Obtener el valor con get_value
    imprimir_separador("OBTENIENDO VALORES");
    resultado = get_value(1, value1, &N_value2, V_value2, &coord);
    printf("Resultado get_value: %d\n", resultado);
    printf("value1: %s\n", value1);
    printf("N_value2: %d, value2: ", N_value2);
    imprimir_vector(V_value2, N_value2);
    printf("value3: (%d, %d)\n\n", coord.x, coord.y);

    // 4. Modificar el valor con modify_value
    imprimir_separador("MODIFICANDO VALORES");
    coord.x = 30; coord.y = 40;
    double vector2[] = {9.9, 8.8, 7.7};
    printf("Modificando clave 1 con value1='Valor Modificado', value2=");
    imprimir_vector(vector2, 3);
    resultado = modify_value(1, "Valor Modificado", 3, vector2, coord);
    printf("Resultado modify_value: %d\n\n", resultado);

    // Obtener el valor modificado
    imprimir_separador("OBTENIENDO VALORES MODIFICADOS");
    resultado = get_value(1, value1, &N_value2, V_value2, &coord);
    printf("Resultado get_value: %d\n", resultado);
    printf("value1: %s\n", value1);
    printf("N_value2: %d, value2: ", N_value2);
    imprimir_vector(V_value2, N_value2);
    printf("value3: (%d, %d)\n\n", coord.x, coord.y);

    // 5. Comprobar la existencia de la clave con exist
    imprimir_separador("COMPROBANDO EXISTENCIA DE CLAVE");
    resultado = exist(1);
    printf("Resultado exist (clave 1): %d\n\n", resultado);

    // 6. Eliminar la clave con delete_key
    imprimir_separador("ELIMINANDO CLAVE");
    resultado = delete_key(1);
    printf("Resultado delete_key: %d\n\n", resultado);

    // 7. Comprobar la existencia de la clave tras la eliminación
    imprimir_separador("COMPROBANDO EXISTENCIA DESPUÉS DE ELIMINAR");
    resultado = exist(1);
    printf("Resultado exist (clave 1): %d\n\n", resultado);

    // 8. Destruir el sistema de tuplas
    imprimir_separador("DESTRUYENDO TODAS LAS TUPLAS");
    resultado = destroy();
    printf("Resultado destroy: %d\n\n", resultado);

    printf("\n=== Fin de las Pruebas ===\n");
    return 0;
}
