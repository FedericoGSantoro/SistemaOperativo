#ifndef MMU_H_
#define MMU_H_

#include "../cpu_vars/cpu_vars.h"
#include <math.h>

int traducir_direccion_mmu(uint32_t dir_logica, int pid);
uint32_t* leer_de_memoria(int dir_fisica, int pid);
void escribir_en_memoria(int dir_fisica, int pid, uint32_t registro, int num_pagina);
void resize_en_memoria(int pid, int size_to_resize);

#endif