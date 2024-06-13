#include "./instruction_cycle.h"

void manejarInterrupciones(blocked_reason motivo_nuevo)
{
    pthread_mutex_lock(&variableInterrupcion);
    if (motivo_bloqueo < motivo_nuevo)
    {
        motivo_bloqueo = motivo_nuevo;
    }
    pthread_mutex_unlock(&variableInterrupcion);
}

t_instruccion *instruccion;

void *mapear_registro(char *nombre_registro)
{

    if (string_equals_ignore_case(nombre_registro, "AX"))
        return (uint8_t *)&(registros_cpu.ax);
    else if (string_equals_ignore_case(nombre_registro, "PC"))
        return (uint32_t *)&(registros_cpu.pc);
    else if (string_equals_ignore_case(nombre_registro, "BX"))
        return (uint8_t *)&(registros_cpu.bx);
    else if (string_equals_ignore_case(nombre_registro, "CX"))
        return (uint8_t *)&(registros_cpu.cx);
    else if (string_equals_ignore_case(nombre_registro, "DX"))
        return (uint8_t *)&(registros_cpu.dx);
    else if (string_equals_ignore_case(nombre_registro, "EAX"))
        return (uint32_t *)&(registros_cpu.eax);
    else if (string_equals_ignore_case(nombre_registro, "EBX"))
        return (uint32_t *)&(registros_cpu.ebx);
    else if (string_equals_ignore_case(nombre_registro, "ECX"))
        return (uint32_t *)&(registros_cpu.ecx);
    else if (string_equals_ignore_case(nombre_registro, "EDX"))
        return (uint32_t *)&(registros_cpu.edx);
    else if (string_equals_ignore_case(nombre_registro, "SI"))
        return (uint32_t *)&(registros_cpu.si);
    else if (string_equals_ignore_case(nombre_registro, "DI"))
        return (uint32_t *)&(registros_cpu.di);
    else
    {
        valor_registro_numerico = string_to_int(nombre_registro);
        return (uint32_t *)&valor_registro_numerico;
    }
}

tipo_de_dato mapear_tipo_de_dato(char *nombre_registro)
{
    // TODO: Corregir caso SI y DI siendo de 32 bits
    if (!is_number(nombre_registro) && string_length(nombre_registro) == 2 && !string_equals_ignore_case(nombre_registro, "PC"))
    {
        return UINT8;
    }
    return UINT32;
}

// Funciones para ejecutar instrucciones (execute)

void sum_instruction(t_list *parametros)
{

    char *destino = (char *)list_get(parametros, 0);
    char *origen = (char *)list_get(parametros, 1);

    void *registro_destino = mapear_registro(destino);
    void *registro_origen = mapear_registro(origen);

    tipo_de_dato tipo_de_dato_destino = mapear_tipo_de_dato(destino);
    tipo_de_dato tipo_de_dato_origen = mapear_tipo_de_dato(origen);

    // TODO: No podria ser un uint32 con un uint8 juntos (ambos casos)?
    if (tipo_de_dato_destino == UINT8 || tipo_de_dato_origen == UINT8)
    {
        uint8_t *registro_destino_casteado = (uint8_t *)registro_destino;
        uint8_t *registro_origen_casteado = (uint8_t *)registro_origen;
        *registro_destino_casteado += *registro_origen_casteado;
    } else {
        uint32_t *registro_destino_casteado = (uint32_t *)registro_destino;
        uint32_t *registro_origen_casteado = (uint32_t *)registro_origen;
        *registro_destino_casteado += *registro_origen_casteado;
    }
}

void sub_instruction(t_list *parametros)
{

    char *destino = (char *)list_get(parametros, 0);
    char *origen = (char *)list_get(parametros, 1);

    void *registro_destino = mapear_registro(destino);
    void *registro_origen = mapear_registro(origen);

    tipo_de_dato tipo_de_dato_destino = mapear_tipo_de_dato(destino);
    tipo_de_dato tipo_de_dato_origen = mapear_tipo_de_dato(origen);

    if (tipo_de_dato_destino == UINT8 || tipo_de_dato_origen == UINT8)
    {
        uint8_t *registro_destino_casteado = (uint8_t *)registro_destino;
        uint8_t *registro_origen_casteado = (uint8_t *)registro_origen;
        *registro_destino_casteado -= *registro_origen_casteado;
    } else {
        uint32_t *registro_destino_casteado = (uint32_t *)registro_destino;
        uint32_t *registro_origen_casteado = (uint32_t *)registro_origen;
        *registro_destino_casteado -= *registro_origen_casteado;
    }
}

void set_instruction(t_list *parametros)
{

    char *destino = (char *)list_get(parametros, 0);
    char *origen = (char *)list_get(parametros, 1);

    void *registro_destino = mapear_registro(destino);
    void *registro_origen = mapear_registro(origen);

    tipo_de_dato tipo_de_dato_destino = mapear_tipo_de_dato(destino);
    tipo_de_dato tipo_de_dato_origen = mapear_tipo_de_dato(origen);

    // TODO: Revisar que el numero siempre es de 32 segun mapear tipo de dato
    if (tipo_de_dato_destino == UINT8 || tipo_de_dato_origen == UINT8)
    {
        uint8_t *registro_destino_casteado = (uint8_t *)registro_destino;
        uint8_t *registro_origen_casteado = (uint8_t *)registro_origen;
        *registro_destino_casteado = *registro_origen_casteado;
    } else {
        uint32_t *registro_destino_casteado = (uint32_t *)registro_destino;
        uint32_t *registro_origen_casteado = (uint32_t *)registro_origen;
        *registro_destino_casteado = *registro_origen_casteado;
    }
}

void jnz_instruction(t_list *parametros)
{

    char *registro = (char *)list_get(parametros, 0);
    char *instruccion_a_moverse = (char *)list_get(parametros, 1);

    // TODO: El registro mapeado puede ser de 8 bits
    uint32_t *registro_mapeado = mapear_registro(registro);
    uint32_t *instruccion_a_moverse_mapeada = mapear_registro(instruccion_a_moverse);

    // Cambio de registro_mapeado a *registro_mapeado para acceder al dato y no al puntero
    if (*registro_mapeado != 0)
    {
        registros_cpu.pc = *instruccion_a_moverse_mapeada - 1; // restamos uno porque despues vamos a sumar uno con el proximo PC++, al finalizar un ciclo de instruccion que tenga a JNZ. Me llevo a consultar si queda en bulce infinito, o como cortamos el JNZ.
    }
}

void io_gen_sleep_instruction(t_list *parametros)
{

    char *nombre_io = (char *)list_get(parametros, 0);
    char *cantidad_tiempo_sleep = (char *)list_get(parametros, 1);
    int *cantidad_tiempo_sleep_parseado = malloc(sizeof(int));
    *cantidad_tiempo_sleep_parseado = string_to_int(cantidad_tiempo_sleep);
    t_params_io *parametro_io = malloc(sizeof(int) * 2);
    t_nombre_instruccion *io_instruccion = malloc(sizeof(int));

    // armado de los parametros de io_detail
    parametro_io->tipo_de_dato = INT;
    parametro_io->valor = cantidad_tiempo_sleep_parseado;

    // armado de io_detail
    io_detail.nombre_io = nombre_io;
    *io_instruccion = IO_GEN_SLEEP;
    io_detail.io_instruccion = *io_instruccion;
    list_add_in_index(io_detail.parametros, 0, parametro_io);

    manejarInterrupciones(LLAMADA_SISTEMA);

    // liberacion de recursos
    // TODO: free de cantidad_tiempo_sleep_parseado y parametro_io?
    free(io_instruccion);
}

void mov_in_instruction(t_list *parametros)
{

    char *registro_datos = (char *)list_get(parametros, 0);
    void *registro_datos_mapeado = mapear_registro(registro_datos);
    char *registro_direccion = (char *)list_get(parametros, 1);
    uint32_t *registro_direccion_mapeado = mapear_registro(registro_direccion);

    uint32_t dir_fisica = traducir_direccion_mmu(*registro_direccion_mapeado, pid);
    if (dir_fisica == -1)
    {
        // TODO: Revisar que hacer en caso de error
        return;
    }

    t_valor_obtenido_de_memoria valor_obtenido_de_memoria = leer_de_memoria(dir_fisica, pid);
    tipo_de_dato tipo_de_dato_datos = mapear_tipo_de_dato(registro_datos);

    // TODO: Funcion generica para esto:
    if (tipo_de_dato_datos == UINT8 || valor_obtenido_de_memoria.tipo_de_dato_valor == UINT8)
    {
        uint8_t *registro_datos_casteado = (uint8_t *)registro_datos_mapeado;
        uint8_t *valor_casteado = (uint8_t *)valor_obtenido_de_memoria.valor;
        *registro_datos_casteado = *valor_casteado;
    } else {
        uint32_t *registro_destino_casteado = (uint32_t *)registro_datos_mapeado;
        uint32_t *registro_origen_casteado = (uint32_t *)valor_obtenido_de_memoria.valor;
        *registro_destino_casteado = *registro_origen_casteado;
    }
    
    // TODO: en vez de registro_direccion_mapeado(DL) no seria valor_casteado(Valor leido)?
    log_info(logger_obligatorio_cpu, "PID: %d - Acción: LEER - Dirección Física: %d - Valor: %d", pid, dir_fisica, *(uint32_t*) registro_direccion_mapeado);
}

void mov_out_instruction(t_list *parametros)
{

    char *registro_datos = (char *)list_get(parametros, 1);
    void *registro_datos_mapeado = mapear_registro(registro_datos);
    char *registro_direccion = (char *)list_get(parametros, 0);
    void *registro_direccion_mapeado = mapear_registro(registro_direccion);

    uint32_t dir_fisica = traducir_direccion_mmu(*(uint32_t*) registro_direccion_mapeado, pid);
    if (dir_fisica == -1)
    {
        log_info(logger_aux_cpu, "HUBO PAGE FAULT");
        return;
    }

    // TODO: Para que quiero el numero de pagina?
    uint32_t num_pagina = numero_pagina(*(uint32_t*) registro_direccion_mapeado);

    tipo_de_dato tipo_de_dato_datos = mapear_tipo_de_dato(registro_datos);

    // TODO: Para que quiero pasar el numero de pagina?
    escribir_en_memoria(dir_fisica, pid, registro_datos_mapeado, tipo_de_dato_datos, num_pagina);

    // TODO: en vez de registro_direccion_mapeado(DL) no seria registro_datos_mapeado(Valor escrito)?
    log_info(logger_obligatorio_cpu, "PID: %d - Acción: ESCRIBIR - Dirección Física: %d - Valor: %d", pid, dir_fisica, *(uint32_t*) registro_direccion_mapeado);
}

void resize_instruction(t_list *parametros)
{

    int size_to_resize = string_to_int((char *)list_get(parametros, 0));

    resize_en_memoria(pid, size_to_resize);
    op_codigo codigoOperacion = recibir_operacion(fd_memoria);
    // TODO: Recibir operacion y fijarse si es Out Of Memory o OK
    log_info(logger_aux_cpu, "PID: %d - Acción: RESIZE - cod op: %d", pid, codigoOperacion);
}

int* peticion_de_paginas(uint32_t *cantidad_bytes) {
    uint32_t nueva_dir_fisica_si = registros_cpu.si;
    uint32_t num_pagina = numero_pagina(registros_cpu.si);
    uint32_t tam_pagina_restante = cantidad_bytes;
    int desplazamiento = registros_cpu.si - num_pagina * tam_pagina;
    int i = 0;

    int* dir_fisica_si = malloc(sizeof(int));
    
    // Para el caso que se pase del tamanio (cantidad_bytes)
    do {
        // Traducimos direccion logica a direccion fisica y aumentamos indice, y chequeamos si falta leer algo
        dir_fisica_si[i] = traducir_direccion_mmu(nueva_dir_fisica_si, pid);
        i++;
        // if (nueva_dir_fisica_si == -1) {
            // // TODO: Revisar que hacer en caso de error
            // return;
            //}
        if (desplazamiento + cantidad_bytes >= tam_pagina) {
            dir_fisica_si = realloc(dir_fisica_si, sizeof(int) * i + 1);
            tam_pagina_restante = cantidad_bytes - tam_pagina + desplazamiento;
            nueva_dir_fisica_si = nueva_dir_fisica_si + tam_pagina - desplazamiento;
        }
    } while (tam_pagina_restante > 0);
    
    return dir_fisica_si;
}

void copy_string_instruction (t_list *parametros){
    uint32_t *cantidad_bytes = (uint32_t *)list_get(parametros, 0);
    int* dir_fisica_si = peticion_de_paginas(cantidad_bytes);
    
    // Traducimos direccion logica de DI a direccion fisica
    // uint32_t dir_fisica_di = traducir_direccion_mmu(registros_cpu.di, pid);
    // if (dir_fisica_di == -1) {
    //     // TODO: Revisar que hacer en caso de error
    //     return;
    // }

    //t_valor_obtenido_de_memoria valor_obtenido_de_memoria = leer_de_memoria(dir_fisica_si, pid);
    
    //TO-DO: free + seguir con esta funcion

}

void exit_instruction(t_list *parametros)
{
    manejarInterrupciones(INTERRUPCION_FIN_EVENTO);
}

// Mapeo y lectura de instrucciones (decode)

t_tipo_instruccion mapear_tipo_instruccion(char *nombre_instruccion)
{

    t_tipo_instruccion tipo_instruccion_mapped;

    if (string_equals_ignore_case(nombre_instruccion, "SET"))
    {
        tipo_instruccion_mapped.nombre_instruccion = SET;
        tipo_instruccion_mapped.execute = set_instruction;
    }
    else if (string_equals_ignore_case(nombre_instruccion, "SUM"))
    {
        tipo_instruccion_mapped.nombre_instruccion = SUM;
        tipo_instruccion_mapped.execute = sum_instruction;
    }
    else if (string_equals_ignore_case(nombre_instruccion, "SUB"))
    {
        tipo_instruccion_mapped.nombre_instruccion = SUB;
        tipo_instruccion_mapped.execute = sub_instruction;
    }
    else if (string_equals_ignore_case(nombre_instruccion, "MOV_IN"))
    {
        tipo_instruccion_mapped.nombre_instruccion = MOV_IN;
        tipo_instruccion_mapped.execute = mov_in_instruction;
    }
    else if (string_equals_ignore_case(nombre_instruccion, "MOV_OUT"))
    {
        tipo_instruccion_mapped.nombre_instruccion = MOV_OUT;
        tipo_instruccion_mapped.execute = mov_out_instruction;
    }
    else if (string_equals_ignore_case(nombre_instruccion, "RESIZE"))
    {
        tipo_instruccion_mapped.nombre_instruccion = RESIZE;
        tipo_instruccion_mapped.execute = resize_instruction;
    }
    else if (string_equals_ignore_case(nombre_instruccion, "JNZ"))
    {
        tipo_instruccion_mapped.nombre_instruccion = JNZ;
        tipo_instruccion_mapped.execute = jnz_instruction;
    }
    else if (string_equals_ignore_case(nombre_instruccion, "COPY_STRING"))
        tipo_instruccion_mapped.nombre_instruccion = COPY_STRING;
    else if (string_equals_ignore_case(nombre_instruccion, "IO_GEN_SLEEP"))
    {
        tipo_instruccion_mapped.nombre_instruccion = IO_GEN_SLEEP;
        tipo_instruccion_mapped.execute = io_gen_sleep_instruction;
    }
    else if (string_equals_ignore_case(nombre_instruccion, "IO_STDIN_READ"))
        tipo_instruccion_mapped.nombre_instruccion = IO_STDIN_READ;
    else if (string_equals_ignore_case(nombre_instruccion, "IO_STDOUT_WRITE"))
        tipo_instruccion_mapped.nombre_instruccion = IO_STDOUT_WRITE;
    else if (string_equals_ignore_case(nombre_instruccion, "IO_FS_CREATE"))
        tipo_instruccion_mapped.nombre_instruccion = IO_FS_CREATE;
    else if (string_equals_ignore_case(nombre_instruccion, "IO_FS_DELETE"))
        tipo_instruccion_mapped.nombre_instruccion = IO_FS_DELETE;
    else if (string_equals_ignore_case(nombre_instruccion, "IO_FS_TRUNCATE"))
        tipo_instruccion_mapped.nombre_instruccion = IO_FS_TRUNCATE;
    else if (string_equals_ignore_case(nombre_instruccion, "IO_FS_WRITE"))
        tipo_instruccion_mapped.nombre_instruccion = IO_FS_WRITE;
    else if (string_equals_ignore_case(nombre_instruccion, "IO_FS_READ"))
        tipo_instruccion_mapped.nombre_instruccion = IO_FS_READ;
    else if (string_equals_ignore_case(nombre_instruccion, "WAIT"))
        tipo_instruccion_mapped.nombre_instruccion = WAIT;
    else if (string_equals_ignore_case(nombre_instruccion, "SIGNAL"))
        tipo_instruccion_mapped.nombre_instruccion = SIGNAL;
    else if (string_equals_ignore_case(nombre_instruccion, "EXIT"))
    {
        tipo_instruccion_mapped.nombre_instruccion = EXIT_PROGRAM;
        tipo_instruccion_mapped.execute = exit_instruction;
    }

    return tipo_instruccion_mapped;
}

t_instruccion *new_instruction(t_tipo_instruccion tipo_instruccion, t_list *parametros)
{
    t_instruccion *tmp = malloc(sizeof(t_instruccion));
    tmp->tipo_instruccion = tipo_instruccion;
    tmp->cant_parametros = list_size(parametros);
    tmp->parametros = parametros;
    return tmp;
}

t_instruccion *procesar_instruccion(char *instruccion_entrante)
{
    // Nos quedamos con el string hasta encontrar el \n
    char *parsed_instruccion_entrante = strtok(instruccion_entrante, "\n");
    // Separamos los tokens (nombre de instruccion y parametros) SET AX BX
    char **tokens = string_split(parsed_instruccion_entrante, " ");
    // Obtenemos el nombre de la instruccion
    char *identificador = tokens[0];
    t_tipo_instruccion tipo_instruccion = mapear_tipo_instruccion(identificador);
    // Agregamos a la lista los parametros de la instruccion
    t_list *parameters = list_create();
    int i = 1; // A partir de 1 son parametros - La lista puede estar vacia.
    char *registro_mapeado;
    char *parametros_string = string_new();
    while (tokens[i] != NULL)
    {
        registro_mapeado = tokens[i];
        string_append_with_format(&parametros_string, "%s ", registro_mapeado);
        list_add(parameters, registro_mapeado);
        i++;
    }
    t_instruccion *instruccion_obtenida = new_instruction(tipo_instruccion, parameters);
    instruccion_obtenida->parametros_string = parametros_string;
    free(identificador);
    free(tokens);
    return instruccion_obtenida;
}

// Funcion para eliminar instruccion de memoria

void liberar_instruccion()
{
    liberar_lista_de_datos_planos(instruccion->parametros);
    free(instruccion);
}