#ifndef KERNEL_H_
#define KERNEL_H_

/*---------LIBRERIAS---------*/

#include "../../utils/src/config/configs.h"
#include "../../utils/src/sockets/sockets.h"
#include "../../utils/src/hilos/hilos.h"
#include <commons/string.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <commons/collections/queue.h>
#include <pthread.h>
#include <semaphore.h>

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

typedef enum {
    FIFO,
    RR,
    VRR,
} alg_planificacion;

/*---------DATOS DE LA CONFIGURACION---------*/

char* PUERTO_ESCUCHA;
char* IP_MEMORIA;
char* PUERTO_MEMORIA;
char* IP_CPU;
char* PUERTO_CPU_DISPATCH;
char* PUERTO_CPU_INTERRUPT;
alg_planificacion ALGORITMO_PLANIFICACION;
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
comando_consola comando;
bool planificacionEjecutandose = true;
char* pathArchivo;
int numeroConsola = 1;

/*---------COLAS---------*/

t_queue* cola_new;
t_queue* cola_ready;
t_queue* cola_exit;
t_queue* cola_exec;
t_queue* cola_blocked;
t_queue* cola_ready_aux;

/*---------HILOS---------*/

pthread_t thread_cpu_dispatch;
pthread_t thread_cpu_interrupt;
pthread_t thread_memoria;
pthread_t thread_consola_interactiva;

/*---------SEMAFOROS---------*/

//pthread_mutex_t sem_gradoMultiprogramacion;
pthread_mutex_t sem_planificacion;
pthread_cond_t condicion_planificacion;
pthread_mutex_t sem_cola_new;
pthread_mutex_t sem_cola_ready;
pthread_mutex_t sem_cola_exec;
pthread_mutex_t sem_cola_blocked;
pthread_mutex_t sem_cola_exit;
sem_t semContadorColaNew;
sem_t semContadorColaReady;
sem_t semContadorColaExec;
sem_t semContadorColaBlocked;
sem_t semContadorColaExit;
sem_t semGradoMultiprogramacion;

/*---------FUNCIONES---------*/

// Inicializa la planificacion de kernel
void iniciarPlanificacion();
// Inicializa la planificacion a corto plazo
void planificacionCortoPlazo();
// Algoritmo para cola de blocked
void corto_plazo_blocked();
// Carga el contexto actual del pcb por el recibido
void cargar_contexto_recibido(t_list* contexto, t_pcb* pcb);
// Quita el primer PCB de la cola indicada
t_pcb* quitarPcbCola(t_queue* cola, pthread_mutex_t semaforo);
// Agrega el PCB a la cola indicada
void agregarPcbCola(t_queue* cola, pthread_mutex_t semaforo, t_pcb* pcb);
// Cambia el contexto del pcb con el recibido y lo asigna a la cola correspondiente
void cambiarContexto(t_list* contexto, t_pcb* pcb);
// Empaqueta los registros de la cpu del contexto para enviarlos
void empaquetar_registros_cpu(t_paquete* paquete, t_pcb* pcb);
// Empaqueta los punteros de memoria para enviarlos
void empaquetar_punteros_memoria(t_paquete* paquete, t_pcb* pcb);
// Empaqueta el contexto de ejecucion para enviarlo
void empaquetar_contexto_ejecucion(t_paquete* paquete, t_pcb* pcb);
// Maneja la conexion con el dispatch de CPU
void mensaje_cpu_dispatch(op_codigo codigoOperacion, t_pcb* pcb);
// Convierte el enum de estado a un string
char* enumEstadoAString(process_state estado);
// Cambia el estado y hace el log
void cambiarEstado(process_state estadoNuevo, t_pcb* pcb);
// Algoritmo para la cola de READY
void corto_plazo_ready();
// Inicializa la planificacion a largo plazo
void planificacionLargoPlazo();
// Algoritmo para la cola de NEW
void largo_plazo_new();
// Algoritmo para la cola de EXIT
void largo_plazo_exit();
// Elimina el pcb en memoria
void eliminar_pcb(t_pcb* pcb);
// Crea el pcb
void crear_pcb();
// Inicializa los punteros a memoria
void iniciarPunterosMemoria(t_pcb* pcb);
// Inicia registros de cpu en 0
void iniciarRegistrosCPU(t_pcb* pcb);
// Asigna los punteros a memoria al pcb
void asignar_punteros_memoria(t_list* punteros, t_pcb* pcb);
// Maneja la conexion con memoria
void mensaje_memoria(op_codigo comandoMemoria, t_pcb* pcb);
// Inicializa las colas
void inicializarColas();
// Inicializa los semaforos
void inicializarSemaforos();
// Inicializa las variables
void inicializarVariables();
// Inicializa la consola interactiva
void iniciarConsolaInteractiva();
// Atiende las peticiones de la consola interactiva
void atender_consola_interactiva();
// Obtiene los pids de la cola
char* obtenerPids (t_queue* cola, pthread_mutex_t semaforo);
// Ejecuta el comando correspondiente
void ejecutar_comando_consola(char** arrayComando);
// Devuelve el comando del enum correspondiente
comando_consola transformarAOperacion(char* operacionLeida);
// Atiende al cliente
void atender_cliente(void* argumentoVoid);
// Itera el paquete y lo muestra por pantalla
void iteradorPaquete(char* value);
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
// Obtiene el algoritmo de la configuracion
alg_planificacion obtenerAlgoritmo();
// Lee la configuracion y lo carga a las variables correspondientes
void leerConfig(); 
// Convierte un array de string a un array de enteros
int* string_array_as_int_array(char** arrayInstancias);
// Libera los espacios de memoria
void terminarPrograma();

#endif