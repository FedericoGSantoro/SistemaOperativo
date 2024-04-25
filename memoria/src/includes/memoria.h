#ifndef MEMORIA_H_
#define MEMORIA_H_

/*---------LIBRERIAS---------*/

#include "./client_handler/client_handler.h"
#include "../../utils/src/hilos/hilos.h"
#include "./memoria_vars.h"

/*---------FUNCIONES---------*/
void leer_config();
int server_escuchar(int fd_memoria);
void gestionar_conexion(void * puntero_fd_cliente);
void terminar_programa();
void inicializar_loggers();
void inicializar_config();


#endif