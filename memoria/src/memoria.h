#ifndef MEMORIA_H_
#define MEMORIA_H_

/*---------LIBRERIAS---------*/
#include <commons/log.h>
#include <commons/config.h>
#include "../../utils/src/config/configs.h"
#include "../../utils/src/sockets/sockets.h"
#include "../../utils/src/hilos/hilos.h"


//GLobales del Config
char* PUERTO_ESCUCHA;
int TAM_MEMORIA;
int TAM_PAGINA;
int RETARDO_RESPUESTA;
char* PATH_INSTRUCCIONES;
int socket_fd_memoria;

//VAriables Globales
t_log* loggerOblig; 
t_log* loggerAux;
t_log* loggerError;
t_config* config;

/*---------FUNCIONES---------*/
void leer_config();
int server_escuchar(int fd_memoria);
void gestionar_conexion(void * puntero_fd_cliente);
void terminar_programa();



#endif