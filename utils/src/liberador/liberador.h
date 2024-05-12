#ifndef LIBERADOR_H_
#define LIBERADOR_H_

#include <commons/collections/list.h>
#include <commons/collections/queue.h>
#include <commons/collections/dictionary.h>
#include <stdlib.h>

/*
Libera una lista de datos con punteros.
*/
void liberar_lista_de_datos_con_punteros(t_list* list);

/*
Libera una lista de datos planos (no estructuras).
*/
void liberar_lista_de_datos_planos(t_list* lista);

/*
Destroyer de elementos planos simples
*/
void free_elements_simple (void* data);

/*
la uso para el dictionary_destroy... como destroyer de la cola utilizada
*/
void destroyer_queue_con_datos_simples(void* cola);

/*
la uso para el dictionary_destroy... como destroyer de la lista de instrucciones utilizada
*/
void destroyer_dictionary_key_con_estructuras(t_dictionary* dictionary, char* key, void(*element_destroyer)(void*));

#endif