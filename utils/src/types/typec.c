#include "./types.h"

char* mapeo_nombre_instruccion(t_nombre_instruccion nombre_instruccion) {

    switch (nombre_instruccion) {
        case SET:
            return "SET";
        case SUM:
            return "SUM";
        case SUB:
            return "SUB";
        case MOV_IN:
            return "MOV_IN";
        case MOV_OUT:
            return "MOV_OUT";
        case RESIZE:
            return "RESIZE";
        case JNZ:
            return "JNZ";
        case COPY_STRING:
            return "COPY_STRING";
        case IO_GEN_SLEEP:
            return "IO_GEN_SLEEP";
        case IO_STDIN_READ:
            return "IO_STDIN_READ";
        case IO_STDOUT_WRITE:
            return "IO_STDOUT_WRITE";
        case IO_FS_CREATE:
            return "IO_FS_CREATE";
        case IO_FS_DELETE:
            return "IO_FS_DELETE";
        case IO_FS_TRUNCATE:
            return "IO_FS_TRUNCATE";
        case IO_FS_WRITE:
            return "IO_FS_WRITE";
        case IO_FS_READ:
            return "IO_FS_READ";
        case WAIT:
            return "WAIT";
        case SIGNAL:
            return "SIGNAL";
        case EXIT_PROGRAM:
            return "EXIT";
        default:
            return "NONE";
    }
}
