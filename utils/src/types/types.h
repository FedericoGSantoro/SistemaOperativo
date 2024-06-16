#ifndef TYPES_H_
#define TYPES_H_

#include "../config/configs.h"
#include <stdint.h>

/* ------------ ENUMS --------*/

// Tipo de datos
typedef enum {
    STRING,
	INT,
	UINT32,
	UINT8,
} tipo_de_dato;

// Operaciones de Instrucciones de CPU
typedef enum
{	
	NONE,
    SET,
    SUM,
    SUB,
    MOV_IN,
    MOV_OUT,
    RESIZE,
    JNZ,
    COPY_STRING,
    IO_GEN_SLEEP,
    IO_STDIN_READ,
    IO_STDOUT_WRITE,
    IO_FS_CREATE,
    IO_FS_DELETE,
    IO_FS_TRUNCATE,
    IO_FS_WRITE,
    IO_FS_READ,
    WAIT,
    SIGNAL,
    EXIT_PROGRAM,
} t_nombre_instruccion;

// Codigos de operaciones
typedef enum {
    MENSAJE,
    PAQUETE,
	CONTEXTO_EJECUCION,
	CREAR_PCB,
	ELIMINAR_PCB,
	FETCH_INSTRUCCION,
    WRITE_EN_MEMORIA,
	LEER_VALOR_MEMORIA,
	RESIZE_EN_MEMORIA,
	ESCRIBIR_VALOR_MEMORIA,
	DEVOLVER_INSTRUCCION,
	DEVOLVER_TAM_PAGINA,
    DEVOLVER_MARCO,
	ESCRIBIR_MARCO,
	OK_OPERACION,
	ERROR_OPERACION,
	INTERRUPCION,
	LECTURA,
	ESCRITURA,
	OUT_OF_MEMORY,
} op_codigo;

// Razones de bloqueo
typedef enum{
	NOTHING,
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

typedef struct{
	tipo_de_dato tipo_de_dato;
	void* valor;
} t_params_io;

typedef struct{
	t_list* parametros;
	char* nombre_io;  
	t_nombre_instruccion io_instruccion;  
} t_io_detail;

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
	t_io_detail io_detail;
} t_contexto_ejecucion;
// PCB
typedef struct{
	uint64_t quantum_faltante; // duracion restante del tiempo de ejecucion
	int io_identifier; // identificador de la entrada/salida correspondiente
	t_contexto_ejecucion contexto_ejecucion;
	char* path_archivo; // archivo con las instrucciones a ejecutar
} t_pcb;

typedef struct {
    uint32_t direccion_fisica;
    uint32_t numero_marco;
} t_direcciones_fisicas;

char* mapeo_nombre_instruccion(t_nombre_instruccion nombre_instruccion);

#endif /* TYPES_VARS_H */