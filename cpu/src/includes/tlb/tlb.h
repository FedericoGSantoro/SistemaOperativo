#ifndef TLB_H_
#define TLB_H_

#include "../cpu_vars/cpu_vars.h"
#include <math.h>

/*----------------ESTRUCTURA TLB----------------*/

typedef struct{
    uint32_t pidProceso;
    uint32_t numeroPagina;
    uint32_t numeroMarco;
} t_entradaTLB;

/*------------------FUNCIONES-------------------*/

/*
Busca el marco de la direccion logica en TLB, si no lo encuentra solicita a memoria la misma, la carga y la devuelve
dir_logica = direccion logica a buscar
*/
uint32_t buscar_marco_en_tlb(uint32_t dir_logica);
/*
Solicita el numero de pagina a memoria y devuelve el marco correspondiente
num_pagina = numero de pagina a pedir
*/
uint32_t solicitar_numero_de_marco(uint32_t num_pagina);
/*
Devuelve el numero de pagina de la direccion logica
dir_logica = direccion logica a obtener el numero de pagina
*/
uint32_t numero_pagina(uint32_t dir_logica);


#endif