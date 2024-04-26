#include "./liberador.h"

// TODO: (EN CASO DE SER NECESARIO) hacer uno para listas, colas, etc,etc,etc.
void free_elements_simple (void* data) {
    free(data);
}

void liberar_lista_de_datos_planos(t_list* lista){
	list_destroy_and_destroy_elements(lista, free_elements_simple);
}

void destroyer_queue_con_datos_simples(void* cola){
    t_queue* cola_casteada = (t_queue*) cola; //castea
    queue_clean_and_destroy_elements(cola_casteada, free_elements_simple);
}

