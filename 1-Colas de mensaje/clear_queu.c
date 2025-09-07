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
    char response_queue[32];  // Cola del cliente
};

int main() {
    mqd_t server_mq;
    struct mq_attr attr;
    struct Message msg;
    ssize_t bytes_read;

    // Abrir la cola del servidor
    server_mq = mq_open(SERVER_QUEUE, O_RDONLY);
    if (server_mq == (mqd_t)-1) {
        perror("Error al abrir la cola del servidor");
        exit(EXIT_FAILURE);
    }

    // Obtener los atributos de la cola
    if (mq_getattr(server_mq, &attr) == -1) {
        perror("Error al obtener los atributos de la cola del servidor");
        mq_close(server_mq);
        exit(EXIT_FAILURE);
    }

    printf("Eliminando mensajes de la cola del servidor...\n");

    // Leer y descartar todos los mensajes en la cola
    while ((bytes_read = mq_receive(server_mq, (char *)&msg, sizeof(struct Message), NULL)) >= 0) {
        printf("Mensaje eliminado\n");
    }

    if (errno != EAGAIN) {
        perror("Error al recibir mensaje");
    }

    mq_close(server_mq);
    printf("Todos los mensajes han sido eliminados de la cola del servidor.\n");

    return 0;
}