#include "instruction.h"

t_instruccion* instruccion;

void sum(int cantidad_parametros, t_instruccion* instruccion) {

    t_list* parametros = instruccion->parametros;
    
    uint32_t origen = *(uint32_t*) list_get(parametros, 0);
    uint32_t destino = *(uint32_t*) list_get(parametros, 1);

    destino += origen;
}

void set(int cantidad_parametros, t_instruccion* instruccion) {

    t_list* parametros = instruccion->parametros;
    
    uint32_t origen = *(uint32_t*) list_get(parametros, 0);
    uint32_t destino = *(uint32_t*) list_get(parametros, 1);

    destino = origen;
}