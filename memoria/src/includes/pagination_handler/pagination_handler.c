#include "./pagination_handler.h"

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

void inicializar_memoria_almacenamiento() {

    int cant_marcos = obtener_cant_marcos();
    espacio_usuario.espacio_usuario = malloc(sizeof(memConfig.tamMemoria));// inicializa todos sus bytes a cero.
    pthread_mutex_init(&espacio_usuario.mx_espacio_usuario, NULL);
    vector_marcos = calloc(cant_marcos, sizeof(int));

    for(int i=0; i < cant_marcos; i++) //creo los marcos/paginas 
    {
       vector_marcos[i] =  0;
    }
    log_info(loggerAux, "Memoria dividida en %d marcos creada", cant_marcos);
}

void inicializar_tabla_paginas(int pid) {
    
    const int cant_paginas = 0;
    t_list* tabla_paginas = list_create();
    char* pid_convertido = int_to_string(pid);
    dictionary_put(tablas_por_proceso, pid_convertido, tabla_paginas);

    log_info(loggerOblig, "PID: %d - Tamaño: %d", pid, cant_paginas);
}

int asignar_frame_libre()
{
    for(int i=0; i<obtener_cant_marcos(); i++){
        if(vector_marcos[i] == 0) {
            vector_marcos[i] = 1;
            return i;
        }
    }
    return -1;
}

int resolver_solicitud_de_marco(int numero_pagina, int pid) {

    char* pid_str = int_to_string(pid);
    t_list* tabla_proceso = dictionary_get(tablas_por_proceso, pid_str);

    if(numero_pagina < 0 || numero_pagina >= (list_size(tabla_proceso))){
        log_error(loggerAux, "El numero de pagina se encuentra fuera de los limites");
        //TODO: Ver que hacer cuando hay errorcito
        return -1;
    }

    t_pagina *pagina = list_get(tabla_proceso, numero_pagina);

    int numero_marco = pagina->marco;
    pagina -> ultima_referencia = time(NULL);
    
    log_info(loggerOblig, "PID: %d - Pagina: %d - Marco: %d",pid, numero_pagina, numero_marco);

    free(pid_str);
    return numero_marco;
}
