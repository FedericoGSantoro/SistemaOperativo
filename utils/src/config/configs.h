#ifndef CONFIGS_H_
#define CONFIGS_H_

#include <stdlib.h>
#include <stdio.h>
#include <commons/config.h>
#include <commons/log.h>

t_config* iniciar_config(char* path, t_log* logger_error, void*(*terminar_programa)());


#endif