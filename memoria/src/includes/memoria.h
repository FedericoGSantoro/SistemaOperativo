#ifndef MEMORIA_H_
#define MEMORIA_H_

/*---------LIBRERIAS---------*/

#include "../../utils/src/sockets/sockets.h"
#include "../../utils/src/hilos/hilos.h"
#include "./instruction_fetcher/instruction_fetcher.h"
#include "./memoria_vars/memoria_vars.h"
#include "../../../utils/src/liberador/liberador.h"


#define rutaConfiguracion "memoria.config"

/*---------FUNCIONES---------*/
void leer_config();
int server_escuchar(int fd_memoria);
void gestionar_conexion(void * puntero_fd_cliente);
void terminar_programa();
void inicializar_loggers();
void inicializar_config();

#endif