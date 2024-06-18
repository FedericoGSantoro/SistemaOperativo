#ifndef MMU_H_
#define MMU_H_

#include "../tlb/tlb.h"

uint32_t traducir_direccion_mmu(uint32_t dir_logica);
void* leer_de_memoria(int dir_fisica, int pid, uint32_t tamanio_a_leer_en_memoria);
void escribir_en_memoria(t_list* direcciones_fisicas, int pid, void* registro, uint32_t cantidad_bytes);
void resize_en_memoria(int pid, int size_to_resize);
/*
Realiza la peticion de las direcciones fisicas a memoria y las retorna como int* (Hacer free posteriormente)
cantidad_bytes = cantidad de bytes a leer o escribir
tipo_de_dato_cantidad_bytes = tipo de dato de la variable
direccion_logica = direccion logica inicial
tipo_de_dato_direccion_logica = tipo de dato de la variable
Retorna un lista asi: [direccionFisica1, direccionFisica2, ...]
*/
t_list* peticion_de_direcciones_fisicas(void* cantidad_bytes, tipo_de_dato tipo_de_dato_cantidad_bytes, void* direccion_logica, tipo_de_dato tipo_de_dato_direccion_logica);
/*
Retorna la cantidad de paginas necesarias basandose en la cantidad a leer o escribir y la direccion logica
cantidad_bytes = cantidad de bytes a leer o escribir
direccion_logica = direccion logica inicial
*/
int cantidad_paginas_necesarias (uint32_t cantidad_bytes, uint32_t dir_logica);
/*
Retorna la cantidad de bytes que se pueden leer a partir del offset de la direccion hasta el final de pagina
dir_logica = direccion logica a comprobar
*/
uint32_t cantidad_bytes_que_se_pueden_leer(uint32_t dir_logica);

#endif