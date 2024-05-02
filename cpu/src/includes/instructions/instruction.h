#ifndef INSTRUCTION_EXECUTION_H_
#define INSTRUCTION_EXECUTION_H_

#include <commons/collections/list.h>
#include <stdint.h>
#include "../cpu_vars/cpu_vars.h"
#include "../../../../utils/src/liberador/liberador.h"

// Operaciones de Instrucciones de CPU
typedef enum
{
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

// Estructuras de instruccion

typedef struct {
	t_nombre_instruccion nombre_instruccion;
	void (*execute)(int, t_list*); //funcion generica para ejecutar una instruccion con cantidad de parametros y parametros dados
} t_tipo_instruccion;

typedef struct
{
    t_tipo_instruccion tipo_instruccion;
    int cant_parametros;
    t_list* parametros;
} t_instruccion;
extern t_instruccion* instruccion;

t_instruccion* procesar_instruccion(char *instruccion_entrante);
void liberar_instruccion();
void sum(int cantidad_parametros, t_instruccion* instruccion);
void set(int cantidad_parametros, t_instruccion* instruccion);

#endif