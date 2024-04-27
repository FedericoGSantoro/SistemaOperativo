#include "./memoria_vars.h"

//cache para almacenar queue de instrucciones
t_dictionary* cache_instrucciones;

//Globales del Config
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
