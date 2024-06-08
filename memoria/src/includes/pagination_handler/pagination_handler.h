#ifndef PAGINATION_HANDLER_H_
#define PAGINATION_HANDLER_H_

#include "../memoria_vars/memoria_vars.h"
#include "../../../../utils/src/castingfunctions/castfunctions.h"
#include <math.h>

void inicializar_tabla_paginas();
int resolver_solicitud_de_marco(int numero_pagina, int pid);

#endif