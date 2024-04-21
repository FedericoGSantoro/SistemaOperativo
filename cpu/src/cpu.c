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

void iniciarConexionCpuMemoria() {
    fd_memoria = crear_conexion(IP_MEMORIA, PUERTO_MEMORIA, logger_error_cpu);
    crearHiloJoin(&hilo_memoria_cpu, (void*)enviarMsjMemoria, NULL, "Memoria", logger_aux_cpu, logger_error_cpu);
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
            recv_contexto_ejecucion(fd_kernel_dispatch);
            // TO-DO ejecutar ciclo de instruccion

            // TO-DO enviar el contexto de ejecucion a Kernel si interrumpen a la CPU
            // Posible solucion con while, mientras no me interrumpan ejecutas ciclo de instruccion, sino sale del while y se envia contexto a Kernel
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

void iteradorPaquete(char* value) {
	log_info(logger_aux_cpu,"%s", value);
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
        case INTERRUPCION_FIN_EVENTO:
            break;
        case INTERRUPCION_RELOJ:
            break;
        case LLAMADA_SISTEMA:
            break;
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

void atenderMemoria() {
    bool aux_control = 1;

    while (aux_control) {
		int cod_op = recibir_operacion(fd_memoria);
		switch (cod_op) {
		case MENSAJE:
			recibir_mensaje(fd_memoria, logger_aux_cpu);
			break;
        case PAQUETE:
            t_list* valoresPaquete = recibir_paquete(fd_memoria);
            list_iterate(valoresPaquete, (void*) iteradorPaquete);
            break;
		case -1:
			log_error(logger_aux_cpu, "Desconexion de Memoria");
            aux_control = 0;
            break;
		default:
			log_warning(logger_aux_cpu,"Operacion desconocida de Memoria");
			break;
		}
	}
}

void enviarMsjMemoria(){
    enviar_mensaje("Hola, soy CPU!", fd_memoria);
}

void desempaquetar_contexto_ejecucion(t_list* paquete) {
    registro_estados = paquete[0]->registro_estados;
    registros_cpu = paquete[0]->registros_cpu;
    instruction_pointer = paquete[0]->registros_cpu->pc;
    // Aumento en 1 para que apunte a la siguiente instruccion
    registros_cpu->pc++;
    punteros_memoria = paquete[0]->punteros_memoria;
}

void recv_contexto_ejecucion(int fd_kernel_dispatch) {
    t_list* paquete = recibir_paquete(fd_kernel_dispatch);

    desempaquetar_contexto_ejecucion(paquete);
    
    list_destroy(paquete);
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