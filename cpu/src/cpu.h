#ifndef CPU_H_
#define CPU_H_

#include <stdlib.h>
#include <stdio.h>
#include <commons/log.h>
#include <commons/config.h>
#include <pthread.h>
#include "../../utils/src/sockets/sockets.h"
#include "../../utils/src/config/configs.h"

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
pthread_t hilo_cpu_memoria;
pthread_t hilo_memoria_cpu;

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

// Atendemos al cliente Kernel modo Dispatch, recibimos mensajes
void atenderKernelDispatch();
// Atendemos al cliente Kernel modo Interrupt, recibimos mensajes
void atenderKernelInterrupt();
// Atendemos al server Memoria, recibimos mensajes
void atenderMemoria();

/* 
Las funciones atenderKernelDispatch, atenderKernelInterrup y atenderMemoria dentro ejecutan un while infinito,
es por eso que necesitamos crear 3 hilos. Cada uno de los hilos ejecutara cada uno de estos procesos como si
fuera en paralelo para que no se bloqueen entre si.
*/
void crearHiloKernelDispatch();
void crearHiloKernelInterrupt();
void crearHilosMemoria();

// Liberaramos espacio de memoria
void terminarPrograma();

#endif