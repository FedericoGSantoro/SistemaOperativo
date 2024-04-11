#ifndef KERNEL_H_
#define KERNEL_H_

#include <stdlib.h>
#include <stdio.h>
#include "../../utils/src/config/configs.h"
#include "../../utils/src/sockets/sockets.h"
#include <commons/config.h>
#include <commons/string.h>

#define rutaConfiguracion "../kernel.config"


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
t_config* config;
t_log* logs_informacion;

void leerConfig();
int* string_as_int_array(char** arrayInstancias);

#endif