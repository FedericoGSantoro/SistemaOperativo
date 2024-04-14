#ifndef KERNEL_H_
#define KERNEL_H_

/*---------LIBRERIAS---------*/

#include <stdlib.h>
#include <stdio.h>
#include "../../utils/src/config/configs.h"
#include "../../utils/src/sockets/sockets.h"
#include "../../utils/src/hilos/hilos.h"
#include <commons/config.h>
#include <commons/string.h>

/*---------DEFINES---------*/

#define rutaConfiguracion "../kernel.config"

/*---------DATOS DE LA CONFIGURACION---------*/

char* PUERTO_ESCUCHA;
char* IP_MEMORIA;
char* PUERTO_MEMORIA;
char* IP_CPU;
char* PUERTO_CPU_DISPATCH;
char* PUERTO_CPU_INTERRUPT;
char* ALGORITMO_PLANIFICACION;
int QUANTUM;
char** RECURSOS;
int* INSTANCIAS_RECURSOS;
int GRADO_MULTIPROGRAMACION;

/*---------ESTRUCTURAS PARA INFORMACION---------*/

// Estructuras para informacion
t_config* config;
t_log* logs_auxiliares;
t_log* logs_obligatorios;
t_log* logs_error;

/*---------FILE DESCRIPTORS CONEXIONES---------*/

int fd_memoria;
int fd_cpu_dispatch;
int fd_cpu_interrupt;
int socket_servidor;

/*---------VARIABLES---------*/



/*---------HILOS---------*/

pthread_t thread_cpu_dispatch;
pthread_t thread_cpu_interrupt;
pthread_t thread_memoria;

/*---------FUNCIONES---------*/
// Inicializa las variables
void inicializarVariables();
// Atiende al cliente
void atender_cliente(void* argumentoVoid);
// Escucha el socket por peticiones
bool escucharServer(int socket_servidor);
// Envia mensaje inicial 
void enviar_handshake();
// Crea los logs obligatorios y auxiliares
void crearLogs();
// Crea los sockets y se conecta hacia los otros modulos
bool crearConexiones();
// Inicializa la config y lee los datos con leerConfig()
void iniciarConfig();
// Lee la configuracion y lo carga a las variables correspondientes
void leerConfig(); 
// Convierte un array de string a un array de enteros
int* string_array_as_int_array(char** arrayInstancias);
// Libera los espacios de memoria
void terminarPrograma(); 

#endif