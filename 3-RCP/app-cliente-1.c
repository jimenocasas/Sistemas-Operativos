#include <stdio.h>
#include <stdlib.h>
#include "claves.h"

// Función para imprimir una línea separadora
void imprimir_separador(const char *operacion) {
    printf("\n=====================================\n");
    printf(">>> %s\n", operacion);
    printf("=====================================\n");
}

int main() {
    printf("\n=== CLIENTE 1: DESTRUCCIÓN INICIAL DEL SISTEMA DE TUPLAS ===\n");

    imprimir_separador("DESTRUYENDO TODAS LAS TUPLAS");

    int resultado = destroy();
    printf("Resultado destroy: %d\n", resultado);

    printf("\n=== FIN DE CLIENTE 1 ===\n");
    return 0;
}
