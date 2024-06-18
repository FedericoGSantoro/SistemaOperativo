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

    if (tipo_de_dato_destino == UINT8 && tipo_de_dato_origen == UINT8) {
        uint8_t *registro_destino_casteado = (uint8_t *)registro_destino;
        uint8_t *registro_origen_casteado = (uint8_t *)registro_origen;
        *registro_destino_casteado += *registro_origen_casteado;
        return;
    }

    if (tipo_de_dato_destino == UINT32 && tipo_de_dato_origen == UINT8) {
        uint32_t *registro_destino_casteado = (uint32_t *)registro_destino;
        uint8_t *registro_origen_casteado = (uint8_t *)registro_origen;
        *registro_destino_casteado += *registro_origen_casteado;
        return;
    }

    if (tipo_de_dato_destino == UINT8 && tipo_de_dato_origen == UINT32) {
        uint8_t *registro_destino_casteado = (uint8_t *)registro_destino;
        uint32_t *registro_origen_casteado = (uint32_t *)registro_origen;
        *registro_destino_casteado += *registro_origen_casteado;
        return;
    }

    uint32_t *registro_destino_casteado = (uint32_t *)registro_destino;
    uint32_t *registro_origen_casteado = (uint32_t *)registro_origen;
    *registro_destino_casteado += *registro_origen_casteado;
}

void sub_instruction(t_list *parametros)
{

    char *destino = (char *)list_get(parametros, 0);
    char *origen = (char *)list_get(parametros, 1);

    void *registro_destino = mapear_registro(destino);
    void *registro_origen = mapear_registro(origen);

    tipo_de_dato tipo_de_dato_destino = mapear_tipo_de_dato(destino);
    tipo_de_dato tipo_de_dato_origen = mapear_tipo_de_dato(origen);

    if (tipo_de_dato_destino == UINT8 && tipo_de_dato_origen == UINT8) {
        uint8_t *registro_destino_casteado = (uint8_t *)registro_destino;
        uint8_t *registro_origen_casteado = (uint8_t *)registro_origen;
        *registro_destino_casteado -= *registro_origen_casteado;
        return;
    }

    if (tipo_de_dato_destino == UINT32 && tipo_de_dato_origen == UINT8) {
        uint32_t *registro_destino_casteado = (uint32_t *)registro_destino;
        uint8_t *registro_origen_casteado = (uint8_t *)registro_origen;
        *registro_destino_casteado -= *registro_origen_casteado;
        return;
    }

    if (tipo_de_dato_destino == UINT8 && tipo_de_dato_origen == UINT32) {
        uint8_t *registro_destino_casteado = (uint8_t *)registro_destino;
        uint32_t *registro_origen_casteado = (uint32_t *)registro_origen;
        *registro_destino_casteado -= *registro_origen_casteado;
        return;
    }

    uint32_t *registro_destino_casteado = (uint32_t *)registro_destino;
    uint32_t *registro_origen_casteado = (uint32_t *)registro_origen;
    *registro_destino_casteado -= *registro_origen_casteado;
}

void set_instruction(t_list *parametros)
{

    char *destino = (char *)list_get(parametros, 0);
    char *origen = (char *)list_get(parametros, 1);

    void *registro_destino = mapear_registro(destino);
    void *registro_origen = mapear_registro(origen);

    tipo_de_dato tipo_de_dato_destino = mapear_tipo_de_dato(destino);
    tipo_de_dato tipo_de_dato_origen = mapear_tipo_de_dato(origen);

    if (tipo_de_dato_destino == UINT8 && tipo_de_dato_origen == UINT8) {
        uint8_t *registro_destino_casteado = (uint8_t *)registro_destino;
        uint8_t *registro_origen_casteado = (uint8_t *)registro_origen;
        *registro_destino_casteado = *registro_origen_casteado;
        return;
    }

    if (tipo_de_dato_destino == UINT32 && tipo_de_dato_origen == UINT8) {
        uint32_t *registro_destino_casteado = (uint32_t *)registro_destino;
        uint8_t *registro_origen_casteado = (uint8_t *)registro_origen;
        *registro_destino_casteado = *registro_origen_casteado;
        return;
    }

    if (tipo_de_dato_destino == UINT8 && tipo_de_dato_origen == UINT32) {
        uint8_t *registro_destino_casteado = (uint8_t *)registro_destino;
        uint32_t *registro_origen_casteado = (uint32_t *)registro_origen;
        *registro_destino_casteado = *registro_origen_casteado;
        return;
    }

    uint32_t *registro_destino_casteado = (uint32_t *)registro_destino;
    uint32_t *registro_origen_casteado = (uint32_t *)registro_origen;
    *registro_destino_casteado = *registro_origen_casteado;
}

void jnz_instruction(t_list *parametros)
{

    char *registro = (char *)list_get(parametros, 0);
    char *instruccion_a_moverse = (char *)list_get(parametros, 1);

    void *registro_mapeado = mapear_registro(registro);
    uint32_t instruccion_a_moverse_mapeada = *(uint32_t*)mapear_registro(instruccion_a_moverse);
    tipo_de_dato tipo_de_dato_registro = mapear_tipo_de_dato(registro);
    uint32_t registro_casteado;

    if (tipo_de_dato_registro == UINT8) {
        uint8_t registro_valor = *(uint8_t*) (registro_mapeado);
        registro_casteado = registro_valor;
    } else {
        registro_casteado = *(uint32_t*) (registro_mapeado);

    }

    if (registro_casteado == 0) {
        return;
    }

    registros_cpu.pc = instruccion_a_moverse_mapeada - 1; // restamos uno porque despues vamos a sumar uno con el proximo PC++, al finalizar un ciclo de instruccion que tenga a JNZ. Me llevo a consultar si queda en bulce infinito, o como cortamos el JNZ.
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

uint32_t get_direccion_fisica (void *registro_direccion_mapeado, tipo_de_dato tipo_de_dato_registro_direccion) {
    
    uint32_t registro_direccion_casteado;

    if (tipo_de_dato_registro_direccion == UINT8) {
        uint8_t registro_direccion_valor = *(uint8_t*) (registro_direccion_mapeado);
        registro_direccion_casteado = registro_direccion_valor;
    } else {
        registro_direccion_casteado = *(uint32_t*) (registro_direccion_mapeado);
    }

    return traducir_direccion_mmu(registro_direccion_casteado);
}

void mov_in_instruction(t_list *parametros)
{

    char *registro_datos = (char *)list_get(parametros, 0);
    void *registro_datos_mapeado = mapear_registro(registro_datos);
    char *registro_direccion = (char *)list_get(parametros, 1);
    void *registro_direccion_mapeado = mapear_registro(registro_direccion);
    tipo_de_dato tipo_de_dato_registro_direccion = mapear_tipo_de_dato(registro_datos);
    uint32_t dir_fisica = get_direccion_fisica(registro_direccion_mapeado, tipo_de_dato_registro_direccion);
    
    if (dir_fisica == -1)
    {
        // TODO: Revisar que hacer en caso de error
        return;
    }

    tipo_de_dato tipo_de_dato_datos = mapear_tipo_de_dato(registro_datos);
    uint32_t tamanio_a_leer_en_memoria;
    switch (tipo_de_dato_datos){
        case UINT32:
        tamanio_a_leer_en_memoria = sizeof(uint32_t);
        break;
        case UINT8:
        tamanio_a_leer_en_memoria = sizeof(uint8_t);
        break;
        case STRING:
        tamanio_a_leer_en_memoria = string_length(registro_datos) + 1;
        break;
    }
    
    void* valor_obtenido_de_memoria = leer_de_memoria(dir_fisica, pid, tamanio_a_leer_en_memoria);

    switch (tipo_de_dato_datos) {
        case UINT8:
            uint8_t *valor_obtenido_mapeado1 = (uint8_t *)valor_obtenido_de_memoria;
            uint8_t *registro_datos_casteado1 = (uint8_t *)registro_datos_mapeado;
            *registro_datos_casteado1 = *valor_obtenido_mapeado1;
            log_info(logger_obligatorio_cpu, "PID: %d - Acción: LEER - Dirección Física: %d - Valor: %d", pid, dir_fisica, *registro_datos_casteado1);
        break;
        case UINT32:
            uint32_t *valor_obtenido_mapeado = (uint32_t *)valor_obtenido_de_memoria;
            uint32_t *registro_datos_casteado = (uint32_t *)registro_datos_mapeado;
            *registro_datos_casteado = *valor_obtenido_mapeado;
            log_info(logger_obligatorio_cpu, "PID: %d - Acción: LEER - Dirección Física: %d - Valor: %d", pid, dir_fisica, *registro_datos_casteado);
        break;
    }
}

void mov_out_instruction(t_list *parametros)
{

    char *registro_datos = (char *)list_get(parametros, 1);
    void *registro_datos_mapeado = mapear_registro(registro_datos);
    char *registro_direccion = (char *)list_get(parametros, 0);
    void *registro_direccion_mapeado = mapear_registro(registro_direccion);
    tipo_de_dato tipo_de_dato_registro_direccion = mapear_tipo_de_dato(registro_datos);
    //uint32_t dir_fisica = get_direccion_fisica(registro_direccion_mapeado, tipo_de_dato_registro_direccion);
    
    tipo_de_dato tipo_de_dato_datos = mapear_tipo_de_dato(registro_datos);
    uint32_t cantidad_bytes;
    char* valor;
    switch (tipo_de_dato_datos){
        case UINT32:
        cantidad_bytes = sizeof(uint32_t);
        uint32_t registro_datos_a_parsear1 = *(uint32_t*) registro_datos_mapeado;
        valor = int_to_string(registro_datos_a_parsear1);
        break;
        case UINT8:
        cantidad_bytes = sizeof(uint8_t);
        uint8_t registro_datos_a_parsear2 = *(uint8_t*) registro_datos_mapeado;
        valor = int_to_string(registro_datos_a_parsear2);
        break;
        case STRING:
        cantidad_bytes = string_length(registro_datos) + 1;
        valor = (char*) registro_datos_mapeado;
        break;
    }

    t_list* devolucion_direcciones_fisicas = peticion_de_direcciones_fisicas(&cantidad_bytes, UINT32, registro_direccion_mapeado, tipo_de_dato_registro_direccion); //estas direcciones SIEMPRE debe haber almenos una
    
    escribir_en_memoria(devolucion_direcciones_fisicas, pid, registro_datos_mapeado, cantidad_bytes);

    for (int i = 0; i < list_size(devolucion_direcciones_fisicas); i++) {

        log_info(logger_obligatorio_cpu, "PID: %d - Acción: ESCRIBIR - Dirección Física: %d - Valor: %s", pid, *(uint32_t*) list_get(devolucion_direcciones_fisicas, i), valor);
    }

    liberar_lista_de_datos_con_punteros(devolucion_direcciones_fisicas);
}

void resize_instruction(t_list *parametros)
{

    int size_to_resize = string_to_int((char *)list_get(parametros, 0));

    resize_en_memoria(pid, size_to_resize);
    op_codigo codigoOperacion = recibir_operacion(fd_memoria);
    // TODO: Recibir operacion y fijarse si es Out Of Memory o OK
    if (codigoOperacion == OUT_OF_MEMORY){
        manejarInterrupciones(INTERRUPCION_FIN_EVENTO); // Deberia ir INTERRUPCION_OUT_OF_MEMORY
        log_error(logger_error_cpu, "Out of Memory Pa, baneado proceso");
        return;
    } //no considero necesario un chequeo de OK_OPERACION pq si
    log_info(logger_aux_cpu, "PID: %d - Acción: RESIZE - cod op: %d", pid, codigoOperacion);
}

void agregar_direccion_fisica_a_lista(uint32_t* dir_fis){
    t_params_io *parametro_io = malloc(sizeof(int) * 2);
    parametro_io->tipo_de_dato = UINT32;
    parametro_io->valor = malloc(sizeof(uint32_t));
    *(uint32_t*)parametro_io->valor = *dir_fis;
    log_info(logger_aux_cpu, "Direccion fisica cargandose: %d", *(uint32_t*) parametro_io->valor);
    list_add(io_detail.parametros, parametro_io);
    //no se si hace falta un free() uwu
} 

void io_stdout_write_instruction(t_list *parametros){ //honores to: capo master fede (s, no w)
    //leo parametros
    //recibe:(Interfaz, Registro Dirección, Registro Tamaño)
    char *nombre_io = (char *)list_get(parametros, 0);
    char *registro_direccion = (char *)list_get(parametros, 1);
    char *registro_tamanio = (char *)list_get(parametros, 2);
    //mapeo
    void* reg_dir = mapear_registro(registro_direccion);
    void* reg_tam = mapear_registro(registro_tamanio);
    
    //armo el array con las direcs fis. y agrego a los parametros c/ posicion
    tipo_de_dato tipo_de_dato_registro_direccion = mapear_tipo_de_dato(registro_direccion);
    tipo_de_dato tipo_de_dato_registro_bytes = mapear_tipo_de_dato(registro_tamanio);
    t_list* dir_fisicas = peticion_de_direcciones_fisicas(reg_tam, tipo_de_dato_registro_bytes, reg_dir, tipo_de_dato_registro_direccion);

    for (int i = 0; i < list_size(dir_fisicas); i++){
        agregar_direccion_fisica_a_lista((uint32_t*) list_get(dir_fisicas, i));
    }
    //agrego el valor del tamaño a leer por ultimo
    t_params_io *parametro_io_tamanio = malloc(sizeof(int) * 2);
    parametro_io_tamanio->tipo_de_dato = INT;
    parametro_io_tamanio->valor = reg_tam;
    list_add(io_detail.parametros, parametro_io_tamanio);
    //cargo nombre instruccion
    t_nombre_instruccion *io_instruccion = malloc(sizeof(int));
    *io_instruccion = IO_STDOUT_WRITE;
    io_detail.io_instruccion = *io_instruccion;
    //cargo nombre io
    io_detail.nombre_io = nombre_io;

    manejarInterrupciones(LLAMADA_SISTEMA);
    free(io_instruccion);
    liberar_lista_de_datos_con_punteros(dir_fisicas);
}

void io_stdin_read_instruction(t_list *parametros){ //honores to: capo master fede (s, no w)
    //leo parametros
    //recibe:(Interfaz, Registro Dirección, Registro Tamaño)
    char *nombre_io = (char *)list_get(parametros, 0);
    char *registro_direccion = (char *)list_get(parametros, 1);
    char *registro_tamanio = (char *)list_get(parametros, 2);
    //mapeo
    void* reg_dir = mapear_registro(registro_direccion);
    void* reg_tam = mapear_registro(registro_tamanio);
    
    //armo el array con las direcs fis. y agrego a los parametros c/ posicion
    tipo_de_dato tipo_de_dato_registro_direccion = mapear_tipo_de_dato(registro_direccion);
    tipo_de_dato tipo_de_dato_registro_bytes = mapear_tipo_de_dato(registro_tamanio);
    t_list* dir_fisicas = peticion_de_direcciones_fisicas(reg_tam, tipo_de_dato_registro_bytes, reg_dir, tipo_de_dato_registro_direccion);

    for (int i = 0; i < list_size(dir_fisicas); i++){
        agregar_direccion_fisica_a_lista((uint32_t*) list_get(dir_fisicas, i));
    }
    //agrego el valor del tamaño a leer por ultimo
    t_params_io *parametro_io_tamanio = malloc(sizeof(int) * 2);
    parametro_io_tamanio->tipo_de_dato = INT;
    parametro_io_tamanio->valor = reg_tam;
    list_add(io_detail.parametros, parametro_io_tamanio);
    //cargo nombre instruccion
    t_nombre_instruccion *io_instruccion = malloc(sizeof(int));
    *io_instruccion = IO_STDIN_READ;
    io_detail.io_instruccion = *io_instruccion;
    //cargo nombre io
    io_detail.nombre_io = nombre_io;

    manejarInterrupciones(LLAMADA_SISTEMA);
    free(io_instruccion);
    liberar_lista_de_datos_con_punteros(dir_fisicas);
}
    
void copy_string_instruction (t_list *parametros) {
    char* parametro_numerico = (char *)list_get(parametros, 0);
    uint32_t* cantidad_bytes = mapear_registro(parametro_numerico);
    uint32_t* registro_si = mapear_registro("SI");
    uint32_t* registro_di = mapear_registro("DI");
    tipo_de_dato tipo_de_dato_registro_si = mapear_tipo_de_dato("SI");
    tipo_de_dato tipo_de_dato_registro_di = mapear_tipo_de_dato("DI");
    t_list* dir_fisicas_si = peticion_de_direcciones_fisicas(cantidad_bytes, UINT32 , registro_si, tipo_de_dato_registro_si);
    t_list* dir_fisicas_di = peticion_de_direcciones_fisicas(cantidad_bytes, UINT32 , registro_di, tipo_de_dato_registro_di);
    void* valor_obtenido_de_memoria;
    char* leido = string_new();
    
    // Obtenemos las direcciones fisicas de SI y DI
    if ( list_size(dir_fisicas_di) == 0 || list_size(dir_fisicas_si) == 0 ) {
        log_error(logger_aux_cpu, "Error al traducir direcciones físicas de SI o DI");
        liberar_lista_de_datos_con_punteros(dir_fisicas_di);
        liberar_lista_de_datos_con_punteros(dir_fisicas_si);
        return;
    }

    // Leemos de memoria desde las direcciones físicas apuntadas por SI
    for (int i = 0; i < list_size(dir_fisicas_si); i++) {
        // Obtenemos la direccion fisica
        uint32_t* direccion_fisica = (uint32_t*)list_get(dir_fisicas_si, i);
        // Calculamos los bytes que podemos leer de esa direccion fisica
        uint32_t cantidad_a_leer = cantidad_bytes_que_se_pueden_leer(*direccion_fisica);
        // Si lo que podemos leer es menor a la cantidad que nos falta por leer leemos todo lo que podemos
        // Si no, leemos lo que nos falta
        if ( cantidad_a_leer < *cantidad_bytes ) {
            valor_obtenido_de_memoria = leer_de_memoria(*direccion_fisica, pid, cantidad_a_leer);
            cantidad_bytes -= cantidad_a_leer;
        } else {
            valor_obtenido_de_memoria = leer_de_memoria(*direccion_fisica, pid, *cantidad_bytes);
        }

        string_append_with_format(&leido, "%s", (char*) valor_obtenido_de_memoria);
    }

    // Obtenemos de nuevo la cantidad total a escribir
    cantidad_bytes = mapear_registro(parametro_numerico);
    //Escribimos el contenido leído en las direcciones físicas apuntadas por DI
    escribir_en_memoria(dir_fisicas_di, pid, leido, *cantidad_bytes);

    // Liberar la memoria asignada
    free(leido);
    liberar_lista_de_datos_con_punteros(dir_fisicas_di);
    liberar_lista_de_datos_con_punteros(dir_fisicas_si);
}

void wait_instruction(t_list *parametros) {
    // Recibimos el recurso
    char *recurso = (char *)list_get(parametros, 0);

    // Cargamos el nombre de la instruccion
    t_nombre_instruccion *io_instruccion = malloc(sizeof(int));
    *io_instruccion = WAIT;
    io_detail.io_instruccion = *io_instruccion;
    // Cargamos el recurso
    io_detail.nombre_io = recurso;

    manejarInterrupciones(LLAMADA_SISTEMA);
    free(io_instruccion);
}

void signal_instruction(t_list *parametros) {
    // Recibimos el recurso
    char *recurso = (char *)list_get(parametros, 0);

    // Cargamos el nombre de la instruccion
    t_nombre_instruccion *io_instruccion = malloc(sizeof(int));
    *io_instruccion = SIGNAL;
    io_detail.io_instruccion = *io_instruccion;
    // Cargamos el recurso
    io_detail.nombre_io = recurso;

    manejarInterrupciones(LLAMADA_SISTEMA);
    free(io_instruccion);
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
    else if (string_equals_ignore_case(nombre_instruccion, "COPY_STRING")) {
        tipo_instruccion_mapped.nombre_instruccion = COPY_STRING;
        tipo_instruccion_mapped.execute = copy_string_instruction;
    }
    else if (string_equals_ignore_case(nombre_instruccion, "IO_GEN_SLEEP"))
    {
        tipo_instruccion_mapped.nombre_instruccion = IO_GEN_SLEEP;
        tipo_instruccion_mapped.execute = io_gen_sleep_instruction;
    }
    else if (string_equals_ignore_case(nombre_instruccion, "IO_STDIN_READ")){
        tipo_instruccion_mapped.nombre_instruccion = IO_STDIN_READ;
        tipo_instruccion_mapped.execute = io_stdin_read_instruction;
    }
    else if (string_equals_ignore_case(nombre_instruccion, "IO_STDOUT_WRITE")){
        tipo_instruccion_mapped.nombre_instruccion = IO_STDOUT_WRITE;
        tipo_instruccion_mapped.execute = io_stdout_write_instruction;
    }
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
    else if (string_equals_ignore_case(nombre_instruccion, "WAIT")) {
        tipo_instruccion_mapped.nombre_instruccion = WAIT;
        tipo_instruccion_mapped.execute = wait_instruction;
    }
    else if (string_equals_ignore_case(nombre_instruccion, "SIGNAL")) {
        tipo_instruccion_mapped.nombre_instruccion = SIGNAL;
        tipo_instruccion_mapped.execute = signal_instruction;
    }
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