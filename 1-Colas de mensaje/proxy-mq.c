#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mqueue.h>
#include <errno.h>
#include <unistd.h>
#include "claves.h"

#define SERVER_QUEUE "/SERVIDOR"

struct Message {
    int operation;
    int key;
    char value1[256];
    int N_value2;
    double V_value2[32];
    struct Coord value3;
    int result;
    char response_queue[32];  // Cola específica del cliente
};

// Función para enviar mensajes al servidor y recibir la respuesta
int send_message(struct Message *msg) {
    mqd_t server_mq, client_mq;
    struct mq_attr attr;
    char client_queue_name[32];

    // Crear el nombre de la cola del cliente con getpid()
    sprintf(client_queue_name, "/CLT_%d", getpid());
    strcpy(msg->response_queue, client_queue_name);

    // Configurar los atributos de la cola
    attr.mq_flags = 0;
    attr.mq_maxmsg = 10;
    attr.mq_msgsize = sizeof(struct Message);
    attr.mq_curmsgs = 0;

    // Crear la cola efímera del cliente para recibir la respuesta
    client_mq = mq_open(client_queue_name, O_CREAT | O_RDONLY, 0700, &attr);
    if (client_mq == (mqd_t)-1) {
        perror("Error al crear la cola del cliente");
        return -2;
    }

    // Abrir la cola del servidor
    server_mq = mq_open(SERVER_QUEUE, O_WRONLY);
    if (server_mq == (mqd_t)-1) {
        perror("Error al abrir la cola del servidor");
        mq_unlink(client_queue_name);
        return -2;
    }

    // Enviar la petición al servidor
    if (mq_send(server_mq, (char *)msg, sizeof(struct Message), 0) == -1) {
        perror("Error al enviar mensaje al servidor");
        mq_close(server_mq);
        mq_unlink(client_queue_name);
        return -2;
    }

    mq_close(server_mq);

    // Esperar y recibir la respuesta
    if (mq_receive(client_mq, (char *)msg, sizeof(struct Message), NULL) == -1) {
        perror("Error al recibir la respuesta del servidor");
        mq_close(client_mq);
        mq_unlink(client_queue_name);
        return -2;
    }


    // Cerrar y eliminar la cola del cliente
    mq_close(client_mq);
    mq_unlink(client_queue_name);

    return msg->result;
}

// Implementación de las funciones de la API
int set_value(int key, char *value1, int N_value2, double *V_value2, struct Coord value3) {

    struct Message msg = {0};
    msg.operation = 1;
    msg.key = key;
    msg.N_value2 = N_value2;
    msg.value3 = value3;

    if (strlen(value1) > 255) {
        strncpy(msg.value1, value1, strlen(value1));
    }

    else{
        strncpy(msg.value1, value1, 255);
        msg.value1[255] = '\0';
        printf("value1: %s\n", msg.value1);
    }

    if (N_value2 < 1 || N_value2 > 32) {
        memcpy(msg.V_value2, V_value2, sizeof(double) * 32);
    }
    else {
        memcpy(msg.V_value2, V_value2, sizeof(double) * N_value2);
        printf("V_value2: %f\n", msg.V_value2[0]);
    }

    return send_message(&msg);
}


int get_value(int key, char *value1, int *N_value2, double *V_value2, struct Coord *value3) {
    struct Message msg = {0
    };
    msg.operation = 2;
    msg.key = key;

    if (send_message(&msg) == -2) return -2;

    strncpy(value1, msg.value1, 255);
    value1[255] = '\0';
    *N_value2 = msg.N_value2;
    memcpy(V_value2, msg.V_value2, sizeof(double) * (*N_value2));
    *value3 = msg.value3;

    return msg.result;
}

int modify_value(int key, char *value1, int N_value2, double *V_value2, struct Coord value3) {
    struct Message msg = {0};
    msg.operation = 3;
    msg.key = key;
    msg.N_value2 = N_value2;
    msg.value3 = value3;

    strncpy(msg.value1, value1, 255);
    msg.value1[255] = '\0';
    memcpy(msg.V_value2, V_value2, sizeof(double) * N_value2);

    return send_message(&msg);
}

int delete_key(int key) {
    struct Message msg = {0};
    msg.operation = 4;
    msg.key = key;

    return send_message(&msg);
}

int exist(int key) {
    struct Message msg = {0};
    msg.operation = 5;
    msg.key = key;

    if (send_message(&msg) == -2) return -2;
    return msg.result;
}

int destroy() {
    struct Message msg = {0};
    msg.operation = 6;

    return send_message(&msg);
}

