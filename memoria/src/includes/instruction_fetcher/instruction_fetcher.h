#ifndef INSTRUCTION_FETCHER_H_
#define INSTRUCTION_FETCHER_H_

#include "../memoria_vars/memoria_vars.h"
#include "../../../../utils/src/liberador/liberador.h"
#include <stdio.h>

char* fetch_instruccion(int pid);
void crear_instrucciones(char* path, int pid);

#endif /* INSTRUCTION_FETCHER_H_ */