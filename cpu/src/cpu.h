#ifndef CPU_H_
#define CPU_H_

#include "../../utils/src/sockets/sockets.h"
#include "../../utils/src/config/configs.h"
#include "../../utils/src/hilos/hilos.h"

#define rutaConfiguracionCpu "../cpu.config"

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

// registros de la cpu
uint64_t instruction_pointer;
uint64_t registro_estados;
t_registros_cpu registros_cpu;
t_punteros_memoria punteros_memoria;
process_state state;

// Inicializamos logs
void iniciarLogs();
// Inicializamos configuracion
void iniciarConfig();
// Leemos los valores del archivo de configuracion y los almacenamos en las variables segun el tipo de dato
void leerConfig();

// Iniciamos servidor de cpu modo dispatch y servidor cpu modo interrupt
void iniciarServidoresCpu();
// Nos conectamos como cliente a Memoria
void iniciarConexionCpuMemoria();
// Esperaramos conexion del cliente kernel modo dispatch y modo interrupt
bool esperarClientes();

/* 
Las funciones atenderKernelDispatch, atenderKernelInterrup y atenderMemoria dentro ejecutan un while infinito,
es por eso que necesitamos crear 3 hilos. Cada uno de los hilos ejecutara cada uno de estos procesos como si
fuera en paralelo para que no se bloqueen entre si.
*/
// Atendemos al cliente Kernel modo Dispatch, recibimos mensajes ejecutamos ciclos de instrucciones y podemos enviar el contexto de ejecucion a Kernel
void atenderKernelDispatch();
// Atendemos al cliente Kernel modo Interrupt, UNICAMENTE recibimos mensajes, NO enviamos nada a Kernel
void atenderKernelInterrupt();
// Atendemos al server Memoria, recibimos mensajes
void atenderMemoria();
// Envia mensaje a memoria
void enviarMsjMemoria();

// Guardo en los registros del cpu lo que recibí en el contexto de ejecucion
void desempaquetar_contexto_ejecucion(t_list* paquete);
// Recibo el contexto de ejecucion que me manda Kernel
void recv_contexto_ejecucion(int fd_kernel_dispatch);

// Liberaramos espacio de memoria
void terminarPrograma();

#endif