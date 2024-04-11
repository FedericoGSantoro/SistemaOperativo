#
#include <sockets/sockets.h>
#include <commons/log.h>
#include <commons/config.h>
#include "../../utils/src/config/config.h"
#include "../../utils/src/sockets/sockets.h"

//GLobales del Config
char* IP;
char* PUERTO_ESCUCHA;
int TAM_MEMORIA;
int TAM_PAGINA;
int RETARDO_RESPUESTA;
//char* PATH_INSTRUCCIONES;

//VAriables Globales
t_log* logger; //logger obligatorio, ogligatorio
t_config* config;
int fd_memoria;
int fd_kernel;
int fd_cpu;
//los fd de las entradas salidas seran dinámicos?

//-------Funciones de inicialización---------
void leer_config();
void terminar_programa();
