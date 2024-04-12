#include "cpu.h"

int main(int argc, char* argv[]) {
    
    // Inicializamos logs
    iniciarLogs();
    
    // Inicializamos configuracion
    iniciarConfig();

    // Leemos los valores del archivo de configuracion y los almacenamos en las variables segun el tipo de dato
    leerConfig();

    // Liberaramos espacio de memoria
    terminarPrograma();

    return 0;
}

void iniciarConfig(){
    configuracion_cpu = iniciar_config(rutaConfiguracionCpu);
    // Comprobamos que se haya creado correctamente
    if (configuracion_cpu == NULL) {
        terminarPrograma();
        abort();
    }
}

void iniciarLogs() {
    logger_obligatorio_cpu = log_create("logsObligatoriosCPU.log", "LOG_OBLIGATORIO_CPU", true, LOG_LEVEL_INFO);
    logger_aux_cpu = log_create("logsExtrasCPU.log", "LOG_EXTRA_CPU", true, LOG_LEVEL_INFO);
    // Comprobamos que los logs se hayan creado correctamente
    if (logger_aux_cpu == NULL || logger_obligatorio_cpu == NULL) {
        terminarPrograma();
        abort();
    }
}

void leerConfig() {
    if (configuracion_cpu != NULL){
        IP_MEMORIA = config_get_string_value(configuracion_cpu, "IP_MEMORIA");
        PUERTO_MEMORIA = config_get_int_value(configuracion_cpu, "PUERTO_MEMORIA");
        PUERTO_ESCUCHA_DISPATCH = config_get_int_value(configuracion_cpu, "PUERTO_ESCUCHA_DISPATCH");
        PUERTO_ESCUCHA_INTERRUPT = config_get_int_value(configuracion_cpu, "PUERTO_ESCUCHA_INTERRUPT");
        CANTIDAD_ENTRADAS_TLB = config_get_int_value(configuracion_cpu, "CANTIDAD_ENTRADAS_TLB");
        ALGORITMO_TLB = config_get_string_value(configuracion_cpu, "ALGORITMO_TLB");
        log_info(logger_aux_cpu, "IP_MEMORIA: %s", IP_MEMORIA);
        log_info(logger_aux_cpu, "PUERTO_MEMORIA: %d", PUERTO_MEMORIA);
        log_info(logger_aux_cpu, "PUERTO_ESCUCHA_DISPATCH: %d", PUERTO_ESCUCHA_DISPATCH);
        log_info(logger_aux_cpu, "PUERTO_ESCUCHA_INTERRUPT: %d", PUERTO_ESCUCHA_INTERRUPT);
        log_info(logger_aux_cpu, "CANTIDAD_ENTRADAS_TLB: %d", CANTIDAD_ENTRADAS_TLB);
        log_info(logger_aux_cpu, "ALGORITMO_TLB: %s", ALGORITMO_TLB);
    } else{
        terminarPrograma();
        abort();
    }        
}

void terminarPrograma() {
    log_destroy(logger_obligatorio_cpu);
    log_destroy(logger_aux_cpu);
    config_destroy(configuracion_cpu);
}