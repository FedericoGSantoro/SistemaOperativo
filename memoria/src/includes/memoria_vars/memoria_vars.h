#ifndef MEMORIA_VARS_H_
#define MEMORIA_VARS_H_

#include "../../utils/src/config/configs.h"
#include <commons/collections/dictionary.h>
#include <commons/collections/list.h>
#include <pthread.h>
#include <semaphore.h>
#include <signal.h>


//cache para almacenar lista de instrucciones (key = PID, value = lista de instrucciones)
extern t_dictionary* cache_instrucciones;

//mapa para almacenar tablas por procesos (key = PID, value = lista, la cual sera la tabla de paginas)
extern t_dictionary* tablas_por_proceso;

//Globales del Config
typedef struct {
    char* puertoEscucha;
    int tamMemoria;
    int tamPagina;
    int retardoRespuesta;
    char* pathInstrucciones;
} t_mem_config;

extern t_mem_config memConfig;

//void* para el espacio de usuario, con su semaforo asociado
typedef struct 
{
    void* espacio_usuario;
    pthread_mutex_t mx_espacio_usuario;
} t_espacio_usuario;

extern t_espacio_usuario espacio_usuario;

extern int *vector_marcos;

// estructura para las paginas en la tabla de paginas (la lista del mapa de tablas por proceso, contiene por cada objeto, una estructura de estas)
typedef struct{
    int marco;
    bool presencia;
    int modificado;
    int pos_en_swap;
    time_t tiempo_carga;//Para FIFO
    time_t ultima_referencia;//Para LRU
    int PID;
    int indice_pag;
} t_pagina;

//Variables Globales
extern t_log* loggerOblig; 
extern t_log* loggerAux;
extern t_log* loggerError;
extern t_config* config;
extern int socketFdMemoria;

//Threads memoria server
extern pthread_t thr_server;
extern pthread_t thr_server_conn;

//Semaforos
extern sem_t sem_retardo;

#endif /* MEMORIA_VARS_H */