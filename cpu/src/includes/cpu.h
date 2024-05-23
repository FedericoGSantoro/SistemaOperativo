#ifndef CPU_H_
#define CPU_H_

#include "./cpu_vars/cpu_vars.h"
#include "./includes/instructions_cycle/instruction_cycle.h"

#define rutaConfiguracionCpu "cpu.config"

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
// Atendemos al cliente Kernel modo Dispatch, recibimos mensajes ejecutamos ciclo de instruccion y enviamos el contexto de ejecucion a Kernel
void atenderKernelDispatch();
// Atendemos al cliente Kernel modo Interrupt, UNICAMENTE recibimos mensajes, NO enviamos nada a Kernel en modo interrumpt
void atenderKernelInterrupt();
// Atendemos al server Memoria
void atenderMemoria(op_codigo codigoMemoria);
// Envia mensaje a memoria
void enviarMsjMemoria();
void iteradorPaquete(char* value);

// Desempaquetamos la interrupcion de kernel y validamos que el pid que nos envia sea igual al pid que estamos ejecutando
bool desempaquetarInterrupcion(t_list *paquete);
// Recibimos interrupcion de kernel, nos envía el pid
void recvInterrupcion();

//
void agregar_io_detail(t_paquete *paquete);
//
void eliminar_io_detail();
// Guardo en los registros del cpu lo que recibí en el contexto de ejecucion
void desempaquetarContextoEjecucion(t_list* paquete);
// Recibo el contexto de ejecucion que me manda Kernel
void recvContextoEjecucion();
// Empaqueto el contexto de ejecucion
void empaquetarContextoEjecucion(t_paquete* paquete);
// Envio contexto de ejecucion
void enviarContextoEjecucion();

// Ciclo de ejecucion de una instruccion
void fetch();
void decode();
void execute();

// Ejecutamos ciclo de instruccion
void ejecutarCicloInstruccion();
// Iniciamos ciclo de instruccion
void iniciar_ciclo_instruccion();

// Liberaramos espacio de memoria
void terminarPrograma();

#endif