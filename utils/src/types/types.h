#ifndef TYPES_H_
#define TYPES_H_

#include "../config/configs.h"

/* ------------ ENUMS --------*/

// Codigos de operaciones
typedef enum {
    MENSAJE,
    PAQUETE,
	CONTEXTO_EJECUCION,
	CREAR_PCB,
	ELIMINAR_PCB,
	FETCH_INSTRUCCION,
	DEVOLVER_INSTRUCCION,
	OK_OPERACION,
	ERROR_OPERACION,
	INTERRUPCION,
} op_codigo;

// Razones de bloqueo
typedef enum{
	UNKNOWN,
	INTERRUPCION_RELOJ,
	INTERRUPCION_FIN_EVENTO,
	LLAMADA_SISTEMA,
} blocked_reason;

// Estados de los procesos
typedef enum{
	NEW,
	READY,
	EXEC,
	BLOCKED,
	EXIT,
} process_state;

/* ------------ STRUCTS --------*/

// t_buffer para enviar y recibir mensajes
typedef struct{
	int size;
	void* stream;
} t_buffer;
// Formato del paquete
typedef struct{
	op_codigo codigo_operacion;
	t_buffer* buffer;
} t_paquete;
// Registros de la CPU
typedef struct{
	uint32_t pc; // program counter es la siguiente instruccion a ejecutar
	uint8_t ax;
	uint8_t bx;
	uint8_t cx;
	uint8_t dx;
	uint32_t eax;
	uint32_t ebx;
	uint32_t ecx;
	uint32_t edx;
	uint32_t si;
	uint32_t di;
} t_registros_cpu;

// Punteros a memoria
// typedef struct{
// 	uint64_t stack_pointer; // puntero a pila del sistema
// 	uint64_t heap_pointer; // puntero a memoria dinamica
// 	uint64_t data_pointer; // puntero a memoria estatica
// 	uint64_t code_pointer; // puntero a instrucciones propias del programa
// } t_punteros_memoria;
// // Contexto de ejecucion
typedef struct{
	uint32_t pid; // process ID
	uint64_t registro_estados; // registros con los flags
	t_registros_cpu registros_cpu;
	// t_punteros_memoria punteros_memoria;
	process_state state;
	blocked_reason motivo_bloqueo;
} t_contexto_ejecucion;
// PCB
typedef struct{
	uint64_t quantum_faltante; // duracion restante del tiempo de ejecucion
	int io_identifier; // identificador de la entrada/salida correspondiente
	t_contexto_ejecucion contexto_ejecucion;
	char* path_archivo; // archivo con las instrucciones a ejecutar
} t_pcb;

#endif /* TYPES_VARS_H */