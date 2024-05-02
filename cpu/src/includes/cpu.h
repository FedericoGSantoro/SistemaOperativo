#ifndef CPU_H_
#define CPU_H_

#include "../../utils/src/sockets/sockets.h"
#include "../../utils/src/config/configs.h"
#include "../../utils/src/hilos/hilos.h"
#include "./includes/instructions/instruction.h"

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

// Inicializamos logs
void iniciarLogs();
// Inicializamos configuracion
void iniciarConfig();
// Leemos los valores del archivo de configuracion y los almacenamos en las variables segun el tipo de dato
void leerConfig();

// Iniciamos mutex
void iniciarMutex();

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
// Atendemos al cliente Kernel modo Interrupt, UNICAMENTE recibimos mensajes, NO enviamos nada a Kernel en modo interrumpt
void atenderKernelInterrupt();
// Atendemos al server Memoria
void atenderMemoria(op_codigo codigoMemoria);
// Envia mensaje a memoria
void enviarMsjMemoria();
void iteradorPaquete(char* value);

// Guardo en los registros del cpu lo que recibí en el contexto de ejecucion
void desempaquetarContextoEjecucion(t_list* paquete);
// Recibo el contexto de ejecucion que me manda Kernel
void recvContextoEjecucion();
// Empaqueto el contexto de ejecucion
void empaquetarContextoEjecucion(t_paquete* paquete);
// Envio contexto de ejecucion
void enviarContextoEjecucion();

// Fetch (captura):
// Se busca la proxima instruccion a ejecutar
// La instruccion a ajecutar se le pide a Memoria utilizando la direccion de memoria del programa (contador de programa) para determinar qué instrucción se debe leer.
void fetch();

// Transformamos el nombre de la instruccion al tipo correspondiente
t_tipo_instruccion mapear_tipo_instruccion(char *nombre_instruccion);
// Asignamos el largo de cada parametro
void add_param_size_to_instruction(t_list *parametros, t_instruccion *instruccion);
// Creamos la estructura t_instruccion
t_instruccion *new_instruction(t_tipo_instruccion tipo_instruccion, t_list *parametros);
// Por cada linea que leemos, obtenemos los tokens y armamos la instruccion con sus parametros y la agregamos a la lista
t_instruccion* process_line(char *line);
// Decode (decodificacion):
// Se interpreta que instrucción es la que se va a ejecutar y si la misma requiere de una traduccion de direccion logica a direccion fisica.
void decode();

// Ejecutar ciclo de instruccion
void ejecutarCicloInstruccion();

// Liberaramos espacio de memoria
void terminarPrograma();

#endif