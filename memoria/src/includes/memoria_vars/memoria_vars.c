#include "./memoria_vars.h"

//cache para almacenar queue de instrucciones
t_dictionary* cache_instrucciones;

//mapa para almacenar tablas por procesos
t_dictionary* tablas_por_proceso;
pthread_mutex_t mx_tablas_paginas;


//Globales del Config
t_mem_config memConfig;

//Global para espacio de usuario
t_espacio_usuario espacio_usuario;

t_list* lista_marcos;
pthread_mutex_t mx_lista_marcos;

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