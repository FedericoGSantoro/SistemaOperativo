#ifndef LIBERADOR_H_
#define LIBERADOR_H_

#include <commons/collections/list.h>
#include <commons/collections/queue.h>
#include <stdlib.h>

/*
Libera una lista de datos planos (no estructuras).
*/
void liberar_lista_de_datos_planos(t_list* list);

/*
Destroyer de elementos planos simples
*/
void free_elements_simple (void* data);

/*
la uso para el dictionary_destroy... como destroyer de la cola utilizada
*/
void destroyer_queue_con_datos_simples(void* cola);

#endif