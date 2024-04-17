#include "./kernel.h"

int main(int argc, char* argv[]){
    // Inicializar variables
    inicializarVariables();
    
    // Handshake
    enviar_handshake();

    // Escucho las conexiones entrantes
    while(escucharServer(socket_servidor));

    // Liberar espacio de memoria
    terminarPrograma();
    
    return 0;
}

void inicializarVariables(){
    // Creacion de logs
    crearLogs();
    
    // Leer y almacenar los datos de la configuracion
    iniciarConfig();

    // Inicializacion servidor
    socket_servidor = iniciar_servidor(PUERTO_ESCUCHA, logs_auxiliares, logs_error);

    // Crear las conexiones hacia cpu y memoria
    if ( crearConexiones() ) {
        log_info(logs_auxiliares, "Conexiones creadas correctamente");
    }
}

void atender_cliente(void* argumentoVoid){
    // Paso el void* a int*
    int* argumentoInt = (int*) argumentoVoid;
    // Me quedo con el dato del int*
    int socket_cliente = *argumentoInt;
    int codigoOperacion;
    // Reviso que haya conexion
    while( socket_cliente != -1 ) {
        // Leo el codigo de operacion enviado
        codigoOperacion = recibir_operacion(socket_cliente);
        if ( codigoOperacion == -1 ) {
            log_warning(logs_auxiliares, "El cliente se desconecto de Kernel");
            return;
        }
        switch (codigoOperacion) {
        case MENSAJE:
            recibir_mensaje(socket_cliente, logs_auxiliares);
            break;
        case PAQUETE:
            t_list* valoresPaquete = recibir_paquete(socket_cliente);
            list_iterate(valoresPaquete, (void*) iteradorPaquete);
            break;
        default:
            log_error(logs_error, "Codigo de operacion no reconocido: %d", codigoOperacion);
            break;
        }
    }
}

void iteradorPaquete(char* value) {
	log_info(logs_auxiliares,"%s", value);
}

bool escucharServer(int socket_servidor) {
    // Escucho el socket constantemente ya que es bloqueante
    int socket_cliente = esperar_cliente(socket_servidor, logs_auxiliares, logs_error);
    // Si aparece alguien:
    if ( socket_cliente != -1 ) {
        // Creo hilo y le asigno atender_cliente pasandole el socket como parametro
        pthread_t thread_cliente;
        crearHiloDetach(&thread_cliente, (void*) atender_cliente, (void*) &socket_cliente, "Cliente", logs_auxiliares, logs_error);
        return true;
    }
    return false;
}

void enviar_handshake() {
    // Envio los mensajes iniciales
    enviar_mensaje("Soy Kernel!", fd_memoria);
    enviar_mensaje("Soy Kernel!", fd_cpu_dispatch);
    enviar_mensaje("Soy Kernel!", fd_cpu_interrupt);
}

void crearLogs() {
    logs_auxiliares = log_create("logsExtras.log", "[EXTRA]", true, LOG_LEVEL_INFO);
    logs_obligatorios = log_create("obligatoriosKernel.log", "[OBLIGATORIOS]", false, LOG_LEVEL_INFO);
    logs_error = log_create("logsExtras.log", "[ERROR]", true, LOG_LEVEL_INFO);
    // Comprobacion de logs creador correctamente
    if ( logs_auxiliares == NULL || logs_obligatorios == NULL || logs_error == NULL) {
        terminarPrograma();
        abort();
    }
}

bool crearConexiones() {

    fd_memoria = crear_conexion(IP_MEMORIA, PUERTO_MEMORIA, logs_error);
    crearHiloDetach(&thread_memoria, (void*) atender_cliente, (void*) &fd_memoria, "Memoria", logs_auxiliares, logs_error);

    fd_cpu_dispatch = crear_conexion(IP_CPU, PUERTO_CPU_DISPATCH, logs_error);
    crearHiloDetach(&thread_cpu_dispatch, (void*) atender_cliente, (void*) &fd_cpu_dispatch, "CPU Dispatch", logs_auxiliares, logs_error);

    fd_cpu_interrupt = crear_conexion(IP_CPU, PUERTO_CPU_INTERRUPT, logs_error);
    crearHiloDetach(&thread_cpu_interrupt, (void*) atender_cliente, (void*) &fd_cpu_interrupt, "CPU Interrupt", logs_auxiliares, logs_error);

    return true;
}

void iniciarConfig() {
    // Inicializacion configuracion
    config = iniciar_config(rutaConfiguracion, logs_error, (void*)terminarPrograma); 
    leerConfig();
    return;
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
    INSTANCIAS_RECURSOS = string_array_as_int_array(arrayInstancias);
    GRADO_MULTIPROGRAMACION = config_get_int_value(config, "GRADO_MULTIPROGRAMACION");
    string_array_destroy(arrayInstancias);
    log_info(logs_auxiliares, "Configuracion cargada correctamente");
}

int* string_array_as_int_array(char** arrayInstancias) {
    int cantidadNumeros = string_array_size(arrayInstancias);
    int* numeros = malloc(sizeof(int) * cantidadNumeros);
    for ( int i = 0; i < cantidadNumeros; i++ ) {
        int numero = atoi(arrayInstancias[i]);
        numeros[i] = numero;
    }
    return numeros;
}

void terminarPrograma() {
    log_destroy(logs_obligatorios);
    log_destroy(logs_auxiliares);
    log_destroy(logs_error);
    config_destroy(config);
    liberar_conexion(socket_servidor);
    liberar_conexion(fd_memoria);
    liberar_conexion(fd_cpu_dispatch);
    liberar_conexion(fd_cpu_interrupt);
}