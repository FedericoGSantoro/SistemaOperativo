#include "cpu.h"

int main(int argc, char* argv[]) {
    
    cpu_logger = log_create("prueba.log", "CPU_LOG", true, LOG_LEVEL_INFO);
    if (cpu_logger == NULL){
        abort();
    }

    configuracion = iniciar_config(rutaConfiguracionCpu);
    leerConfig();
    log_destroy(cpu_logger);
    config_destroy(configuracion);

    return 0;
}

// Extraemos del archivo de configuracion
void leerConfig(){
    if (configuracion != NULL){
        IP_MEMORIA = config_get_string_value(configuracion, "IP_MEMORIA");
        PUERTO_MEMORIA = config_get_int_value(configuracion, "PUERTO_MEMORIA");
        PUERTO_ESCUCHA_DISPATCH = config_get_int_value(configuracion, "PUERTO_ESCUCHA_DISPATCH");
        PUERTO_ESCUCHA_INTERRUPT = config_get_int_value(configuracion, "PUERTO_ESCUCHA_INTERRUPT");
        CANTIDAD_ENTRADAS_TLB = config_get_int_value(configuracion, "CANTIDAD_ENTRADAS_TLB");
        ALGORITMO_TLB = config_get_string_value(configuracion, "ALGORITMO_TLB");
    } else{
        abort();
    }        
}