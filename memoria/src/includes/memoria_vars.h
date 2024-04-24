#ifndef MEMORIA_VARS_H_
#define MEMORIA_VARS_H_

#define rutaConfiguracion "../memoria.config"

#include "../../utils/src/config/configs.h"

//Globales del Config
typedef struct {
    char* puertoEscucha;
    int tamMemoria;
    int tamPagina;
    int retardoRespuesta;
    char* pathInstrucciones;
} t_mem_config;

t_mem_config memConfig;

//Variables Globales
t_log* loggerOblig; 
t_log* loggerAux;
t_log* loggerError;
t_config* config;
int socketFdMemoria;

//Threads memoria server
pthread_t thr_server;
pthread_t thr_server_conn;

#endif /* MEMORIA_VARS_H */