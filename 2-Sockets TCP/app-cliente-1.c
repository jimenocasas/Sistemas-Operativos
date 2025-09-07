#include <stdio.h>
#include <stdlib.h>
#include "claves.h"

// Función para imprimir una línea separadora con el nombre de la operación
void imprimir_separador(const char *operacion) {
    printf("\n=====================================\n");
    printf(">>> %s\n", operacion);
    printf("=====================================\n");
}

int main() {
    // Mensaje inicial indicando el objetivo del cliente 1
    printf("\n=== CLIENTE 1: DESTRUCCIÓN INICIAL DEL SISTEMA DE TUPLAS ===\n");

    // Imprime un separador para la operación de destrucción
    imprimir_separador("DESTRUYENDO TODAS LAS TUPLAS");

    // Llama a la función destroy() del sistema de tuplas para eliminar todas las entradas existentes
    int resultado = destroy();
    printf("Resultado destroy: %d\n", resultado); // Imprime el resultado de la operación

    // Mensaje de fin de ejecución del cliente
    printf("\n=== FIN DE CLIENTE 1 ===\n");
    return 0;
}