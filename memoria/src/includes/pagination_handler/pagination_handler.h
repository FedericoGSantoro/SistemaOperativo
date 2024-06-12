#ifndef PAGINATION_HANDLER_H_
#define PAGINATION_HANDLER_H_

#include "../memoria_vars/memoria_vars.h"
#include "../../../../utils/src/castingfunctions/castfunctions.h"
#include <math.h>

void inicializar_memoria_almacenamiento();
void inicializar_tabla_paginas();
uint32_t resolver_solicitud_de_marco(uint32_t numero_pagina, int pid);
int obtener_cant_pags(int size_proceso);
int obtener_cant_marcos();
uint32_t asignar_frame_libre();

#endif