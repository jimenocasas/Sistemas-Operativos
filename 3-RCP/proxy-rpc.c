#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "claves_rpc.h"
#include "real.h" // Contiene las funciones _set_value, etc.

CLIENT *clnt = NULL;

void init_client() {
    if (clnt != NULL) return;

    char *ip = getenv("IP_TUPLAS");
    if (!ip) {
        fprintf(stderr, "Error: variable de entorno IP_TUPLAS no definida.\n");
        exit(1);
    }

    clnt = clnt_create(ip, CLAVES_PROG, CLAVES_VERS, "tcp");
    if (!clnt) {
        clnt_pcreateerror(ip);
        exit(1);
    }
}

int destroy() {
    init_client();
    return *destroy_1(NULL, clnt);
}

int set_value(int key, char *value1, int N_value2, double *V_value2, struct Coord value3) {
    init_client();

    struct Entrada entrada;
    entrada.key = key;
    entrada.value1 = value1;
    entrada.N_value2 = N_value2;
    entrada.V_value2.V_value2_len = N_value2;
    entrada.V_value2.V_value2_val = V_value2;
    entrada.value3.x = value3.x;
    entrada.value3.y = value3.y;

    return *set_value_1(&entrada, clnt);
}

int get_value(int key, char *value1, int *N_value2, double *V_value2, struct Coord *value3) {
    init_client();

    struct SoloClave clave;
    clave.key = key;

    struct RespuestaGet *res = get_value_1(&clave, clnt);
    if (!res || res->resultado != 0) return -1;

    strncpy(value1, res->value1, 255);
    *N_value2 = res->V_value2.V_value2_len;
    memcpy(V_value2, res->V_value2.V_value2_val, (*N_value2) * sizeof(double));
    value3->x = res->value3.x;
    value3->y = res->value3.y;

    return 0;
}

int modify_value(int key, char *value1, int N_value2, double *V_value2, struct Coord value3) {
    init_client();

    struct Entrada entrada;
    entrada.key = key;
    entrada.value1 = value1;
    entrada.N_value2 = N_value2;
    entrada.V_value2.V_value2_len = N_value2;
    entrada.V_value2.V_value2_val = V_value2;
    entrada.value3.x = value3.x;
    entrada.value3.y = value3.y;

    return *modify_value_1(&entrada, clnt);
}

int delete_key(int key) {
    init_client();

    struct SoloClave clave;
    clave.key = key;

    return *delete_key_1(&clave, clnt);
}

int exist(int key) {
    init_client();

    struct SoloClave clave;
    clave.key = key;

    return *exist_1(&clave, clnt);
}
