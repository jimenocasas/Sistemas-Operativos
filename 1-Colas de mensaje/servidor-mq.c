#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mqueue.h>
#include <errno.h>
#include <unistd.h>
#include "claves.h"
#include <pthread.h>
#include "real.h"

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

mqd_t server_mq;
int sync_copied = 0;
pthread_cond_t sync_cond;
pthread_mutex_t sync_mutex;

// Procesa el mensaje recibido del proxy y ejecuta la operación correspondiente
void* process_message(void *msg) {
    struct Message msg_local;
    pthread_mutex_lock(&sync_mutex);
    msg_local = *(struct Message *)msg;
    printf("Procesando operación %d para clave %d\n", msg_local.operation, msg_local.key);
    sync_copied = 1;
    printf("sync_copied ya es 1\n");
    pthread_cond_broadcast(&sync_cond);
    pthread_mutex_unlock(&sync_mutex);

    printf("sync_copied = %i", sync_copied);
    printf("sync_mutex desbloqueado\n");

    switch (msg_local.operation) {
        case 1:
            msg_local.result = _set_value(msg_local.key, msg_local.value1, msg_local.N_value2, msg_local.V_value2, msg_local.value3);
            break;
        case 2:
            msg_local.result = _get_value(msg_local.key, msg_local.value1, &msg_local.N_value2, msg_local.V_value2, &msg_local.value3);
            break;
        case 3:
            msg_local.result = _modify_value(msg_local.key, msg_local.value1, msg_local.N_value2, msg_local.V_value2, msg_local.value3);
            break;
        case 4:
            msg_local.result = _delete_key(msg_local.key);
            break;
        case 5:
            msg_local.result = _exist(msg_local.key);
            break;
        case 6:
            printf("Operación destruir\n");
            msg_local.result = _destroy();
            printf("Operación destruir REALIZADA\n");
            break;
        
        default:
            msg_local.result = -1;
            printf("Operación inválida: %d\n", msg_local.operation);
            break;
    }

    // Enviar la respuesta al cliente en su cola específica
    mqd_t client_mq = mq_open(msg_local.response_queue, O_WRONLY);
    if (client_mq == (mqd_t)-1) {
        perror("Error al abrir la cola del cliente para enviar la respuesta");
        pthread_exit(NULL);
    }

    if (mq_send(client_mq, (char *)&msg_local, sizeof(struct Message), 0) == -1) {
        perror("Error al enviar la respuesta al cliente");
        pthread_exit(NULL);
    }
    printf("Respuesta enviada al cliente\n");
    mq_close(client_mq);
    pthread_exit(NULL);
}

int main() {
    struct mq_attr attr;
    struct Message msg;
    ssize_t bytes_read;

    attr.mq_flags = 0;
    attr.mq_maxmsg = 10;
    attr.mq_msgsize = sizeof(struct Message);
    attr.mq_curmsgs = 0;


    pthread_attr_t attr_th;

    pthread_attr_init(&attr_th);
    pthread_attr_setdetachstate(&attr_th, PTHREAD_CREATE_DETACHED);
    pthread_mutex_init(&sync_mutex, NULL);
    pthread_cond_init(&sync_cond, NULL);

    server_mq = mq_open(SERVER_QUEUE, O_CREAT | O_RDONLY, 0644, &attr);
    if (server_mq == (mqd_t)-1) {
        perror("Error al abrir la cola del servidor");
        exit(EXIT_FAILURE);
    }

    printf("Servidor en funcionamiento...\n");

    sync_copied = 0;
    while (1) {
        bytes_read = mq_receive(server_mq, (char *)&msg, sizeof(struct Message), NULL);
        printf("Mensaje recibido por el servidor\n");

        if (bytes_read > 0) {
            pthread_t thid;
            pthread_create(&thid, &attr_th, process_message, (void *)&msg);
            pthread_mutex_lock(&sync_mutex);

            printf("Esperando a que el hilo termine de copiar el mensaje\n");
            while (sync_copied == 0) {
                fprintf(stderr, "Hilo esperando a que se copie el mensaje\n");
                pthread_cond_wait(&sync_cond, &sync_mutex);
                printf("Ya he salido del wait");
                
            }
            printf("Hilo listo para procesar el mensaje\n");

            sync_copied = 0;
            pthread_mutex_unlock(&sync_mutex);
            
            printf("Ya se ha acabado la primera operacion\n");
        } else {
            perror("Error al recibir el mensaje");
        }
    }

    return 0;
}
