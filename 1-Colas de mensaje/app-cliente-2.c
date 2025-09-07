#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "claves.h"

void imprimir_vector(double *vector, int N) {
    printf("[");
    for (int i = 0; i < N; i++) {
        printf("%.2f", vector[i]);
        if (i < N - 1) printf(", ");
    }
    printf("]\n");
}

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

    printf("\n=== Pruebas de Casos Límite del Sistema de Tuplas ===\n\n");

    // 1. Inicializar el sistema
    imprimir_separador("INICIALIZANDO EL SISTEMA");
    resultado = destroy();
    printf("Resultado destroy: %d\n\n", resultado);

    // 2. Insertar clave con N_value2 = 1 (mínimo permitido)
    imprimir_separador("INSERTANDO CLAVE CON N_value2 = 1");
    coord.x = 5; coord.y = 10;
    double vector_min[] = {3.14};
    resultado = set_value(100, "Min Value", 1, vector_min, coord);
    printf("Resultado set_value (clave 100, N=1): %d (Debe ser 0)\n\n", resultado);

    // 3. Insertar clave con N_value2 = 32 (máximo permitido)
    imprimir_separador("INSERTANDO CLAVE CON N_value2 = 32");
    coord.x = 15; coord.y = 25;
    double vector_max[32];
    for (int i = 0; i < 32; i++) vector_max[i] = i * 1.1;
    resultado = set_value(101, "Max Value", 32, vector_max, coord);
    printf("Resultado set_value (clave 101, N=32): %d (Debe ser 0)\n\n", resultado);

    // 4. Intentar insertar clave con N_value2 = 33 (fuera de rango, debe fallar)
    imprimir_separador("INSERTANDO CLAVE CON N_value2 = 33 (DEBE FALLAR)");
    resultado = set_value(102, "Out of Range", 33, vector_max, coord);
    printf("Resultado set_value (clave 102, N=33): %d (Debe ser -1)\n\n", resultado);

    // 5. Intentar insertar clave con un value1 de más de 255 caracteres (debe fallar)
    imprimir_separador("INSERTANDO CLAVE CON value1 > 255 CARACTERES (DEBE FALLAR)");
    char long_value1[300];
    memset(long_value1, 'A', 299);
    long_value1[299] = '\0';  // Forzar que sea una cadena válida
    resultado = set_value(103, long_value1, 5, vector_max, coord);
    printf("Resultado set_value (clave 103, value1 muy largo): %d (Debe ser -1)\n\n", resultado);

    // 6. Intentar obtener clave inexistente (debe fallar)
    imprimir_separador("OBTENIENDO CLAVE INEXISTENTE (DEBE FALLAR)");
    resultado = get_value(200, value1, &N_value2, V_value2, &coord);
    printf("Resultado get_value (clave 200): %d (Debe ser -1)\n\n", resultado);

    // 7. Intentar modificar clave inexistente (debe fallar)
    imprimir_separador("MODIFICANDO CLAVE INEXISTENTE (DEBE FALLAR)");
    resultado = modify_value(300, "No Existe", 3, vector_max, coord);
    printf("Resultado modify_value (clave 300): %d (Debe ser -1)\n\n", resultado);

    // 8. Intentar eliminar clave inexistente (debe fallar)
    imprimir_separador("ELIMINANDO CLAVE INEXISTENTE (DEBE FALLAR)");
    resultado = delete_key(400);
    printf("Resultado delete_key (clave 400): %d (Debe ser -1)\n\n", resultado);

    // 9. Insertar, eliminar y verificar inexistencia
    imprimir_separador("INSERTAR Y ELIMINAR CLAVE");
    resultado = set_value(500, "Temporal", 2, vector_min, coord);
    printf("Resultado set_value (clave 500): %d (Debe ser 0)\n", resultado);
    resultado = delete_key(500);
    printf("Resultado delete_key (clave 500): %d (Debe ser 0)\n", resultado);
    resultado = exist(500);
    printf("Resultado exist (clave 500): %d (Debe ser 0)\n\n", resultado);

    // 10. Insertar múltiples claves y verificar existencia
    imprimir_separador("INSERTAR MÚLTIPLES CLAVES Y VERIFICAR EXISTENCIA");
    for (int i = 600; i < 610; i++) {
        resultado = set_value(i, "Multiple", 2, vector_min, coord);
        printf("Insertando clave %d: %d (Debe ser 0)\n", i, resultado);
    }
    for (int i = 600; i < 610; i++) {
        resultado = exist(i);
        printf("Existencia clave %d: %d (Debe ser 1)\n", i, resultado);
    }

    // 11. Destruir el sistema y verificar que todas las claves desaparecen
    imprimir_separador("DESTRUYENDO EL SISTEMA Y VERIFICANDO");
    resultado = destroy();
    printf("Resultado destroy: %d (Debe ser 0 o -1 si ya estaba vacío)\n", resultado);

    for (int i = 600; i < 610; i++) {
        resultado = exist(i);
        printf("Existencia clave %d tras destroy: %d (Debe ser 0)\n", i, resultado);
    }

    printf("\n=== Fin de las Pruebas de Casos Límite ===\n");
    return 0;
}
