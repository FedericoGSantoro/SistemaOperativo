#ifndef MMU_H_
#define MMU_H_

#include "../cpu_vars/cpu_vars.h"
#include <math.h>

int traducir_direccion_mmu(int dir_logica, int pid);
char* leer_de_memoria(int dir_fisica, int pid);

#endif