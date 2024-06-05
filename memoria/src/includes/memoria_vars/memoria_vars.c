#include "./memoria_vars.h"

//cache para almacenar queue de instrucciones
t_dictionary* cache_instrucciones;

//cache para almacenar tablas por procesos
t_dictionary* cache_tabla_por_proceso;

//Globales del Config
t_mem_config memConfig;

//Global para espacio de usuario
t_espacio_usuario espacio_usuario;

int *vector_marcos;

//Variables Globales
t_log* loggerOblig; 
t_log* loggerAux;
t_log* loggerError;
t_config* config;
int socketFdMemoria;

//Threads memoria server
pthread_t thr_server;
pthread_t thr_server_conn;

//Semaforo
sem_t sem_retardo;