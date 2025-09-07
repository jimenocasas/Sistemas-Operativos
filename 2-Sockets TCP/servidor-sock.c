#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <semaphore.h>
#include "claves.h"
#include "real.h"
#include <cjson/cJSON.h>
#include <errno.h>

#define BUFFER_SIZE 1024

pthread_mutex_t lista_mutex = PTHREAD_MUTEX_INITIALIZER;

ssize_t readLine(int fd, void *buffer, size_t n)
{
	ssize_t numRead;  /* num of bytes fetched by last read() */
	size_t totRead;	  /* total bytes read so far */
	char *buf;
	char ch;


	if (n <= 0 || buffer == NULL) { 
		errno = EINVAL;
		return -1; 
	}

	buf = buffer;
	totRead = 0;
	
	for (;;) {
        	numRead = read(fd, &ch, 1);	/* read a byte */

        	if (numRead == -1) {	
            		if (errno == EINTR)	/* interrupted -> restart read() */
                		continue;
            	else
			return -1;		/* some other error */
        	} else if (numRead == 0) {	/* EOF */
            		if (totRead == 0)	/* no byres read; return 0 */
                		return 0;
			else
                		break;
        	} else {			/* numRead must be 1 if we get here*/
            		if (ch == '\n')
                		break;
            		if (ch == '\0')
                		break;
            		if (totRead < n - 1) {		/* discard > (n-1) bytes */
				totRead++;
				*buf++ = ch; 
			}
		} 
	}
	
	*buf = '\0';
    	return totRead;
}


void *handle_client(void *client_socket) {
    int sd = *(int *)client_socket;
    free(client_socket);

    char buffer[BUFFER_SIZE] = {0};
    size_t bytes_received = readLine(sd, buffer, BUFFER_SIZE); // Leer la línea completa

    if (bytes_received <= 0) {
        perror("Error al recibir datos");
        close(sd);
        return NULL;
    }
    //buffer[bytes_received] = '\0';

    cJSON *json = cJSON_Parse(buffer);
    if (!json) {
        fprintf(stderr, "Error al parsear el JSON recibido.\n");
        close(sd);
        return NULL;
    }

    int operation = cJSON_GetObjectItem(json, "operation")->valueint;
    int key, N_value2 = 0, result;
    char value1[256];
    double V_value2[32];
    struct Coord value3;

    pthread_mutex_lock(&lista_mutex);
    switch (operation) {
        case 1: // set_value
        case 3: // modify_value
            key = cJSON_GetObjectItem(json, "key")->valueint;
            strcpy(value1, cJSON_GetObjectItem(json, "value1")->valuestring);
            N_value2 = cJSON_GetObjectItem(json, "N_value2")->valueint;
            cJSON *V_value2_json = cJSON_GetObjectItem(json, "V_value2");
            for (int i = 0; i < N_value2; i++) {
                V_value2[i] = cJSON_GetArrayItem(V_value2_json, i)->valuedouble;
            }
            cJSON *value3_item = cJSON_GetObjectItem(json, "value3");
            value3.x = cJSON_GetObjectItem(value3_item, "x")->valueint;
            value3.y = cJSON_GetObjectItem(value3_item, "y")->valueint;

            result = (operation == 1) ?
                _set_value(key, value1, N_value2, V_value2, value3) :
                _modify_value(key, value1, N_value2, V_value2, value3);
            break;

        case 2: // get_value
            key = cJSON_GetObjectItem(json, "key")->valueint;
            result = _get_value(key, value1, &N_value2, V_value2, &value3);
            if (result == 0) {
                cJSON_AddStringToObject(json, "value1", value1);
                cJSON_AddNumberToObject(json, "N_value2", N_value2);
                cJSON *array = cJSON_CreateArray();
                for (int i = 0; i < N_value2; i++) {
                    cJSON_AddItemToArray(array, cJSON_CreateNumber(V_value2[i]));
                }
                cJSON_AddItemToObject(json, "V_value2", array);

                cJSON *coord = cJSON_CreateObject();
                cJSON_AddNumberToObject(coord, "x", value3.x);
                cJSON_AddNumberToObject(coord, "y", value3.y);
                cJSON_AddItemToObject(json, "value3", coord);
            }
            break;

        case 4:
            key = cJSON_GetObjectItem(json, "key")->valueint;
            result = _delete_key(key);
            break;

        case 5:
            key = cJSON_GetObjectItem(json, "key")->valueint;
            result = _exist(key);
            break;

        case 6:
            result = _destroy();
            break;

        default:
            result = -1;
    }
    pthread_mutex_unlock(&lista_mutex);

    cJSON_ReplaceItemInObject(json, "resultado", cJSON_CreateNumber(result));
    char *json_response = cJSON_Print(json);
    if (json_response) {
        send(sd, json_response, strlen(json_response), 0);
        free(json_response);
    }

    cJSON_Delete(json);
    close(sd);
    pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Uso: %s <PUERTO>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int sd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sd < 0) {
        perror("Error al crear el socket");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in server_addr;
    int addr_len = sizeof(server_addr);
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(atoi(argv[1]));

    // Reutilizar la dirección del socket, en caso de que el puerto esté en uso
    int val = 1;
    setsockopt(sd, SOL_SOCKET, SO_REUSEADDR,(void*) &val, sizeof(val));


    if (bind(sd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Error en bind");
        close(sd);
        exit(EXIT_FAILURE);
    }

    if (listen(sd, 10) < 0) {
        perror("Error en listen");
        close(sd);
        exit(EXIT_FAILURE);
    }

    printf("Servidor escuchando en el puerto %s...\n", argv[1]);

    while (1) {
        int *client_sd = malloc(sizeof(int));

        printf("Esperando conexion\n");
        *client_sd = accept(sd, (struct sockaddr *)&server_addr, (socklen_t *)&addr_len);

        if (*client_sd < 0) {
            perror("Error en accept\n");
            free(client_sd);
            continue;
        }

        printf("Conexión aceptada de IP: %s   Puerto: %d\n", inet_ntoa(server_addr.sin_addr), server_addr.sin_port);

        pthread_t thread_id;
        pthread_create(&thread_id, NULL, handle_client, client_sd);
        pthread_detach(thread_id);
    }

    close(sd);
    return 0;
}
