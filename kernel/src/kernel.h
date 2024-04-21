#ifndef KERNEL_H_
#define KERNEL_H_

/*---------LIBRERIAS---------*/

#include "../../utils/src/config/configs.h"
#include "../../utils/src/sockets/sockets.h"
#include "../../utils/src/hilos/hilos.h"
#include <commons/string.h>
#include <readline/readline.h>

/*---------DEFINES---------*/

#define rutaConfiguracion "../kernel.config"

/*---------DATOS DE LA CONFIGURACION---------*/

typedef enum {
    EJECUTAR_SCRIPT,
    INICIAR_PROCESO,
    FINALIZAR_PROCESO,
    DETENER_PLANIFICACION,
    INICIAR_PLANIFICACION,
    MULTIPROGRAMACION,
    PROCESO_ESTADO,
} comando_consola;

/*---------DATOS DE LA CONFIGURACION---------*/

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

/*---------ESTRUCTURAS PARA INFORMACION---------*/

// Estructuras para informacion
t_config* config;
t_log* logs_auxiliares;
t_log* logs_obligatorios;
t_log* logs_error;

/*---------FILE DESCRIPTORS CONEXIONES---------*/

int fd_memoria;
int fd_cpu_dispatch;
int fd_cpu_interrupt;
int socket_servidor;

/*---------VARIABLES---------*/

int pid_siguiente = 1;

/*---------HILOS---------*/

pthread_t thread_cpu_dispatch;
pthread_t thread_cpu_interrupt;
pthread_t thread_memoria;
pthread_t thread_consola_interactiva;

/*---------FUNCIONES---------*/
// Inicializa las variables
void inicializarVariables();
// Inicializa la consola interactiva
void iniciarConsolaInteractiva();
// Atiende las peticiones de la consola interactiva
void atender_consola_interactiva();
// Ejecuta el comando correspondiente
void ejecutar_comando_consola(char** arrayComando);
// Devuelve el comando del enum correspondiente
comando_consola transformarAOperacion(char* operacionLeida);
// Atiende al cliente
void atender_cliente(void* argumentoVoid);
// Escucha el socket por peticiones
bool escucharServer(int socket_servidor);
// Envia mensaje inicial 
void enviar_handshake();
// Crea los logs obligatorios y auxiliares
void crearLogs();
// Crea los sockets y se conecta hacia los otros modulos
bool crearConexiones();
// Inicializa la config y lee los datos con leerConfig()
void iniciarConfig();
// Lee la configuracion y lo carga a las variables correspondientes
void leerConfig(); 
// Convierte un array de string a un array de enteros
int* string_array_as_int_array(char** arrayInstancias);
// Libera los espacios de memoria
void terminarPrograma();

#endif