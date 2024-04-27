#include "entradasalida.h"

int main(int argc, char* argv[]) {
    //Inicializa todo
    inicializar();
    operacionLeida = readline("OPERACION > ");
    while(strcmp(operacionLeida, "exit")) {
        enviarA = readline("DESTINO > ");
        string_to_upper(enviarA);
        switch ( transformarAOperacion(operacionLeida) ) {
        case MENSAJE:
            tipoOperacion = enviarMsj;
            enviarOperacionA();
            break;
        case PAQUETE:
            tipoOperacion = enviarPaquete;
            enviarOperacionA();
            break;
        default:
            log_error(logger_error, "Comando no reconocido");
            break;
        }
        operacionLeida = readline("OPERACION > ");
    }
    free(operacionLeida);
    free(enviarA);
    terminarPrograma();

    return 0;
}


void enviarMsj(){
    char* comandoLeido = readline("String > ");
    enviar_mensaje(comandoLeido, socketAEnviar);
    log_info(logger_auxiliar, "Mensaje enviado");
    free(comandoLeido);
}

void enviarPaquete() {
    char* comandoLeido;
	t_paquete* paquete = crear_paquete(PAQUETE);

	// Leemos y esta vez agregamos las lineas al paquete
	comandoLeido = readline("String > "); // Leo de consola
	while (strcmp(comandoLeido, "")){ // Mientras no sea cadena vacia
		agregar_a_paquete(paquete, comandoLeido, strlen(comandoLeido)+1); // Agregamos al paquete el stream
		comandoLeido = readline("String > "); // Leo nueva linea
	}
	enviar_paquete(paquete, socketAEnviar); // Enviamos el paquete
    log_info(logger_auxiliar, "Paquete enviado");
	free(comandoLeido);
	eliminar_paquete(paquete);
}

void enviarOperacionA() {
    char* moduloNombre; // No hace falta liberar ya que es cadena literal
    pthread_t hiloAEnviar;
    if ( !strcmp(enviarA, "KERNEL") ) {
        hiloAEnviar = hilo_kernel;
        socketAEnviar = fd_kernel;
        moduloNombre = "Kernel";
    }
    else if ( !strcmp(enviarA, "MEMORIA") ) {
        hiloAEnviar = hilo_memoria;
        socketAEnviar = fd_memoria;
        moduloNombre = "Memoria";
    }
    else {
        log_error(logger_error, "Destino Incorrecto");
        return;
    }
    crearHiloJoin(&hiloAEnviar, (void*) tipoOperacion, NULL, moduloNombre, logger_auxiliar, logger_error);
}

op_codigo transformarAOperacion(char* operacionLeida) {
    string_to_upper(operacionLeida);
    if ( !strcmp(operacionLeida, "MENSAJE") ) { // strcmp devuelve 0 si son iguales
        return MENSAJE;
    } else if ( !strcmp(operacionLeida, "PAQUETE") ) {
        return PAQUETE;
    } else {
        return -1; // Valor por defecto para indicar error
    }
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
    //crearHiloDetach(&hilo_kernel, (void*)enviarMsjKernel, NULL, "Kernel", logger_auxiliar, logger_error);
}

void inicializarConexionMemoria()
{
    fd_memoria = crear_conexion(IP_MEMORIA, PUERTO_MEMORIA, logger_error);
    //crearHiloDetach(&hilo_memoria, (void*)enviarMsjMemoria, NULL, "Memoria", logger_auxiliar, logger_error);

}

void enviarMsjMemoria(){
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
