#ifndef MEMORIA_VARS_H_
#define MEMORIA_VARS_H_
#define rutaConfiguracion "../memoria.config"

#include "../../utils/src/config/configs.h"
#include <commons/collections/dictionary.h>
#include <commons/collections/list.h>

//cache para almacenar lista de instrucciones (key = PID, value = lista de instrucciones)
extern t_dictionary* cache_instrucciones;

//Globales del Config
typedef struct {
    char* puertoEscucha;
    int tamMemoria;
    int tamPagina;
    int retardoRespuesta;
    char* pathInstrucciones;
} t_mem_config;

extern t_mem_config memConfig;

//Variables Globales
extern t_log* loggerOblig; 
extern t_log* loggerAux;
extern t_log* loggerError;
extern t_config* config;
extern int socketFdMemoria;

//Threads memoria server
extern pthread_t thr_server;
extern pthread_t thr_server_conn;

#endif /* MEMORIA_VARS_H */