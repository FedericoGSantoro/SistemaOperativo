#include "tlb.h"

// Chequear uint32_t dir_logica
int buscar_marco_en_tlb(uint32_t dir_logica) {
    // Se busca si la pagina de la dir logica esta en el tlb
    uint32_t num_pagina = numero_pagina(dir_logica);
    int indice = -1;
    t_entradaTLB* entradaTLB = NULL;

    log_info(logger_aux_cpu, "Buscamdp pagina %d de direccion logica %d", num_pagina, dir_logica);
    for (int i = 0; i < cantidadEntradasActuales; i++) {
        t_entradaTLB* entradaActualARevisar = (t_entradaTLB*) list_get(listaEntradasTLB, i);
        if ( entradaActualARevisar->numeroPagina == num_pagina && entradaActualARevisar->pidProceso == pid ) {
            indice = i;
            entradaTLB = entradaActualARevisar;
            break;
        }
    }
    // Si lo encuentra lo devuelve (Si es LRU lo saca de la lista y lo agregga al final)
    if ( entradaTLB != NULL ) {
        log_info(logger_aux_cpu, "Encontre la pagina en la TLB: %d -> %d", entradaTLB->numeroPagina, entradaTLB->numeroMarco);
        log_info(logger_obligatorio_cpu, "PID: %d - TLB HIT - Pagina: %d", pid, entradaTLB->numeroPagina);
        if ( ALGORITMO_TLB == LRU ) {
            list_remove(listaEntradasTLB, indice);
            list_add(listaEntradasTLB, entradaTLB);
            log_info(logger_aux_cpu, "Actualizada a entrada mas reciente por LRU");
        }
    } 
    // Si no lo encuentra hace la peticion a memoria para la traduccion y lo carga (Si no hay espacio, saca el primer elemento y agrega el nuevo al final)
    else {
        log_info(logger_aux_cpu, "No encontre la pagina en la TLB: %d", num_pagina);
        log_info(logger_obligatorio_cpu, "PID: %d - TLB MISS - Pagina: %d", pid, num_pagina);
        int num_marco = solicitar_numero_de_marco(num_pagina);

        if (num_marco == -1) {
            return num_marco;
        }

        if ( CANTIDAD_ENTRADAS_TLB == 0 ) {
            log_info(logger_aux_cpu, "No existe TLB, devolviendo marco");
            free(entradaTLB);
            return num_marco;
        }
        entradaTLB = malloc(sizeof(t_entradaTLB));
        entradaTLB->numeroMarco = num_marco;
        entradaTLB->numeroPagina = num_pagina;
        entradaTLB->pidProceso = pid;
        if ( cantidadEntradasActuales < CANTIDAD_ENTRADAS_TLB ) {
            list_add(listaEntradasTLB, entradaTLB);
            cantidadEntradasActuales++;
            log_info(logger_aux_cpu, "Se añade la entrada a la TLB: %d, ahora %d entradas", entradaTLB->numeroMarco, cantidadEntradasActuales);
        } else {
            t_entradaTLB* entradaVieja = (t_entradaTLB*) list_remove(listaEntradasTLB, 0);
            free(entradaVieja);
            list_add(listaEntradasTLB, entradaTLB);
            log_info(logger_aux_cpu, "Se añade la entrada a la TLB: %d, sobreescribiendo la entrada menos relevante por algoritmo", num_marco);
        }
    }
    return entradaTLB->numeroMarco;
}

uint32_t numero_pagina(uint32_t dir_logica) {
    return floor(dir_logica / tam_pagina);
}

int solicitar_numero_de_marco(uint32_t num_pagina) {

    op_codigo cod_op;

    do {
        t_paquete *paquete = crear_paquete(DEVOLVER_MARCO);

        agregar_a_paquete(paquete, &num_pagina, sizeof(uint32_t));
        agregar_a_paquete(paquete, &pid, sizeof(int));

        enviar_paquete(paquete, fd_memoria);

        eliminar_paquete(paquete);

        cod_op = recibir_operacion(fd_memoria);
    } while (cod_op != DEVOLVER_MARCO && cod_op != NO_MEMORY);

    if (cod_op == NO_MEMORY) {
        return -1;
    }

    t_list *paquete_recibido = recibir_paquete(fd_memoria);
    
    uint32_t numero_marco = *(uint32_t*) list_get(paquete_recibido, 0);
   
    list_destroy(paquete_recibido);

    return numero_marco;
}