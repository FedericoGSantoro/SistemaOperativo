#ifndef INSTRUCTION_EXECUTION_H_
#define INSTRUCTION_EXECUTION_H_

#include <commons/collections/list.h>
#include <stdint.h>
#include "../cpu_vars/cpu_vars.h"
#include "../../../../utils/src/liberador/liberador.h"
#include "../../../../utils/src/castingfunctions/castfunctions.h"

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
	void (*execute)(t_list*); //funcion generica para ejecutar una instruccion con parametros dados
} t_tipo_instruccion;

typedef struct
{
    t_tipo_instruccion tipo_instruccion;
    int cant_parametros;
    t_list* parametros;
    char* parametros_string;
} t_instruccion;
extern t_instruccion* instruccion;

uint32_t* mapear_registro(char *nombre_registro);
void liberar_instruccion();
void sum_instruction(t_list* parametros);
void sub_instruction(t_list* parametros);
void set_instruction(t_list* parametros);
void jnz_instruction(t_list* parametros);
void io_gen_sleep_instruction(t_list* parametros);
void exit_instruction(t_list* parametros);
t_tipo_instruccion mapear_tipo_instruccion(char *nombre_instruccion);
t_instruccion *new_instruction(t_tipo_instruccion tipo_instruccion, t_list *parametros);
t_instruccion* procesar_instruccion(char *instruccion_entrante);
void liberar_instruccion();

#endif