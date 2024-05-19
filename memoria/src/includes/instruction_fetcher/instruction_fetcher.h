#ifndef INSTRUCTION_FETCHER_H_
#define INSTRUCTION_FETCHER_H_

#include "../memoria_vars/memoria_vars.h"
#include "../../../../utils/src/castingfunctions/castfunctions.h"
#include "../../../../utils/src/liberador/liberador.h"

char* fetch_instruccion(int pid, int pc);
void crear_instrucciones(char* path, int pid);
void eliminar_instrucciones(int pid);

#endif /* INSTRUCTION_FETCHER_H_ */