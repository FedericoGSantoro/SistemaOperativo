#ifndef TYPES_H_
#define TYPES_H_

#include "../config/configs.h"


//Ciclo de instrucciones

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
    EXIT
} t_nombre_instruccion;

typedef struct
{
    t_nombre_instruccion nombre_instruccion;
    int cant_parametros;
    int p_length[4]; // 4 * 4
    t_list* parametros;
} t_instruccion;

#endif /* TYPES_VARS_H */
