#include "mmu.h"


int cantidad_paginas_necesarias (uint32_t cantidad_bytes, uint32_t dir_logica) {
    // Calculamos offset, cantidad de bytes a leer o escribir a partir del offset y determinamos cantidad de paginas en 1
    // Se asume que no va a haber casos que se lean o escriban 0 bytes
    int desplazamiento = dir_logica % tam_pagina; 
    int cantidad_faltante = cantidad_bytes - (tam_pagina - desplazamiento); 
    int cantidad_paginas = 1;

    // Si falta bytes por leer o escribir sumamos una pagina 
    for(int i = 0; cantidad_faltante > 0; i++) {
        cantidad_faltante = cantidad_faltante - tam_pagina; 
        cantidad_paginas++; 
    }
    return cantidad_paginas;
}

int* peticion_de_direcciones_fisicas(uint32_t cantidad_bytes, uint32_t* direccion_logica) {
    // Creamos una copia del dato para no modificar el dato original
    uint32_t dir_logica = *direccion_logica; 
    int num_pagina = numero_pagina(dir_logica);

    // Calculamos la cantidad de paginas necesarias
    int cantidad_paginas = cantidad_paginas_necesarias(cantidad_bytes, dir_logica);

    // Creamos dinamicamente el array de direcciones fisicas
    int* dir_fisicas = malloc(sizeof(int) * (cantidad_paginas + 1));
    // Asignamos al primer elemento la cantidad de direcciones fisicas
    dir_fisicas[0] = cantidad_paginas;
    // Por cada pagina necesitada se realiza la traduccion a direccion fisica de la direccion logica
    for (int i = 1; i < cantidad_paginas; i++) {
        // En el primer caso se realiza con la direccion logica original, despues con una calculada que apunta a la siguiente paginas
        dir_fisicas[i] = traducir_direccion_mmu((uint32_t)dir_logica, pid); 
        // Se cambia la direccion logica para que apunte a la siguiente pagina
        dir_logica = ( num_pagina + i ) * tam_pagina; 
    }

    return dir_fisicas;
}


uint32_t solicitar_numero_de_marco(uint32_t num_pagina, int pid)
{
    t_paquete *paquete = crear_paquete(DEVOLVER_MARCO);

    agregar_a_paquete(paquete, &num_pagina, sizeof(uint32_t));
    agregar_a_paquete(paquete, &pid, sizeof(int));

    enviar_paquete(paquete, fd_memoria);

    op_codigo cod_op = recibir_operacion(fd_memoria);

    // TODO: Por que esta asi, que significa?
    while (cod_op != DEVOLVER_MARCO)
    {
        cod_op = recibir_operacion(fd_memoria);
    }
    
    t_list *paquete_recibido = recibir_paquete(fd_memoria);
    
    uint32_t numero_marco = *(uint32_t*) list_get(paquete_recibido, 0);
   
    return numero_marco;
}

uint32_t numero_pagina(uint32_t dir_logica)
{
    return floor(dir_logica / tam_pagina);
}

uint32_t traducir_direccion_mmu(uint32_t dir_logica, int pid)
{
    int desplazamiento = dir_logica - numero_pagina(dir_logica) * tam_pagina; // esto seria el resto entre la division de DL y tamanio de pagina (cuyo cociente es el numero de pagina)
   
    uint32_t num_marco = solicitar_numero_de_marco(numero_pagina(dir_logica), pid);
    if(num_marco == -1)
    {
        return -1;
    }
    
    uint32_t dir_fisica = (num_marco * tam_pagina) + desplazamiento;
    return dir_fisica;
}

t_valor_obtenido_de_memoria leer_de_memoria(int dir_fisica, int pid)
{
    t_paquete *paquete = crear_paquete(LEER_VALOR_MEMORIA);

    agregar_a_paquete(paquete, &dir_fisica, sizeof(int));

    agregar_a_paquete(paquete, &pid, sizeof(int));

    enviar_paquete(paquete, fd_memoria);

    // TODO: Eliminar paquete
    op_codigo cod_op = recibir_operacion(fd_memoria);
    while (cod_op != LEER_VALOR_MEMORIA)
    {
        cod_op = recibir_operacion(fd_memoria);
    }

    t_list *paquete_recibido = recibir_paquete(fd_memoria);
    tipo_de_dato tipo_de_dato_recibido = *(tipo_de_dato*) list_get(paquete_recibido, 0);
    void* valor_leido = list_get(paquete_recibido, 1);

    t_valor_obtenido_de_memoria valor_obtenido_de_memoria;
    valor_obtenido_de_memoria.valor = valor_leido;
    valor_obtenido_de_memoria.tipo_de_dato_valor = tipo_de_dato_recibido;

    list_destroy(paquete_recibido);

    return valor_obtenido_de_memoria;
}

void escribir_en_memoria(uint32_t dir_fisica, int pid, void* registro, tipo_de_dato tipo_de_dato_datos)
{
    t_paquete *paquete = crear_paquete(ESCRIBIR_VALOR_MEMORIA);

    agregar_a_paquete(paquete, &dir_fisica, sizeof(uint32_t));

    agregar_a_paquete(paquete, &pid, sizeof(int));

    if (tipo_de_dato_datos == UINT32)
    {
        agregar_a_paquete(paquete, registro, sizeof(uint32_t));
    } else {
        agregar_a_paquete(paquete, registro, sizeof(uint8_t));
    }

    agregar_a_paquete(paquete, &tipo_de_dato_datos, sizeof(tipo_de_dato));

    enviar_paquete(paquete, fd_memoria);
    
    op_codigo cod_op = recibir_operacion(fd_memoria);
    while (cod_op != ESCRIBIR_VALOR_MEMORIA)
    {
        cod_op = recibir_operacion(fd_memoria);
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
