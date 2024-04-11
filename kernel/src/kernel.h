#ifndef KERNEL_H_
#define KERNEL_H_

/*---------LIBRERIAS---------*/

#include <stdlib.h>
#include <stdio.h>
#include "../../utils/src/config/configs.h"
#include "../../utils/src/sockets/sockets.h"
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


/*---------FILE DESCRIPTORS CONEXIONES---------*/

int fd_memoria;
int fd_cpu_dispatch;
int fd_cpu_interrupt;
int fd_escucha;

/*---------FUNCIONES---------*/
// Lee la configuracion y lo carga a las variables correspondientes
void leerConfig(); 
// Crea los sockets y se conecta hacia los otros modulos
bool crearConexiones();
// Convierte un array de string a un array de enteros
int* string_array_as_int_array(char** arrayInstancias); 
// Libera los espacios de memoria
void terminarPrograma(); 

#endif