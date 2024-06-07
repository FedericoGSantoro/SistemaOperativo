#include "./pagination_handler.h"
#include "../../../../utils/src/castingfunctions/castfunctions.h"

int obtener_cant_pags(int size_proceso) {
     return ceil(size_proceso / memConfig.tamPagina); //dependiendo del tamaño del proceso se obtiene la cantidad de paginas con la division entera
}

void inicializar_tabla_paginas(int pid) {
    
    const int cant_paginas = 0;
    t_list* tabla_paginas = list_create();
    char* pid_convertido = int_to_string(pid);
    dictionary_put(cache_tabla_por_proceso, pid_convertido, tabla_paginas);

    log_info(loggerOblig, "PID: %d - Tamaño: %d", pid, cant_paginas);
}