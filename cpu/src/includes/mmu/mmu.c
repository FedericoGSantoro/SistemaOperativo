#include "mmu.h"

int solicitar_numero_de_marco(int num_pagina, int pid)
{
    t_paquete *paquete = crear_paquete(DEVOLVER_MARCO);

    agregar_a_paquete(paquete, &num_pagina, sizeof(int));
    agregar_a_paquete(paquete, &pid, sizeof(int));

    enviar_paquete(paquete, fd_memoria);

    int numero_marco;

    if(recv(fd_memoria, &numero_marco, sizeof(int), MSG_WAITALL) <= 0)
    {
        log_error(logger_aux_cpu, "Ocurrio un error al recibir el numero de marco");
        return -1;
    }

   
    return numero_marco;
}

int numero_pagina(int dir_logica)
{
    return floor(dir_logica / tam_pagina);
}

int traducir_direccion_mmu(uint32_t dir_logica, int pid)
{
    int desplazamiento = dir_logica - numero_pagina(dir_logica) * tam_pagina; // esto seria el resto entre la division de DL y tamanio de pagina (cuyo cociente es el numero de pagina)
   
    int num_marco = solicitar_numero_de_marco(numero_pagina(dir_logica), pid);
    if(num_marco == -1)
    {
        return -1;
    }
    
    int dir_fisica = (num_marco * tam_pagina) + desplazamiento;
    return dir_fisica;
}

uint32_t* leer_de_memoria(int dir_fisica, int pid)
{
    t_paquete *paquete = crear_paquete(LEER_VALOR_MEMORIA);

    agregar_a_paquete(paquete, &dir_fisica, sizeof(int));

    agregar_a_paquete(paquete, &pid, sizeof(int));

    enviar_paquete(paquete, fd_memoria);

    op_codigo cod_op = recibir_operacion(fd_memoria);
    if(cod_op != WRITE)
    {
        log_error(logger_aux_cpu, "Ocurrio un error al hacer MOV_IN");
    }
    t_list *paquete_recibido = recibir_paquete(fd_memoria);

    uint32_t* valor_leido = malloc(sizeof(uint32_t));
    *valor_leido = *(uint32_t*) list_get(paquete_recibido, 0);
    
    return valor_leido;
}

void escribir_en_memoria(int dir_fisica, int pid, uint32_t registro, int num_pagina)
{
    t_paquete *paquete = crear_paquete(ESCRIBIR_VALOR_MEMORIA);

    agregar_a_paquete(paquete, &dir_fisica, sizeof(int));

    agregar_a_paquete(paquete, &pid, sizeof(int));

    agregar_a_paquete(paquete, &registro, sizeof(uint32_t));

    agregar_a_paquete(paquete, &num_pagina, sizeof(int));

    enviar_paquete(paquete, fd_memoria);
    
    op_codigo cod_op = recibir_operacion(fd_memoria);
    if(cod_op != OK_OPERACION)
    {
        log_error(logger_aux_cpu, "Error al escribir en memoria");
        return;
    }

    log_info(logger_aux_cpu, "SE ESCRIBIO EN MEMORIA");
}

void resize_en_memoria(int pid, int size_to_resize) {

    t_paquete *paquete = crear_paquete(RESIZE_EN_MEMORIA);

    agregar_a_paquete(paquete, &pid, sizeof(int));
    agregar_a_paquete(paquete, &size_to_resize, sizeof(int));

    enviar_paquete(paquete, fd_memoria);

    log_info(logger_aux_cpu, "SE RESIZEO EN MEMORIA");
}
