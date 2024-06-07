#ifndef MMU_H_
#define MMU_H_

#include "../cpu_vars/cpu_vars.h"

int traducir_direccion_mmu(int dir_logica, int pid);

#endif