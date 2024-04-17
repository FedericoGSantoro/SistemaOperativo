#ifndef ENTRADASALIDA_H_
#define ENTRADASALIDA_H_

/*---------LIBRERIAS---------*/

#include "../../utils/src/config/configs.h"
#include "../../utils/src/sockets/sockets.h"
#include "../../utils/src/hilos/hilos.h"
#include <commons/string.h>
#include <readline/readline.h>

/*---------DEFINES---------*/

#define rutaConfiguracion "../entradasalida.config"

/*---------DATOS DE LA CONFIGURACION---------*/

char* TIPO_INTERFAZ;
int TIEMPO_UNIDAD_TRABAJO;
char*IP_KERNEL;
char* PUERTO_KERNEL;
char* IP_MEMORIA;
char* PUERTO_MEMORIA;
char* PATH_BASE_DIALFS;
int BLOCK_SIZE;
int BLOCK_COUNT;

/*---------ESTRUCTURAS PARA INFORMACION---------*/

// Estructuras para informacion
t_log* logger_obligatorio;
t_log* logger_auxiliar;
t_log* logger_error;
t_config* configuracion;

/*---------FILE DESCRIPTORS CONEXIONES---------*/

int fd_memoria;
int fd_kernel;

/*---------VARIABLES---------*/

// Variable para leer la operacion (Actualmente = mensaje, paquete, exit)
char* operacionLeida;
// Variable para leer a quien enviar la operacion
char* enviarA;
// Variable para guardar donde se va a enviar la operacion
int socketAEnviar;
// Puntero a funcion para indicar que debe hacer el hilo
void (*tipoOperacion)();

/*---------HILOS---------*/

pthread_t hilo_memoria;
pthread_t hilo_kernel;

/*---------FUNCIONES---------*/
// Envia un mensaje al socket indicado despues de leer la cadena
void enviarMsj();
// Envia un paquete al socket indicado despues de leer cadenas
void enviarPaquete();
// Determina si enviar la operacion a kernel o memoria
void enviarOperacionA();
// Transforma la operacionLeida en un codigo de operacion para utilizar en un switch por ejemplo
op_codigo transformarAOperacion(char* operacionLeida);

// Inicializa las variables
void inicializar();
void inicializarLogs();
void inicializarConexionKernel();
void inicializarConfig();
void inicializarConexiones();
void inicializarConexionKernel();
void inicializarConexionMemoria();
void leerConfig();
void enviarMsjMemoria();
void enviarMsjKernel();

// Liberaramos espacio de memoria
void terminarPrograma();

#endif