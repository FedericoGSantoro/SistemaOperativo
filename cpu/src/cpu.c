#include "cpu.h"

int main(int argc, char* argv[]) {
    
    iniciarLogs();

    iniciarConfig();
    leerConfig();

    iniciarServidoresCpu();
    iniciarConexionCpuMemoria();
    while(esperarClientes());
    
    terminarPrograma();

    return 0;
}

void iniciarLogs() {
    logger_obligatorio_cpu = log_create("logsObligatoriosCPU.log", "LOG_OBLIGATORIO_CPU", true, LOG_LEVEL_INFO);
    logger_aux_cpu = log_create("logsExtrasCPU.log", "LOG_EXTRA_CPU", true, LOG_LEVEL_INFO);
    logger_error_cpu = log_create("logsExtrasCPU.log", "LOG_ERROR_CPU", true, LOG_LEVEL_INFO);
    // Comprobamos que los logs se hayan creado correctamente
    if ( logger_aux_cpu == NULL || logger_obligatorio_cpu == NULL || logger_error_cpu == NULL ) {
        terminarPrograma();
        abort();
    }
}

void iniciarConfig() {
    configuracion_cpu = iniciar_config(rutaConfiguracionCpu, logger_error_cpu, (void*)terminarPrograma);
}

void leerConfig() {
    if (configuracion_cpu != NULL){
        IP_MEMORIA = config_get_string_value(configuracion_cpu, "IP_MEMORIA");
        PUERTO_MEMORIA = config_get_string_value(configuracion_cpu, "PUERTO_MEMORIA");
        PUERTO_ESCUCHA_DISPATCH = config_get_string_value(configuracion_cpu, "PUERTO_ESCUCHA_DISPATCH");
        PUERTO_ESCUCHA_INTERRUPT = config_get_string_value(configuracion_cpu, "PUERTO_ESCUCHA_INTERRUPT");
        CANTIDAD_ENTRADAS_TLB = config_get_int_value(configuracion_cpu, "CANTIDAD_ENTRADAS_TLB");
        ALGORITMO_TLB = config_get_string_value(configuracion_cpu, "ALGORITMO_TLB");
    } else{
        terminarPrograma();
        abort();
    }        
}

void iniciarServidoresCpu() {
    fd_cpu_dispatch = iniciar_servidor(PUERTO_ESCUCHA_DISPATCH, logger_aux_cpu, logger_error_cpu);
    fd_cpu_interrupt = iniciar_servidor(PUERTO_ESCUCHA_INTERRUPT, logger_aux_cpu, logger_error_cpu);
}

void enviarMsjMemoria(){
    enviar_mensaje("Hola, soy CPU!", fd_memoria);
}

void iniciarConexionCpuMemoria() {
    fd_memoria = crear_conexion(IP_MEMORIA, PUERTO_MEMORIA, logger_error_cpu);
    enviarMsjMemoria();
    //crearHiloDetach(&hilo_memoria_cpu, (void*)enviarMsjMemoria, NULL, "Memoria", logger_aux_cpu, logger_error_cpu);
}

void iteradorPaquete(char* value) {
	log_info(logger_aux_cpu,"%s", value);
}

void atenderKernelDispatch() {
    bool aux_control = 1;
    
    // While infinito mientras kernel dispatch este conectado al servidor
    // Sale del while cuando se desconecta o si se encuentra con una operacion desconocida
    while (aux_control) {
		int cod_op = recibir_operacion(fd_kernel_dispatch);
		switch (cod_op) {
		case MENSAJE:
			recibir_mensaje(fd_kernel_dispatch, logger_aux_cpu);
			break;
        case PAQUETE:
            t_list* valoresPaquete = recibir_paquete(fd_kernel_dispatch);
            list_iterate(valoresPaquete, (void*) iteradorPaquete);
            break;
        case CONTEXTO_EJECUCION:
            recvContextoEjecucion();
            log_info(logger_aux_cpu, "Recibi el contexto de ejecucion");
            log_info(logger_aux_cpu, "Inicio ciclo de instruccion");
            // IN-PROGRESS ejecutar ciclo de instruccion
            //ejecutarCicloInstruccion();

            break;
        // Case -1 para salir del while infinito
		case -1:
			log_error(logger_aux_cpu, "Desconexion de Kernel Modo Dispatch");
			// Al setear en 0 en la proxima iteracion ya no entra en el while y sigue ejecutandose el programa
            aux_control = 0;
            break;
		default:
			log_warning(logger_aux_cpu,"Operacion desconocida de Kernel Modo Dispatch");
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
			recibir_mensaje(fd_kernel_interrupt, logger_aux_cpu);
			break;
        case PAQUETE:
            t_list* valoresPaquete = recibir_paquete(fd_kernel_interrupt);
            list_iterate(valoresPaquete, (void*) iteradorPaquete);
            break;
        // case INTERRUPCION_FIN_EVENTO:
        //     break;
        // case INTERRUPCION_RELOJ:
        //     break;
        // case LLAMADA_SISTEMA:
        //     break;
		case -1:
			log_error(logger_aux_cpu, "Desconexion de Kernel Modo Interrupt");
            aux_control = 0;
            break;
		default:
			log_warning(logger_aux_cpu,"Operacion desconocida de Kernel Modo Interrupt");
			break;
		}
	}
}

bool esperarClientes() {
    fd_kernel_dispatch = esperar_cliente(fd_cpu_dispatch, logger_aux_cpu, logger_error_cpu);
    fd_kernel_interrupt = esperar_cliente(fd_cpu_interrupt, logger_aux_cpu, logger_error_cpu);
    if (fd_kernel_dispatch != -1 && fd_kernel_interrupt != -1){
        // Posibilidad de crear hilo join con Kernel Dispatch - REVISAR
        crearHiloDetach(&hilo_kernel_dispatch_cpu, (void*)atenderKernelDispatch, NULL, "Kernel Dispatch", logger_aux_cpu, logger_error_cpu);
        crearHiloDetach(&hilo_kernel_interrumpt_cpu, (void*)atenderKernelInterrupt, NULL, "Kernel Interrupt", logger_aux_cpu, logger_error_cpu);
        return true;
    }
    return false;
}

void desempaquetarContextoEjecucion(t_list* paquete) {
    pid = list_get(paquete, 0);
    registro_estados = list_get(paquete, 1);
    registros_cpu.pc = list_get(paquete, 2);
    registros_cpu.ax = list_get(paquete, 3);
    registros_cpu.bx = list_get(paquete, 4);
    registros_cpu.cx = list_get(paquete, 5);
    registros_cpu.dx = list_get(paquete, 6);
    registros_cpu.eax = list_get(paquete, 7);
    registros_cpu.ebx = list_get(paquete, 8);
    registros_cpu.ecx = list_get(paquete, 9);
    registros_cpu.edx = list_get(paquete, 10);
    registros_cpu.si = list_get(paquete, 11);
    registros_cpu.di = list_get(paquete, 12);
    motivo_bloqueo = (blocked_reason) list_get(paquete, 13);

    //TO-DO: para no bloquearnos con lo que sigue, probar primero empaquetar contexto y enviarlo a kernel
}

void recvContextoEjecucion() {
    t_list* paquete = recibir_paquete(fd_kernel_dispatch);
    desempaquetarContextoEjecucion(paquete);
    list_destroy(paquete);
}

void fetch(int fd_memoria) {

    // TO-DO: usar la funcion crear paquete aca en vez de hacerlo a mano
    uint32_t stream; // tamanio del pc
    t_paquete* paquete = malloc(sizeof(t_paquete));
    paquete->codigo_operacion = FETCH_INSTRUCCION;
    paquete->buffer = malloc(sizeof(t_buffer));
    paquete->buffer->size = sizeof(uint32_t);
    paquete->buffer->stream = malloc(paquete->buffer->size);
    memcpy(paquete->buffer->stream, registros_cpu.pc, paquete->buffer->size);
    enviar_paquete(paquete, fd_memoria);
    eliminar_paquete(paquete);

    // Recibimos de memoria el pc y lo guardamos en ir
    t_list* pc = recibir_paquete(fd_memoria);
    t_registros_cpu* pc_aux = list_get(pc, 0);
    ir = pc_aux->pc;
    list_destroy(pc);
    free(pc_aux);

    // Aumento en 1 para que apunte a la siguiente instruccion
    registros_cpu.pc++;
}

void atenderMemoria(op_codigo codigoMemoria) {
    switch (codigoMemoria) {
    case FETCH_INSTRUCCION:
            fetch(fd_memoria);
        break;
    default:
        log_warning(logger_aux_cpu,"Operacion desconocida de Memoria");
        break;
    }
}

void ejecutarCicloInstruccion() {

    // Fetch (captura):
    // Se busca la proxima instruccion a ejecutar
    // La instruccion a ajecutar se le pide a Memoria utilizando la direccion de memoria del programa (contador de programa) para determinar qué instrucción se debe leer.
    crearHiloJoin(&hilo_memoria_cpu, (void*)atenderMemoria, FETCH_INSTRUCCION, "Memoria", logger_aux_cpu, logger_error_cpu);

    // Decode (decodificacion):
    // Se interpreta que instrucción es la que se va a ejecutar y si la misma requiere de una traduccion de direccion logica a direccion fisica.

    // Execute (ejecucion):
    // Se ejecuta lo correspondiente a cada instruccion

    // Check Interrupt:
    // Se revisa si Kernel nos envio una interupcion al PID que se está ejecutando. En caso afirmativo, se envía a Kernel el Contexto de ejecucion con el motivo de la interrupcion (mediante el kernel dispatch).
    // Posible solucion con while, mientras no me interrumpan ejecutas ciclo de instruccion, sino sale del while y se envia contexto a Kernel
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
}