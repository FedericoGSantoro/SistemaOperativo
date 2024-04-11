#include "./kernel.h"

int main(int argc, char* argv[]) {
    logs_informacion = log_create("logsExtras.log", "[EXTRA]", true, LOG_LEVEL_INFO);
    config = iniciar_config(rutaConfiguracion);
    if ( config == NULL ) {
        config_destroy(config);
        abort();
    }
    leerConfig();
    log_info(logs_informacion, "Configuracion cargada correctamente");
    terminarPrograma();
    return 0;
}

void leerConfig() {
    PUERTO_ESCUCHA = config_get_string_value(config, "PUERTO_ESCUCHA");
    IP_MEMORIA = config_get_string_value(config, "IP_MEMORIA");
    PUERTO_MEMORIA = config_get_string_value(config, "PUERTO_MEMORIA");
    IP_CPU = config_get_string_value(config, "IP_CPU");
    PUERTO_CPU_DISPATCH = config_get_string_value(config, "PUERTO_CPU_DISPATCH");
    PUERTO_CPU_INTERRUPT = config_get_string_value(config, "PUERTO_CPU_INTERRUPT");
    ALGORITMO_PLANIFICACION = config_get_string_value(config, "ALGORITMO_PLANIFICACION");
    QUANTUM = config_get_int_value(config, "QUANTUM");
    RECURSOS = config_get_array_value(config, "RECURSOS");
    char** arrayInstancias = string_array_new();
    arrayInstancias = config_get_array_value(config, "INSTANCIAS_RECURSOS");
    INSTANCIAS_RECURSOS = string_as_int_array(arrayInstancias);
    GRADO_MULTIPROGRAMACION = config_get_int_value(config, "GRADO_MULTIPROGRAMACION");
    string_array_destroy(arrayInstancias);
}

int* string_as_int_array(char** arrayInstancias) {
    int cantidadNumeros = string_array_size(arrayInstancias);
    int* numeros = malloc(sizeof(int) * cantidadNumeros);
    for ( int i = 0; i < cantidadNumeros; i++ ) {
        int numero = atoi(arrayInstancias[i]);
        numeros[i] = numero;
    }
    return numeros;
}

void terminarPrograma() {
    log_destroy(logs_informacion);
    config_destroy(config);
}