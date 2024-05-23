#ifndef CPU_VARS_H_
#define CPU_VARS_H_

#include "../../utils/src/sockets/sockets.h"
#include "../../utils/src/config/configs.h"
#include "../../utils/src/hilos/hilos.h"

extern t_log* logger_obligatorio_cpu;
extern t_log* logger_aux_cpu;
extern t_log* logger_error_cpu;
extern t_config* configuracion_cpu;

//datos de la configuracion
extern char* IP_MEMORIA;
extern char* PUERTO_MEMORIA;
extern char* PUERTO_ESCUCHA_DISPATCH;
extern char* PUERTO_ESCUCHA_INTERRUPT;
extern int CANTIDAD_ENTRADAS_TLB;
extern char* ALGORITMO_TLB;

// file descriptor para la conexion con kernel y memoria
extern int fd_cpu_dispatch;
extern int fd_cpu_interrupt;
extern int fd_kernel_dispatch;
extern int fd_kernel_interrupt;
extern int fd_memoria;

// hilos
extern pthread_t hilo_kernel_dispatch_cpu;
extern pthread_t hilo_kernel_interrumpt_cpu;
extern pthread_t hilo_memoria_cpu;

// Contexto de ejecucion
extern uint32_t pid;
extern uint64_t registro_estados;
extern t_registros_cpu registros_cpu;
extern t_io_detail io_detail;
extern uint32_t valor_registro_numerico;
//extern t_punteros_memoria punteros_memoria;
extern process_state state;
extern blocked_reason motivo_bloqueo;
// instruction register (IR) almacena la instruccion actual que se está ejecutando o que está por ejecutarse
extern char* ir;

// Mutex
extern pthread_mutex_t variableInterrupcion;
extern bool hayInterrupcion;

#endif