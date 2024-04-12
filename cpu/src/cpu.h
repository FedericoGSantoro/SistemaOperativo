#ifndef CPU_H_
#define CPU_H_

#include <stdlib.h>
#include <stdio.h>
#include <commons/log.h>
#include <commons/config.h>
#include "../../utils/src/sockets/sockets.h"
#include "../../utils/src/config/configs.h"

#define rutaConfiguracionCpu "../cpu.config"

t_log* logger_obligatorio_cpu;
t_log* logger_aux_cpu;
t_config* configuracion_cpu;

char* IP_MEMORIA;
int PUERTO_MEMORIA;
int PUERTO_ESCUCHA_DISPATCH;
int PUERTO_ESCUCHA_INTERRUPT;
int CANTIDAD_ENTRADAS_TLB;
char* ALGORITMO_TLB;

void iniciarLogs();
void iniciarConfig();
void leerConfig();
void terminarPrograma();

#endif