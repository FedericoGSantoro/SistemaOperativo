#include "./pagination_handler.h"

pthread_mutex_t* crear_mutex(){
	pthread_mutex_t* mutex = malloc(sizeof(pthread_mutex_t));
	pthread_mutex_init(mutex,NULL);
	return mutex;
}

int obtener_cant_pags(int size_proceso) {
     return ceil(size_proceso / memConfig.tamPagina); //dependiendo del tamaño del proceso se obtiene la cantidad de paginas con la division entera
}

int obtener_cant_pags_usadas() {

    t_list* todas_las_tablas_de_paginas = dictionary_elements(tablas_por_proceso);
    int cantidad_paginas = 0; 
    for(int i = 0; i < list_size(todas_las_tablas_de_paginas); i++) {
        t_list* tablas_paginas_de_proceso = list_get(todas_las_tablas_de_paginas, i);
        cantidad_paginas += list_size(tablas_paginas_de_proceso);
    }

    log_info(loggerAux, "Cantidad de paginas usadas %d", cantidad_paginas);

    return cantidad_paginas;
}

int obtener_cant_marcos() {
     return memConfig.tamMemoria / memConfig.tamPagina;
}

t_marco* crear_marco(int i){
	t_marco* nuevo_marco=malloc(sizeof(t_marco));
	nuevo_marco->libre=true;
	nuevo_marco->base=memConfig.tamPagina * i;//0
	nuevo_marco->limite=memConfig.tamPagina * (i+1);//32
	nuevo_marco->mutexMarco = crear_mutex();
	return nuevo_marco;
}

void inicializar_memoria_almacenamiento() {

    pthread_mutex_init(&espacio_usuario.mx_espacio_usuario, NULL);
    pthread_mutex_init(&mx_tablas_paginas, NULL);
    pthread_mutex_init(&mx_lista_marcos, NULL);

    int cant_marcos = obtener_cant_marcos();
    espacio_usuario.espacio_usuario = malloc(memConfig.tamMemoria);// inicializa todos sus bytes a cero.
    lista_marcos = list_create();
    espacio_usuario.tipo_de_dato_almacenado = list_create();
    for(int i=0; i < cant_marcos; i++) //creo los marcos/paginas 
    {
        list_add(lista_marcos, crear_marco(i));
    }

    for(int i=0; i < memConfig.tamMemoria; i++) //creo los marcos/paginas 
    {
        list_add_in_index(espacio_usuario.tipo_de_dato_almacenado, i, 0);
    }
    
    log_info(loggerAux, "Memoria dividida en %d marcos creada", cant_marcos);
}

void inicializar_tabla_paginas(int pid) {
    
    const int cant_paginas = 0;
    t_list* tabla_paginas = list_create();
    char* pid_convertido = int_to_string(pid);
	
    pthread_mutex_lock(&mx_tablas_paginas);
	dictionary_put(tablas_por_proceso, pid_convertido, tabla_paginas);
    pthread_mutex_unlock(&mx_tablas_paginas);

    log_info(loggerOblig, "PID: %d - Tamaño: %d", pid, cant_paginas);
}

bool marco_libre(t_marco * marco){
	//if(marco->libre) debug("Estoy libre");
	return (marco->libre);
}

uint32_t asignar_frame_libre()
{
        t_marco* marco_libre_encontrado;
        uint32_t numero_marco_hallado;
		pthread_mutex_lock(&mx_lista_marcos);
        marco_libre_encontrado = list_find(lista_marcos, (void*)marco_libre);
		pthread_mutex_unlock(&mx_lista_marcos);

        pthread_mutex_lock(marco_libre_encontrado->mutexMarco);
	    marco_libre_encontrado->libre = false;
	    uint32_t base = marco_libre_encontrado->base;
	    pthread_mutex_unlock(marco_libre_encontrado->mutexMarco);

	    numero_marco_hallado = base / memConfig.tamPagina;

        return numero_marco_hallado;
}

uint32_t resolver_solicitud_de_marco(uint32_t numero_pagina, int pid) {

    char* pid_str = int_to_string(pid);

    pthread_mutex_lock(&mx_tablas_paginas);
    t_list* tabla_proceso = dictionary_get(tablas_por_proceso, pid_str);

    if(numero_pagina < 0 || numero_pagina >= (list_size(tabla_proceso))){
        log_error(loggerAux, "El numero de pagina se encuentra fuera de los limites");
        //TODO: Ver que hacer cuando hay errorcito
        return -1;
    }

    t_pagina *pagina = list_get(tabla_proceso, numero_pagina);
	pthread_mutex_unlock(&mx_tablas_paginas);

    pthread_mutex_lock(pagina->mx_pagina);
    uint32_t numero_marco = pagina->marco;
    pagina->ultima_referencia = time(NULL);
    pthread_mutex_unlock(pagina->mx_pagina);

    log_info(loggerOblig, "PID: %d - Pagina: %d - Marco: %d",pid, numero_pagina, numero_marco);

    free(pid_str);
    return numero_marco;
}

void resize_proceso(int pid, int size_to_resize) {

    int cant_marcos = obtener_cant_marcos();
    int cant_pags = obtener_cant_pags(size_to_resize);
    
    int cant_pags_usadas = obtener_cant_pags_usadas();
    if (cant_pags + cant_pags_usadas > cant_marcos) {
        // TODO: VER DE DEVOLVER UN OUT OF MEMORY PARA CPU!!!!
    }

    char* pid_str = int_to_string(pid);
    t_list* tabla_paginas_de_proceso = (t_list*) dictionary_get(tablas_por_proceso, pid_str);

    int index = 0;
    int size_tablas_paginas_de_proceso = list_size(tabla_paginas_de_proceso);
    if (size_tablas_paginas_de_proceso != 0) {
        index = size_tablas_paginas_de_proceso - 1;
    }

    for(int i = index; i < index + cant_pags; i++) {
        t_pagina *pagina = (t_pagina*)malloc(sizeof(t_pagina));
        pagina->marco = asignar_frame_libre();
        pagina->tiempo_carga = 0; 
        pagina->ultima_referencia = 0;
        pagina->pid = pid;
        pagina->mx_pagina = crear_mutex();
        list_add_in_index(tabla_paginas_de_proceso, i, pagina);
    }

    log_info(loggerAux, "Quedaron %d paginas", list_size(tabla_paginas_de_proceso));

    free(pid_str);
}
