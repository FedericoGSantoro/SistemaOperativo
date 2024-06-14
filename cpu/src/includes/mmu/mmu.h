#ifndef MMU_H_
#define MMU_H_

#include "../cpu_vars/cpu_vars.h"
#include <math.h>

uint32_t traducir_direccion_mmu(uint32_t dir_logica, int pid);
t_valor_obtenido_de_memoria leer_de_memoria(int dir_fisica, int pid);
uint32_t numero_pagina(uint32_t dir_logica);
void escribir_en_memoria(uint32_t dir_fisica, int pid, void* registro, tipo_de_dato tipo_de_dato_datos, uint32_t num_pagina);
void resize_en_memoria(int pid, int size_to_resize);
/*
Realiza la peticion de las direcciones fisicas a memoria y las retorna como int* (Hacer free posteriormente)
cantidad_bytes = cantidad de bytes a leer o escribir
direccion_logica = direccion logica inicial
Retorna un int* asi: [cantidad_direcciones, direccionFisica1, direccionFisica2, ...]
*/
int* peticion_de_direcciones_fisicas(uint32_t cantidad_bytes, uint32_t* direccion_logica);
/*
Retorna la cantidad de paginas necesarias basandose en la cantidad a leer o escribir y la direccion logica
cantidad_bytes = cantidad de bytes a leer o escribir
direccion_logica = direccion logica inicial
*/
int cantidad_paginas_necesarias (uint32_t cantidad_bytes, uint32_t dir_logica);

#endif