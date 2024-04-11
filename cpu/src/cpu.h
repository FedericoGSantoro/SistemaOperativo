#ifndef CPU_H_
#define CPU_H_

#include <stdlib.h>
#include <stdio.h>
#include "../../utils/src/sockets/sockets.h"
#include <commons/log.h>
#include <commons/config.h>
#include "../../utils/src/config/configs.h"

#define rutaConfiguracionCpu "/home/utnso/Desktop/TP-SO/tp-2024-1c-Grupo-AFFOM/cpu/cpu.config"

t_log* cpu_logger;
t_config* configuracion;

char* IP_MEMORIA;
int PUERTO_MEMORIA;
int PUERTO_ESCUCHA_DISPATCH;
int PUERTO_ESCUCHA_INTERRUPT;
int CANTIDAD_ENTRADAS_TLB;
char* ALGORITMO_TLB;

void leerConfig();

#endif