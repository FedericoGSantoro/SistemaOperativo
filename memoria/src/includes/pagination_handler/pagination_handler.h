#ifndef PAGINATION_HANDLER_H_
#define PAGINATION_HANDLER_H_

#include "../memoria_vars/memoria_vars.h"
#include "../../../../utils/src/castingfunctions/castfunctions.h"
#include <math.h>
#include "../../../../utils/src/sockets/sockets.h"
#include "../../../../utils/src/types/types.h"
#include "../../../../utils/src/liberador/liberador.h"

void inicializar_memoria_almacenamiento();
void inicializar_tabla_paginas();
int resolver_solicitud_de_marco(uint32_t numero_pagina, int pid);
int obtener_cant_pags(int size_proceso);
int obtener_cant_marcos();
uint32_t asignar_frame_libre();
void eliminar_paginas(int pid);
void resize_proceso(int pid, int size_to_resize, int fd_cliente_cpu);

#endif