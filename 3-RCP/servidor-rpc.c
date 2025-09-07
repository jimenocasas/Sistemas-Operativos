#include <stdio.h>
#include <string.h>
#include "claves_rpc.h"
#include "real.h"  // Usa struct Coord y funciones _set_value, _get_value, etc.

int *destroy_1_svc(void *arg, struct svc_req *rqstp) {
    static int result;
    result = _destroy();
    return &result;
}

int *set_value_1_svc(struct Entrada *entrada, struct svc_req *rqstp) {
    static int result;

    struct Coord coord;
    coord.x = entrada->value3.x;
    coord.y = entrada->value3.y;

    result = _set_value(
        entrada->key,
        entrada->value1,
        entrada->N_value2,
        entrada->V_value2.V_value2_val,
        coord
    );

    return &result;
}

struct RespuestaGet *get_value_1_svc(struct SoloClave *clave, struct svc_req *rqstp) {
    static struct RespuestaGet respuesta;
    static char value1[256];
    static double V_value2[32];  // buffer temporal
    struct Coord coord;
    int N;

    int res = _get_value(
        clave->key,
        value1,
        &N,
        V_value2,
        &coord
    );

    respuesta.resultado = res;

    if (res == 0) {
        respuesta.value1 = value1;
        respuesta.N_value2 = N;
        respuesta.V_value2.V_value2_len = N;
        respuesta.V_value2.V_value2_val = V_value2;
        respuesta.value3.x = coord.x;
        respuesta.value3.y = coord.y;
    }

    return &respuesta;
}

int *modify_value_1_svc(struct Entrada *entrada, struct svc_req *rqstp) {
    static int result;

    struct Coord coord;
    coord.x = entrada->value3.x;
    coord.y = entrada->value3.y;

    result = _modify_value(
        entrada->key,
        entrada->value1,
        entrada->N_value2,
        entrada->V_value2.V_value2_val,
        coord
    );

    return &result;
}

int *delete_key_1_svc(struct SoloClave *clave, struct svc_req *rqstp) {
    static int result;
    result = _delete_key(clave->key);
    return &result;
}

int *exist_1_svc(struct SoloClave *clave, struct svc_req *rqstp) {
    static int result;
    result = _exist(clave->key);
    return &result;
}
