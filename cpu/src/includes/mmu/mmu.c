#include "mmu.h"

uint32_t cantidad_bytes_que_se_pueden_leer(uint32_t dir) {
    uint32_t offset = dir % tam_pagina;
    return tam_pagina - offset;
}

int cantidad_paginas_necesarias (uint32_t cantidad_bytes, uint32_t dir_logica) {
    // Calculamos offset, cantidad de bytes a leer o escribir a partir del offset y determinamos cantidad de paginas en 1
    // Se asume que no va a haber casos que se lean o escriban 0 bytes
    int cantidad_faltante = cantidad_bytes - cantidad_bytes_que_se_pueden_leer(dir_logica); 
    int cantidad_paginas = 1;

    // Si falta bytes por leer o escribir sumamos una pagina 
    for(int i = 0; cantidad_faltante > 0; i++) {
        cantidad_faltante = cantidad_faltante - tam_pagina; 
        cantidad_paginas++; 
    }
    return cantidad_paginas;
}

t_list* peticion_de_direcciones_fisicas(void* cantidad_bytes, tipo_de_dato tipo_de_dato_cantidad_bytes, void* direccion_logica, tipo_de_dato tipo_de_dato_direccion_logica) {
    // Creamos una copia del dato para no modificar el dato original
    uint32_t cant_bytes = get_registro_numerico_casteado_32b(cantidad_bytes, tipo_de_dato_cantidad_bytes);
    uint32_t dir_logica = get_registro_numerico_casteado_32b(direccion_logica, tipo_de_dato_direccion_logica); 
    int num_pagina = numero_pagina(dir_logica);

    // Calculamos la cantidad de paginas necesarias
    int cantidad_paginas = cantidad_paginas_necesarias(cant_bytes, dir_logica);

    // Creamos dinamicamente el array de direcciones fisicas
    //int* dir_fisicas = malloc(sizeof(int) * (cantidad_paginas + 1));
    // Asignamos al primer elemento la cantidad de direcciones fisicas
    //dir_fisicas[0] = cantidad_paginas; //TODO: refactorizar esto para no hacer este hack y devolver un struct con cantidad de paginas y el array de direcciones fisicas
   
    t_list* direcciones_fisicas = list_create();
    
    // Por cada pagina necesitada se realiza la traduccion a direccion fisica de la direccion logica
    for (int i = 0; i < cantidad_paginas; i++) {
        // En el primer caso se realiza con la direccion logica original, despues con una calculada que apunta a la siguiente paginas
        int* traduccion = malloc(sizeof(int));
        *traduccion = traducir_direccion_mmu(dir_logica);
        
        if (*traduccion == -1) {
            return list_create();
        }

        list_add_in_index(direcciones_fisicas, i, traduccion); 
        // Se cambia la direccion logica para que apunte a la siguiente pagina
        dir_logica = ( num_pagina + i + 1 ) * tam_pagina; 
    }

    return direcciones_fisicas;
}

int traducir_direccion_mmu(uint32_t dir_logica)
{

    uint32_t num_pagina = numero_pagina(dir_logica);
    int desplazamiento = dir_logica - num_pagina * tam_pagina; // esto seria el resto entre la division de DL y tamanio de pagina (cuyo cociente es el numero de pagina)
   
    // TODO: Cambiar para usar TLB:
    int num_marco = buscar_marco_en_tlb(dir_logica);
    log_info(logger_obligatorio_cpu, "PID: %d - OBTENER MARCO - Pagina: %d - Marco: %d", pid, num_pagina, num_marco);
    
    if(num_marco == -1)
    {
        return -1;
    }
    
    uint32_t dir_fisica = (num_marco * tam_pagina) + desplazamiento;
    return dir_fisica;
}

t_list* obtener_valores() {
    
    t_list* paquete_recibido = recibir_paquete(fd_memoria);
    t_list* valores = list_create();

    for (int i = 0; i < list_size(paquete_recibido); i++) {
        void* bytes_guardados = list_get(paquete_recibido, i);
        list_add_in_index(valores, i, bytes_guardados);
    }

    list_destroy(paquete_recibido);

    return valores;
}

t_list* leer_de_memoria(t_list* direcciones_fisicas, int pid, uint32_t tamanio_a_leer_en_memoria)
{
    t_paquete *paquete = crear_paquete(LEER_VALOR_MEMORIA);

    int cantidad_direcciones_fisicas = list_size(direcciones_fisicas);
    agregar_a_paquete(paquete, &cantidad_direcciones_fisicas, sizeof(int)); //agrego cuantas direcciones fisicas hay

    for (int i = 0; i < cantidad_direcciones_fisicas; i++) {
        //uint32_t direccion_fisica = *(uint32_t*)list_get(direcciones_fisicas, i);
        agregar_a_paquete(paquete, (uint32_t*)list_get(direcciones_fisicas, i), sizeof(uint32_t));
    }

    agregar_a_paquete(paquete, &pid, sizeof(int));

    agregar_a_paquete(paquete, &tamanio_a_leer_en_memoria, sizeof(uint32_t));

    enviar_paquete(paquete, fd_memoria);

    eliminar_paquete(paquete);

    op_codigo cod_op = recibir_operacion(fd_memoria);
    while (cod_op != LEER_VALOR_MEMORIA)
    {
        cod_op = recibir_operacion(fd_memoria);
    }

    t_list* valores_leidos = obtener_valores();

    log_info(logger_aux_cpu, "SE LEYO EN MEMORIA");

    return valores_leidos;
}

t_list* escribir_en_memoria(t_list* direcciones_fisicas, int pid, void* registro, uint32_t cantidad_bytes)
{
    t_paquete *paquete = crear_paquete(ESCRIBIR_VALOR_MEMORIA);

    int cantidad_direcciones_fisicas = list_size(direcciones_fisicas);
    agregar_a_paquete(paquete, &cantidad_direcciones_fisicas, sizeof(int)); //agrego cuantas direcciones fisicas hay

    for (int i = 0; i < cantidad_direcciones_fisicas; i++) {
        agregar_a_paquete(paquete, (uint32_t*)list_get(direcciones_fisicas, i), sizeof(uint32_t));
    }

    agregar_a_paquete(paquete, &pid, sizeof(int));

    agregar_a_paquete(paquete, &cantidad_bytes, sizeof(uint32_t));

    agregar_a_paquete(paquete, registro, cantidad_bytes);

    enviar_paquete(paquete, fd_memoria);

    eliminar_paquete(paquete);
    
    op_codigo cod_op = recibir_operacion(fd_memoria);
    while (cod_op != ESCRIBIR_VALOR_MEMORIA)
    {
        cod_op = recibir_operacion(fd_memoria);
    }

    t_list* valores_escritos = obtener_valores();

    log_info(logger_aux_cpu, "SE ESCRIBIO EN MEMORIA");

    return valores_escritos;
}

void resize_en_memoria(int pid, int size_to_resize) {

    t_paquete *paquete = crear_paquete(RESIZE_EN_MEMORIA);

    agregar_a_paquete(paquete, &pid, sizeof(int));
    agregar_a_paquete(paquete, &size_to_resize, sizeof(int));

    enviar_paquete(paquete, fd_memoria);

    eliminar_paquete(paquete);

    log_info(logger_aux_cpu, "SE RESIZEO EN MEMORIA");
}
