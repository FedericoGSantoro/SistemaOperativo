#include "./includes/cpu.h"

int main(int argc, char *argv[]) {

    iniciarLogs();

    iniciarConfig();
    leerConfig();

    iniciarMutex();
    iniciarServidoresCpu();
    iniciarConexionCpuMemoria();

    getTamanioPagina();

    while (esperarClientes());

    terminarPrograma();

    return 0;
}

void iniciarLogs() {
    logger_obligatorio_cpu = log_create("logsObligatoriosCPU.log", "LOG_OBLIGATORIO_CPU", true, LOG_LEVEL_INFO);
    logger_aux_cpu = log_create("logsExtrasCPU.log", "LOG_EXTRA_CPU", true, LOG_LEVEL_INFO);
    logger_error_cpu = log_create("logsExtrasCPU.log", "LOG_ERROR_CPU", true, LOG_LEVEL_INFO);
    // Comprobamos que los logs se hayan creado correctamente
    if (logger_aux_cpu == NULL || logger_obligatorio_cpu == NULL || logger_error_cpu == NULL) {
        terminarPrograma();
        abort();
    }
}

void iniciarConfig() {
    configuracion_cpu = iniciar_config(rutaConfiguracionCpu, logger_error_cpu, (void *)terminarPrograma);
}

enumAlgoritmo obtenerAlgoritmoTLB(char* algoritmo) {
    if ( string_equals_ignore_case(algoritmo, "FIFO") ) {
        return FIFO;
    } else if ( string_equals_ignore_case(algoritmo, "LRU") ) {
        return LRU;
    } else {
        log_error(logger_error_cpu, "Error al traducir el algoritmo de la TLB: %s", algoritmo);
        terminarPrograma();
        abort();
    }
}

void leerConfig() {
    if (configuracion_cpu != NULL) {
        IP_MEMORIA = config_get_string_value(configuracion_cpu, "IP_MEMORIA");
        PUERTO_MEMORIA = config_get_string_value(configuracion_cpu, "PUERTO_MEMORIA");
        PUERTO_ESCUCHA_DISPATCH = config_get_string_value(configuracion_cpu, "PUERTO_ESCUCHA_DISPATCH");
        PUERTO_ESCUCHA_INTERRUPT = config_get_string_value(configuracion_cpu, "PUERTO_ESCUCHA_INTERRUPT");
        CANTIDAD_ENTRADAS_TLB = config_get_int_value(configuracion_cpu, "CANTIDAD_ENTRADAS_TLB");
        char* algoritmo_leido = config_get_string_value(configuracion_cpu, "ALGORITMO_TLB");
        ALGORITMO_TLB = obtenerAlgoritmoTLB(algoritmo_leido);
        listaEntradasTLB = list_create();
    } else {
        terminarPrograma();
        abort();
    }
}

void iniciarMutex() {
    pthread_mutex_init(&variableInterrupcion, NULL);
}

void iniciarServidoresCpu() {
    fd_cpu_dispatch = iniciar_servidor(PUERTO_ESCUCHA_DISPATCH, logger_aux_cpu, logger_error_cpu);
    fd_cpu_interrupt = iniciar_servidor(PUERTO_ESCUCHA_INTERRUPT, logger_aux_cpu, logger_error_cpu);
}

void getTamanioPagina() {
    enviar_codigo_op(DEVOLVER_TAM_PAGINA, fd_memoria);
    recibir_operacion(fd_memoria);
    char* tam_pagina_obtenido = recibir_mensaje(fd_memoria);
    tam_pagina = string_to_int(tam_pagina_obtenido);
}

void enviarMsjMemoria() {
    enviar_mensaje("Hola, soy CPU!", fd_memoria);
}

void iniciarConexionCpuMemoria() {
    fd_memoria = crear_conexion(IP_MEMORIA, PUERTO_MEMORIA, logger_error_cpu);
    enviarMsjMemoria();
}

void iteradorPaquete(char *value) {
    log_info(logger_aux_cpu, "%s", value);
}

void iniciar_ciclo_instruccion() {
    recvContextoEjecucion();
    log_info(logger_aux_cpu, "Recibi el contexto de ejecucion!");

    // Mientras no exista interrupcion de kernel se ejecuta un ciclo de instruccion, sino sale del while y se envia contexto a Kernel
    // Leemos el estado de la interrupcion utilizando mutex por si el hilo Kernel Interrupt está modificando la variable
    pthread_mutex_lock(&variableInterrupcion);
    while (motivo_bloqueo == NOTHING) {    
        log_info(logger_aux_cpu, "motivo viejo: %d", motivo_bloqueo);
        pthread_mutex_unlock(&variableInterrupcion);
        log_info(logger_aux_cpu, "Inicio ciclo de instruccion");
        ejecutarCicloInstruccion();
        // Aumento en 1 al final del ciclo para que apunte a la siguiente instruccion
        log_info(logger_aux_cpu, "valores registros importantes AX: %d BX: %d PC: %d", registros_cpu.ax, registros_cpu.bx, registros_cpu.pc);
        registros_cpu.pc++;
        pthread_mutex_lock(&variableInterrupcion);
    }
    log_info(logger_aux_cpu, "motivo nuevo: %d", motivo_bloqueo);
    pthread_mutex_unlock(&variableInterrupcion);
    // Empaquetamos el contexto de ejecucion y se lo enviamos a Kernel
    enviarContextoEjecucion();
    
    log_info(logger_aux_cpu, "Envie el contexto de ejecucion!");
}

void atenderKernelDispatch() {
    bool aux_control = 1;

    // While infinito mientras kernel dispatch este conectado al servidor
    // Sale del while cuando se desconecta o si se encuentra con una operacion desconocida
    while (aux_control) {
        int cod_op = recibir_operacion(fd_kernel_dispatch);
        switch (cod_op) {
            case MENSAJE:
                char *mensaje = recibir_mensaje(fd_kernel_dispatch);
                log_info(logger_aux_cpu, "Me llegó el mensaje %s", mensaje);
                free(mensaje);
                break;
            case PAQUETE:
                t_list *valoresPaquete = recibir_paquete(fd_kernel_dispatch);
                list_iterate(valoresPaquete, (void *)iteradorPaquete);
                list_destroy(valoresPaquete);
                break;
            case CONTEXTO_EJECUCION:
                iniciar_ciclo_instruccion();
                break;
            // Case -1 para salir del while infinito
            case -1:
                log_error(logger_aux_cpu, "Desconexion de Kernel Modo Dispatch");
                // Al setear en 0 en la proxima iteracion ya no entra en el while y sigue ejecutandose el programa
                aux_control = 0;
                break;
            default:
                log_warning(logger_aux_cpu, "Operacion desconocida de Kernel Modo Dispatch");
                break;
        }
    }
}

void atenderKernelInterrupt() {
    bool aux_control = 1;

    while (aux_control) {
        int cod_op = recibir_operacion(fd_kernel_interrupt);
        switch (cod_op) {
            case MENSAJE:
                char *mensaje = recibir_mensaje(fd_kernel_interrupt);
                log_info(logger_aux_cpu, "Me llegó el mensaje %s", mensaje);
                free(mensaje);
                break;
            case PAQUETE:
                t_list *valoresPaquete = recibir_paquete(fd_kernel_interrupt);
                list_iterate(valoresPaquete, (void *)iteradorPaquete);
                list_destroy(valoresPaquete);
                break;
            // Caso de INTERRUPCION_RELOJ:
            case INTERRUPCION:
                recvInterrupcion();
                log_info(logger_aux_cpu, "Me llegó una interrupción!");
                break;
            case -1:
                log_error(logger_aux_cpu, "Desconexion de Kernel Modo Interrupt");
                aux_control = 0;
                break;
            default:
                log_warning(logger_aux_cpu, "Operacion desconocida de Kernel Modo Interrupt");
                break;
        }
    }
}

bool esperarClientes() {
    fd_kernel_dispatch = esperar_cliente(fd_cpu_dispatch, logger_aux_cpu, logger_error_cpu);
    fd_kernel_interrupt = esperar_cliente(fd_cpu_interrupt, logger_aux_cpu, logger_error_cpu);
    if (fd_kernel_dispatch != -1 && fd_kernel_interrupt != -1) {
        crearHiloDetach(&hilo_kernel_dispatch_cpu, (void *)atenderKernelDispatch, NULL, "Kernel Dispatch", logger_aux_cpu, logger_error_cpu);
        crearHiloDetach(&hilo_kernel_interrumpt_cpu, (void *)atenderKernelInterrupt, NULL, "Kernel Interrupt", logger_aux_cpu, logger_error_cpu);
        return true;
    }
    return false;
}

void desempaquetarInterrupcion(t_list *paquete) {
    uint32_t pid_aux = *(uint32_t *)list_get(paquete, 0);
    if (pid == pid_aux) {
        manejarInterrupciones(INTERRUPCION_RELOJ);
    }
}

void recvInterrupcion() {
    t_list *paquete = recibir_paquete(fd_kernel_interrupt);
    desempaquetarInterrupcion(paquete);
    list_destroy(paquete);
}

void agregar_io_detail(t_paquete *paquete) {
    agregar_a_paquete(paquete, &(io_detail.parametros->elements_count), sizeof(int));
    log_info(logger_aux_cpu, "Cantidad de parametros: %d", io_detail.parametros->elements_count);
    for (int i = 0; i < io_detail.parametros->elements_count; i++) {
        t_params_io parametro_io = *(t_params_io*)list_get(io_detail.parametros, i);
        int size_parametro;
        void *valor_parametro_a_enviar;
        switch (parametro_io.tipo_de_dato) {
            case INT:
                size_parametro = sizeof(int);
                valor_parametro_a_enviar = malloc(size_parametro);
                *(int *)valor_parametro_a_enviar = *(int *)parametro_io.valor;
                log_info(logger_aux_cpu, "Se envia el parametro int %d", *(int *)valor_parametro_a_enviar);
                break;
            case UINT32:
                size_parametro = sizeof(uint32_t);
                valor_parametro_a_enviar = malloc(size_parametro);
                *(uint32_t *)valor_parametro_a_enviar = *(uint32_t *)parametro_io.valor;
                log_info(logger_aux_cpu, "Se envia el parametro uint32 %d", *(uint32_t *)valor_parametro_a_enviar);
                break;
            case STRING:
                size_parametro = string_length(parametro_io.valor) + 1;
                valor_parametro_a_enviar = malloc(size_parametro);
                valor_parametro_a_enviar = (char *)parametro_io.valor;
                log_info(logger_aux_cpu, "Se envia el parametro string %s", (char *) valor_parametro_a_enviar);
                break;
            default:
                log_error(logger_error_cpu, "Error tipo de dato enviado");
                break;
        }
        agregar_a_paquete(paquete, &parametro_io.tipo_de_dato, sizeof(int));
        agregar_a_paquete(paquete, valor_parametro_a_enviar, size_parametro);
    }
    agregar_a_paquete(paquete, io_detail.nombre_io, strlen(io_detail.nombre_io) + 1);
    agregar_a_paquete(paquete, &io_detail.io_instruccion, sizeof(int));
}

void eliminar_io_detail() {
    if (io_detail.parametros == NULL || io_detail.parametros->elements_count == 0) {
        return;
    }
    
    for (int i = 0; i < io_detail.parametros->elements_count; i++) {
        t_params_io* parametro_a_eliminar = list_get(io_detail.parametros, i);
        free(parametro_a_eliminar->valor);
        free(parametro_a_eliminar);
    }
    list_destroy(io_detail.parametros);
}

void empaquetarContextoEjecucion(t_paquete *paquete) {
    agregar_a_paquete(paquete, &(pid), sizeof(uint32_t));
    agregar_a_paquete(paquete, &(registro_estados), sizeof(uint64_t));
    agregar_a_paquete(paquete, &(registros_cpu.pc), sizeof(uint32_t));
    agregar_a_paquete(paquete, &(registros_cpu.ax), sizeof(uint8_t));
    agregar_a_paquete(paquete, &(registros_cpu.bx), sizeof(uint8_t));
    agregar_a_paquete(paquete, &(registros_cpu.cx), sizeof(uint8_t));
    agregar_a_paquete(paquete, &(registros_cpu.dx), sizeof(uint8_t));
    agregar_a_paquete(paquete, &(registros_cpu.eax), sizeof(uint32_t));
    agregar_a_paquete(paquete, &(registros_cpu.ebx), sizeof(uint32_t));
    agregar_a_paquete(paquete, &(registros_cpu.ecx), sizeof(uint32_t));
    agregar_a_paquete(paquete, &(registros_cpu.edx), sizeof(uint32_t));
    agregar_a_paquete(paquete, &(registros_cpu.si), sizeof(uint32_t));
    agregar_a_paquete(paquete, &(registros_cpu.di), sizeof(uint32_t));
    agregar_a_paquete(paquete, &(motivo_bloqueo), sizeof(int));
    agregar_a_paquete(paquete, &(motivoFinalizacion), sizeof(int));
    agregar_io_detail(paquete);
}

void enviarContextoEjecucion() {
    t_paquete *paquete = crear_paquete(OK_OPERACION);
    empaquetarContextoEjecucion(paquete);
    enviar_paquete(paquete, fd_kernel_dispatch);
    eliminar_io_detail();
    eliminar_paquete(paquete);
}

void desempaquetarContextoEjecucion(t_list *paquete) {
    pid = *(uint32_t *)list_get(paquete, 0);
    registro_estados = *(uint64_t *)list_get(paquete, 1);
    registros_cpu.pc = *(uint32_t *)list_get(paquete, 2);
    registros_cpu.ax = *(uint8_t *)list_get(paquete, 3);
    registros_cpu.bx = *(uint8_t *)list_get(paquete, 4);
    registros_cpu.cx = *(uint8_t *)list_get(paquete, 5);
    registros_cpu.dx = *(uint8_t *)list_get(paquete, 6);
    registros_cpu.eax = *(uint32_t *)list_get(paquete, 7);
    registros_cpu.ebx = *(uint32_t *)list_get(paquete, 8);
    registros_cpu.ecx = *(uint32_t *)list_get(paquete, 9);
    registros_cpu.edx = *(uint32_t *)list_get(paquete, 10);
    registros_cpu.si = *(uint32_t *)list_get(paquete, 11);
    registros_cpu.di = *(uint32_t *)list_get(paquete, 12);
    motivo_bloqueo = *(blocked_reason *)list_get(paquete, 13);
    io_detail.nombre_io = "";
    io_detail.parametros = list_create();
    io_detail.io_instruccion = NONE;
    motivoFinalizacion = NONEXISTENT;
}

void recvContextoEjecucion() {
    t_list *paquete = recibir_paquete(fd_kernel_dispatch);
    desempaquetarContextoEjecucion(paquete);
    list_destroy(paquete);
}

void fetch() {
    t_paquete *paquete = crear_paquete(FETCH_INSTRUCCION);
    agregar_a_paquete(paquete, &(registros_cpu.pc), sizeof(uint32_t));
    agregar_a_paquete(paquete, &(pid), sizeof(uint32_t));
    enviar_paquete(paquete, fd_memoria);
    eliminar_paquete(paquete);

    // Recibimos de memoria la instruccion y lo guardamos en ir
    recibir_operacion(fd_memoria);
    ir = recibir_mensaje(fd_memoria);
    log_info(logger_obligatorio_cpu, "PID: %d - FETCH - Program Counter: %d", pid, registros_cpu.pc);
}

void atenderMemoria(op_codigo codigoMemoria) {
    switch (codigoMemoria) {
        case FETCH_INSTRUCCION:
            fetch();
            break;
        default:
            log_warning(logger_aux_cpu, "Operacion desconocida de Memoria");
            break;
    }
}

void decode() {
    instruccion = procesar_instruccion(ir);
}

void execute() {
    log_info(logger_obligatorio_cpu, "PID: %d - Ejecutando: %s - [%s]", pid, mapeo_nombre_instruccion(instruccion->tipo_instruccion.nombre_instruccion), instruccion->parametros_string);
    instruccion->tipo_instruccion.execute(instruccion->parametros);
}

void ejecutarCicloInstruccion() {
    // Fetch:
    atenderMemoria(FETCH_INSTRUCCION);

    // Decode:
    decode();

    // Execute:
    execute();

    // Eliminar instruccion:
    liberar_instruccion();
}

void terminarPrograma() {
    log_destroy(logger_obligatorio_cpu);
    log_destroy(logger_aux_cpu);
    log_destroy(logger_error_cpu);
    config_destroy(configuracion_cpu);
    liberar_conexion(fd_cpu_dispatch);
    liberar_conexion(fd_cpu_interrupt);
    liberar_conexion(fd_memoria);
    liberar_conexion(fd_kernel_dispatch);
    liberar_conexion(fd_kernel_interrupt);
    pthread_mutex_destroy(&variableInterrupcion);
    liberar_lista_de_datos_con_punteros(listaEntradasTLB);
}
