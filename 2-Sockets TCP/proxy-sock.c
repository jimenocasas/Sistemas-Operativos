#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include "claves.h"
#include <cjson/cJSON.h>

#define BUFFER_SIZE 1024

// Función para conectar con el servidor
int connect_to_server() {
    char *server_ip = getenv("IP_TUPLAS");
    char *server_port = getenv("PORT_TUPLAS");

    if (!server_ip || !server_port) {
        fprintf(stderr, "Error: Variables de entorno IP_TUPLAS y PORT_TUPLAS no definidas.\n");
        return -1;
    }

    int sd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP); // Crear socket correctamente
    if (sd < 0) {
        perror("Error al crear el socket");
        return -1;
    }

    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(atoi(server_port));
    inet_pton(AF_INET, server_ip, &server_addr.sin_addr);

    if (connect(sd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Error al conectar con el servidor");
        close(sd);
        return -1;
    }

    return sd;
}

// Función para enviar JSON y recibir respuesta
char* send_request(const char *json_request) {
    int sd = connect_to_server();
    if (sd < 0) return NULL;

    send(sd, json_request, strlen(json_request) + 1, 0); //+1 para añadir un caracter de finalizacion TOCADO DESPUES DE LA ENTREGA

    static char buffer[BUFFER_SIZE];
    int bytes = recv(sd, buffer, BUFFER_SIZE - 1, 0);
    if (bytes < 0) {
        perror("recv");
        close(sd);
        return NULL;
    }
    buffer[bytes] = '\0';
    
    close(sd);
    return buffer;
}

// Implementación de set_value
int set_value(int key, char *value1, int N_value2, double *V_value2, struct Coord value3) {
    cJSON *json = cJSON_CreateObject();
    cJSON_AddNumberToObject(json, "operation", 1);
    cJSON_AddNumberToObject(json, "key", key);
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
    cJSON_AddNumberToObject(json, "resultado", -2); // Si el resultado es -2, ha habido un error ya que no se ha tocado 
    
    char *json_str = cJSON_Print(json);
    char *response = send_request(json_str);
    cJSON_Delete(json);
    free(json_str);
    
    if (!response) return -1;
    cJSON *response_json = cJSON_Parse(response);
    int result = cJSON_GetObjectItem(response_json, "resultado")->valueint;
    cJSON_Delete(response_json);
    return result;
}

// Implementación de get_value
int get_value(int key, char *value1, int *N_value2, double *V_value2, struct Coord *value3) {
    cJSON *json = cJSON_CreateObject();
    cJSON_AddNumberToObject(json, "operation", 2);
    cJSON_AddNumberToObject(json, "key", key);
    cJSON_AddNumberToObject(json, "resultado", -2); // Si el resultado es -2, ha habido un error ya que no se ha tocado 
    
    char *json_str = cJSON_Print(json);
    char *response = send_request(json_str);
    cJSON_Delete(json);
    free(json_str);
    
    if (!response) return -1;
    cJSON *response_json = cJSON_Parse(response);
    int result = cJSON_GetObjectItem(response_json, "resultado")->valueint;
    if (result == 0) {
        strcpy(value1, cJSON_GetObjectItem(response_json, "value1")->valuestring);
        *N_value2 = cJSON_GetObjectItem(response_json, "N_value2")->valueint;
        cJSON *array = cJSON_GetObjectItem(response_json, "V_value2");
        for (int i = 0; i < *N_value2; i++) {
            V_value2[i] = cJSON_GetArrayItem(array, i)->valuedouble;
        }
        cJSON *coord = cJSON_GetObjectItem(response_json, "value3");
        value3->x = cJSON_GetObjectItem(coord, "x")->valuedouble;
        value3->y = cJSON_GetObjectItem(coord, "y")->valuedouble;
    }
    cJSON_Delete(response_json);
    return result;
}

// Implementación de modify_value
int modify_value(int key, char *value1, int N_value2, double *V_value2, struct Coord value3) {
    cJSON *json = cJSON_CreateObject();
    cJSON_AddNumberToObject(json, "operation", 3);
    cJSON_AddNumberToObject(json, "key", key);
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
    cJSON_AddNumberToObject(json, "resultado", -2); // Si el resultado es -2, ha habido un error ya que no se ha tocado 
    
    char *json_str = cJSON_Print(json);
    char *response = send_request(json_str);
    cJSON_Delete(json);
    free(json_str);
    
    if (!response) return -1;
    cJSON *response_json = cJSON_Parse(response);
    int result = cJSON_GetObjectItem(response_json, "resultado")->valueint;
    cJSON_Delete(response_json);
    return result;
}

// Implementación de delete_value utilizando JSON
int delete_key(int key) {
    cJSON *json = cJSON_CreateObject();
    cJSON_AddNumberToObject(json, "operation", 4);
    cJSON_AddNumberToObject(json, "key", key);
    cJSON_AddNumberToObject(json, "resultado", -2); // Si el resultado es -2, ha habido un error ya que no se ha tocado 
    
    char *json_str = cJSON_Print(json);

    // Enviar la petición al servidor y recibir la respuesta
    char *response = send_request(json_str);

    cJSON_Delete(json);
    free(json_str);
    
    if (!response) {
        return -1; // Error en la comunicación con el servidor
    }

    // Parsear la respuesta JSON para obtener el índice "resultado"
    cJSON *response_json = cJSON_Parse(response);
    if (!response_json) {
        return -1; // Error al parsear la respuesta
    }

    int result = cJSON_GetObjectItem(response_json, "resultado") -> valueint;
    cJSON_Delete(response_json);

    return result; // Retornar el valor de "resultado"
}

// Implementación de exist utilizando JSON
int exist(int key) {
    cJSON *json = cJSON_CreateObject();
    cJSON_AddNumberToObject(json, "operation", 5);
    cJSON_AddNumberToObject(json, "key", key);
    cJSON_AddNumberToObject(json, "resultado", -2); // Si el resultado es -2, ha habido un error ya que no se ha tocado 
    
    char *json_str = cJSON_Print(json);

    // Enviar la petición al servidor y recibir la respuesta
    char *response = send_request(json_str);

    cJSON_Delete(json);
    free(json_str);
    
    if (!response) {
        return -1; // Error en la comunicación con el servidor
    }

    // Parsear la respuesta JSON para obtener el índice "resultado"
    cJSON *response_json = cJSON_Parse(response);
    if (!response_json) {
        return -1; // Error al parsear la respuesta
    }

    int result = cJSON_GetObjectItem(response_json, "resultado") -> valueint;
    cJSON_Delete(response_json);

    return result; // Retornar el valor de "resultado"
}

// Implementación de destroy utilizando JSON   
int destroy() {
    cJSON *json = cJSON_CreateObject();
    cJSON_AddNumberToObject(json, "operation", 6);
    cJSON_AddNumberToObject(json, "resultado", -2); // Si el resultado es -2, ha habido un error ya que no se ha tocado 
    
    char *json_str = cJSON_Print(json);

    // Enviar la petición al servidor y recibir la respuesta
    char *response = send_request(json_str);


    cJSON_Delete(json);
    free(json_str);
    
    if (!response) {
        return -2; // Error en la comunicación con el servidor
    }

    // Parsear la respuesta JSON para obtener el índice "resultado"
    cJSON *response_json = cJSON_Parse(response);

    if (!response_json) {
        return -1; // Error al parsear la respuesta
    }

    int result = cJSON_GetObjectItem(response_json, "resultado") -> valueint;
    cJSON_Delete(response_json);

    return result; // Retornar el valor de "resultado"
}

