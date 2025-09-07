#include "real.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

//Creamos el mutex para que la lista este protegida
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;


typedef struct Clave {
    int key;  //Valor que se usa como identificador de la clave.
    char value1[256];  //Cadena de caracteres, que tiene reservado 256(value1).
    int N_value2;  //Número de elementos del vector V_value2 (1-32).
    double V_value2[32];  //Vector de doubles (V_value2).
    struct Coord value3;  //Estructura Coord (x,y).
    struct Clave *next;  //Puntero al siguiente nodo de la lista.
} Clave;

Clave *head = NULL;

int _destroy(){
    pthread_mutex_lock(&mutex);
    if (!head) {
        printf("Error: La lista está vacía\n");
        pthread_mutex_unlock(&mutex);
        return -1; //Si la lista está vacía, devolvemos -1. 
    }

    while (head)
    {
       Clave *clave_actual = head;
       if (!clave_actual) {
            printf("Error: No se ha podido eliminar la clave\n");
            pthread_mutex_unlock(&mutex);
            return -1;
       }
       head = head->next;
       free(clave_actual);
    }

    pthread_mutex_unlock(&mutex);
    return 0;
}

int _set_value(int key, char *value1, int N_value2, double *V_value2, struct Coord value3) {  
    
    pthread_mutex_lock(&mutex);

    if (N_value2 < 1 || N_value2 > 32) {
        pthread_mutex_unlock(&mutex);
        return -1;
    }
        
        // Comprobamos si la clave ya existe, en ese caso se devuelve -1
        Clave *clave_actual = head;
        while (clave_actual) {
            if (clave_actual->key == key) {
                printf("Error: La clave %d ya existe. No se puede insertar.\n", key);
                pthread_mutex_unlock(&mutex);
                return -1;
            }
            clave_actual = clave_actual->next;
        }

    
        // Crear una nueva clave añadiendo los valores necesarios
        Clave *nueva_clave = (Clave *)malloc(sizeof(Clave));
        if (!nueva_clave) {
            pthread_mutex_unlock(&mutex);
            return -1;  // No hay memoria disponible para crear la nueva clave, devolvemos -1
        }
        nueva_clave->key = key;

        if (strlen(value1) > 255){
            pthread_mutex_unlock(&mutex);
            return -1;  // La longitud de value1 es mayor de 255, devolvemos -1
        }

        strcpy(nueva_clave->value1, value1);
        nueva_clave->N_value2 = N_value2;
        memcpy(nueva_clave->V_value2, V_value2, N_value2 * sizeof(double));

        nueva_clave->value3 = value3;

        // Actualizamos la lista de claves, añadiendo a dónde apunta la nueva clave, y definiendo la nueva clave como la cabeza de la lista
        nueva_clave->next = head;
        head = nueva_clave;
        
        pthread_mutex_unlock(&mutex);
        return 0;
    }

int _get_value(int key, char *value1, int *N_value2, double *V_value2, struct Coord *value3) {

    pthread_mutex_lock(&mutex);
    Clave *clave_actual = head;
    while (clave_actual) {
        if (clave_actual->key == key) {

            // Copiamos los valores de la clave actual a los parámetros de salida
            strcpy(value1, clave_actual->value1); 
            *N_value2 = clave_actual->N_value2;
            memcpy(V_value2, clave_actual->V_value2, *N_value2 * sizeof(double)); 
            *value3 = clave_actual->value3;
            pthread_mutex_unlock(&mutex);
            return 0;
        }

        // Avanzamos al siguiente nodo
        clave_actual = clave_actual->next;
    }

    //No se ha encontrado la clave con el valor key
    pthread_mutex_unlock(&mutex);
    return -1;
}

int _modify_value(int key, char *value1, int N_value2, double *V_value2, struct Coord value3) {

    pthread_mutex_lock(&mutex);
    if (N_value2 < 1 || N_value2 > 32){
        pthread_mutex_unlock(&mutex);
        return -1;
    } 

    Clave *clave_actual = head;
    while (clave_actual) {
        if (clave_actual->key == key) {

            // Modificamos los valores de la clave actual
            if (strlen(value1) > 255) {
                pthread_mutex_unlock(&mutex);
                return -1;  // La longitud de value1 es mayor de 255, devolvemos -1
            }
            strcpy(clave_actual->value1, value1);

            clave_actual->N_value2 = N_value2;
            memcpy(clave_actual->V_value2, V_value2, N_value2 * sizeof(double));

            clave_actual->value3 = value3;
            pthread_mutex_unlock(&mutex);
            return 0;
        }

        // Avanzamos al siguiente nodo
        clave_actual = clave_actual->next;
    }

    //No se ha encontrado la clave con el valor key
    pthread_mutex_unlock(&mutex);
    return -1;
}

int _delete_key(int key) {

    pthread_mutex_lock(&mutex);
    Clave *clave_actual = head;
    Clave *clave_anterior = NULL;
    while (clave_actual) {
        if (clave_actual->key == key) {

            // Eliminamos la clave actual
            if (clave_anterior) {
                clave_anterior->next = clave_actual->next;
            } 
            
            //Si no hay clave anterior, la clave actual es la cabeza de la lista; por lo tanto, la cabeza de la lista pasa a ser la siguiente clave
            else {
                head = clave_actual->next;
            }

            // Liberamos la memoria de la clave actual
            free(clave_actual);
            pthread_mutex_unlock(&mutex);
            return 0;
        }

        // Avanzamos al siguiente nodo
        clave_anterior = clave_actual;
        clave_actual = clave_actual->next;
    }

    //No se ha encontrado la clave con el valor key
    pthread_mutex_unlock(&mutex);
    return -1;
}

int _exist(int key) {

    pthread_mutex_lock(&mutex);
    Clave *clave_actual = head;
    while (clave_actual) {
        if (clave_actual->key == key) {
            pthread_mutex_unlock(&mutex);
            return 1;
        }

        // Avanzamos al siguiente nodo
        clave_actual = clave_actual->next;
    }

    //No se ha encontrado la clave con el valor key
    pthread_mutex_unlock(&mutex);
    return 0;
}
