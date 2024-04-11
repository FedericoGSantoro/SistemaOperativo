#
#include <sockets/sockets.h>
#include <commons/log.h>
#include <commons/config.h>
#include "../../utils/src/config/config.h"
#include "../../utils/src/sockets/sockets.h"

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
t_config* config;

//-------Funciones de inicialización---------
void leer_config();
int server_escuchar(int fd_memoria);
void gestionar_conexion(/*unaOperacion? Handshake? ver */);
void terminar_programa();