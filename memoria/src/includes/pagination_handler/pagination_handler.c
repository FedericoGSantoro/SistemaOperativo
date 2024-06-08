#include "./pagination_handler.h"
#include "../../../../utils/src/castingfunctions/castfunctions.h"

int obtener_cant_pags(int size_proceso) {
     return ceil(size_proceso / memConfig.tamPagina); //dependiendo del tamaño del proceso se obtiene la cantidad de paginas con la division entera
}

void inicializar_tabla_paginas(int pid) {
    
    const int cant_paginas = 0;
    t_list* tabla_paginas = list_create();
    char* pid_convertido = int_to_string(pid);
    dictionary_put(tablas_por_proceso, pid_convertido, tabla_paginas);

    log_info(loggerOblig, "PID: %d - Tamaño: %d", pid, cant_paginas);
}

int resolver_solicitud_de_marco(int numero_pagina, int pid) {

    t_list* tabla_proceso = dictionary_get(tablas_por_proceso, pid);

    if(numero_pagina < 0 || numero_pagina >= (list_size(tabla_proceso))){
        log_error(loggerAux, "El numero de pagina se encuentra fuera de los limites");
        //TODO: Ver que hacer cuando hay errorcito
        return;
    }

    t_pagina *pagina = list_get(tabla_proceso, numero_pagina);

    if(!pagina->presencia)
    {
        log_error(loggerAux, "La pagina no se encuentra en memoria");
        //TODO: Ver que hacer cuando hay errorcito
        return;
    }

    int numero_marco = pagina -> marco;
    pagina -> ultima_referencia = time(NULL);
    
    log_info(loggerOblig, "PID: %d - Pagina: %d - Marco: %d",pid, numero_pagina, numero_marco);

    return numero_marco;
}
