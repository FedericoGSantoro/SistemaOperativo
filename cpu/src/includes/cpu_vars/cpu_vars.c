#include "./cpu_vars.h"

t_log* logger_obligatorio_cpu;
t_log* logger_aux_cpu;
t_log* logger_error_cpu;
t_config* configuracion_cpu;

//datos de la configuracion
char* IP_MEMORIA;
char* PUERTO_MEMORIA;
char* PUERTO_ESCUCHA_DISPATCH;
char* PUERTO_ESCUCHA_INTERRUPT;
int CANTIDAD_ENTRADAS_TLB;
char* ALGORITMO_TLB;

// file descriptor para la conexion con kernel y memoria
int fd_cpu_dispatch;
int fd_cpu_interrupt;
int fd_kernel_dispatch;
int fd_kernel_interrupt;
int fd_memoria;

// hilos
pthread_t hilo_kernel_dispatch_cpu;
pthread_t hilo_kernel_interrumpt_cpu;
pthread_t hilo_memoria_cpu;

// Contexto de ejecucion
uint32_t pid;
uint64_t registro_estados;
t_registros_cpu registros_cpu;
t_punteros_memoria punteros_memoria;
process_state state;
blocked_reason motivo_bloqueo;
// instruction register (IR) almacena la instruccion actual que se está ejecutando o que está por ejecutarse
char* ir;

// Mutex
pthread_mutex_t variableInterrupcion;
bool hayInterrupcion = false;