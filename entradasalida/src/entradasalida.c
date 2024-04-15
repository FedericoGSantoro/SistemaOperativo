#include "entradasalida.h"

int main(int argc, char* argv[]) {
    
    inicializar();
    system("sleep 5");

    terminarPrograma();

    return 0;
}

void inicializar(){
    inicializarLogs();

    inicializarConfig();

    inicializarConexiones();
}

void inicializarLogs(){
    logger_obligatorio = log_create("entradasalida.log", "LOG_OBLIGATORIO_ENTRADA-SALIDA", true, LOG_LEVEL_INFO);
    logger_auxiliar = log_create("entradasalidaExtras.log", "LOG_EXTRA_ENTRADA_SALIDA", true, LOG_LEVEL_INFO);
    logger_error = log_create("entradasalidaExtras.log", "LOG_ERROR_ENTRADA_SALIDA", true, LOG_LEVEL_ERROR);
    // Compruebo que los logs se hayan creado correctamente
    if (logger_auxiliar == NULL || logger_obligatorio == NULL || logger_error == NULL) {
        terminarPrograma();
        abort();
    }
}

void inicializarConfig(){
    configuracion = iniciar_config(rutaConfiguracion, logger_error, (void*)terminarPrograma);
    leerConfig();
}

void leerConfig() {
        TIPO_INTERFAZ = config_get_string_value(configuracion, "TIPO_INTERFAZ");
        TIEMPO_UNIDAD_TRABAJO = config_get_int_value(configuracion, "TIEMPO_UNIDAD_TRABAJO");
        IP_KERNEL = config_get_string_value(configuracion, "IP_KERNEL");
        PUERTO_KERNEL = config_get_string_value(configuracion, "PUERTO_KERNEL");
        IP_MEMORIA = config_get_string_value(configuracion, "IP_MEMORIA");
        PUERTO_MEMORIA = config_get_string_value(configuracion, "PUERTO_MEMORIA");
        PATH_BASE_DIALFS = config_get_string_value(configuracion, "PATH_BASE_DIALFS");
        BLOCK_SIZE = config_get_int_value(configuracion, "BLOCK_SIZE");
        BLOCK_COUNT = config_get_int_value(configuracion, "BLOCK_COUNT");
}

void inicializarConexiones(){
    inicializarConexionKernel();
    inicializarConexionMemoria();
}

void inicializarConexionKernel()
{
    fd_kernel = crear_conexion(IP_KERNEL, PUERTO_KERNEL, logger_error);
    crearHilo(&hilo_kernel, (void*)enviarMsjKernel, NULL, "Kernel", logger_auxiliar, logger_error);
}

void inicializarConexionMemoria()
{
    fd_memoria = crear_conexion(IP_MEMORIA, PUERTO_MEMORIA, logger_error);
    crearHilo(&hilo_memoria, (void*)enviarMsjMemoria, NULL, "Memoria", logger_auxiliar, logger_error);

}

void enviarMsjMemoria(){
    log_info(logger_auxiliar, "ENTRE BIEN");
    enviar_mensaje("Hola, soy I/O!", fd_memoria);
}

void enviarMsjKernel(){
    enviar_mensaje("Hola, soy I/O!", fd_kernel);
}

void terminarPrograma() {
    log_destroy(logger_obligatorio);
    log_destroy(logger_auxiliar);
    log_destroy(logger_error);
    config_destroy(configuracion);
    liberar_conexion(fd_memoria);
    liberar_conexion(fd_kernel);
}
