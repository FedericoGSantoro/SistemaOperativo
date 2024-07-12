#include "./pagination_handler.h"

pthread_mutex_t* crear_mutex(){
	pthread_mutex_t* mutex = malloc(sizeof(pthread_mutex_t));
	pthread_mutex_init(mutex,NULL);
	return mutex;
}

int obtener_cant_pags(int size_proceso) {
    float resultado =  ((float) size_proceso / (float) memConfig.tamPagina);
    return ceil(resultado); //dependiendo del tamaño del proceso se obtiene la cantidad de paginas con la division entera
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

t_list* get_tabla_paginas_por_proceso(uint32_t pid) {

    char* pid_str = int_to_string(pid);
    return (t_list*) dictionary_get(tablas_por_proceso, pid_str);

    free(pid_str);
}

int obtener_cant_paginas_usadas_por_proceso(uint32_t pid) {

    t_list* tabla_paginas_de_proceso = get_tabla_paginas_por_proceso(pid);
    return list_size(tabla_paginas_de_proceso);
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
    for(int i=0; i < cant_marcos; i++) //creo los marcos/paginas 
    {
        list_add(lista_marcos, crear_marco(i));
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

void agrandar_proceso(int cant_pags_usadas, int cant_pags_a_risezear, int cant_marcos, int pid) {

    t_list* tabla_paginas_de_proceso = get_tabla_paginas_por_proceso(pid);

    int index = 0;
    int size_tablas_paginas_de_proceso = list_size(tabla_paginas_de_proceso);
    if (size_tablas_paginas_de_proceso != 0) {
        index = size_tablas_paginas_de_proceso - 1;
    }

    for(int i = index; i < index + cant_pags_a_risezear; i++) {
        t_pagina *pagina = (t_pagina*)malloc(sizeof(t_pagina));
        pagina->marco = asignar_frame_libre();
        pagina->tiempo_carga = 0; 
        pagina->ultima_referencia = 0;
        pagina->pid = pid;
        pagina->mx_pagina = crear_mutex();
        list_add_in_index(tabla_paginas_de_proceso, i, pagina);
    }
}

void achicar_proceso(int cant_pags_a_risezear, int pid) {
    
    t_list* tabla_paginas = get_tabla_paginas_por_proceso(pid);
    for (int i = list_size(tabla_paginas)-1; i >= cant_pags_a_risezear; i--) {
        //No usamos mutex porque es independiente por id de proceso! -> NO voy a estar añadiendo nuevas paginas a un proceso mientras estoy removiendo!
        t_pagina* pagina_removida = (t_pagina*) list_remove(tabla_paginas, i);
        uint32_t id_marco_usado = pagina_removida->marco;
        t_marco* marco_usado = list_get(lista_marcos, id_marco_usado);
        pthread_mutex_lock(marco_usado->mutexMarco);
        marco_usado->libre = false;
        pthread_mutex_unlock(marco_usado->mutexMarco); 
    
        pthread_mutex_destroy(pagina_removida->mx_pagina);   
        free(pagina_removida);    
    }
}

void resize_proceso(int pid, int size_to_resize, int fd_cliente_cpu) {

    int cant_marcos = obtener_cant_marcos();
    int cant_pags_a_risezear = obtener_cant_pags(size_to_resize);
    
    int cant_pags_usadas = obtener_cant_pags_usadas();
    if (cant_pags_a_risezear + cant_pags_usadas > cant_marcos) {
        // outofmemory
        enviar_codigo_op(NO_MEMORY, fd_cliente_cpu); //No considero necesario mandar un OK_OPERACION
        return;
    }

    int cant_paginas_usadas_por_proceso = obtener_cant_paginas_usadas_por_proceso(pid);

    if (cant_paginas_usadas_por_proceso < cant_pags_a_risezear){ //La cantidad de paginas es menor? Osea, estoy agrandando el proceso?
        agrandar_proceso(cant_pags_usadas, cant_pags_a_risezear, cant_marcos, pid);
        log_info(loggerOblig, "PID: %d - Tamaño Actual: %d - Tamaño a Ampliar: %d", pid, cant_paginas_usadas_por_proceso*memConfig.tamPagina, cant_pags_a_risezear*memConfig.tamPagina);
    } else {
        achicar_proceso(cant_pags_a_risezear, pid);
        log_info(loggerOblig, "PID: %d - Tamaño Actual: %d - Tamaño a Reducir: %d", pid, cant_paginas_usadas_por_proceso*memConfig.tamPagina, cant_pags_a_risezear*memConfig.tamPagina);
    }

    log_info(loggerAux, "Quedaron %d paginas para PID: %d", list_size(get_tabla_paginas_por_proceso(pid)), pid);
}

void destruir_pagina(t_pagina* pagina){
	pthread_mutex_lock(&mx_lista_marcos);
	t_marco* marco_a_liberar = list_get(lista_marcos, pagina->marco);
	pthread_mutex_unlock(&mx_lista_marcos);

	pthread_mutex_lock(marco_a_liberar->mutexMarco);
	marco_a_liberar->libre = true;
	pthread_mutex_unlock(marco_a_liberar->mutexMarco);
	pthread_mutex_destroy(pagina->mx_pagina);
	free(pagina);
}

void destruir_tabla_paginas(t_list* tabla_de_paginas){
	list_destroy_and_destroy_elements(tabla_de_paginas,(void*)destruir_pagina);
}

void eliminar_paginas(int pid) {

    char* pid_str = int_to_string(pid);
	if(dictionary_has_key(tablas_por_proceso,pid_str)){
		t_list* tabla_de_paginas = dictionary_remove(tablas_por_proceso, pid_str);
		log_info(loggerOblig, "Destruccion Tabla de Paginas PID: %d - Tamaño: %d", pid, list_size(tabla_de_paginas));
		destruir_tabla_paginas(tabla_de_paginas);
	}
}