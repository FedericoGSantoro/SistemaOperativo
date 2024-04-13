#include "./kernel.h"

int main(int argc, char* argv[]){
    // Inicializar variables
    inicializarVariables();
    
    // Handshake
    enviar_handshake();

    // Escucho las conexiones entrantes
    while(escucharServer());

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
    socket_servidor = iniciar_servidor(PUERTO_ESCUCHA, logs_auxiliares);

    // Crear las conexiones hacia cpu y memoria
    if ( crearConexiones() ) {
        log_info(logs_auxiliares, "Conexiones creadas correctamente");
    }
}

bool escucharServer(int socket_servidor) {
    int socket_cliente = esperar_cliente(socket_servidor, logs_auxiliares);
    if ( socket_cliente != -1 ) {
        // TODO: Escuchar peticion del cliente y hacer lo que pide    
        return true;
    }
    return false;
}

void enviar_handshake() {
    enviar_mensaje("Soy Kernel!", fd_memoria);
    enviar_mensaje("Soy Kernel!", fd_cpu_dispatch);
    enviar_mensaje("Soy Kernel!", fd_cpu_interrupt);
}

void crearLogs() {
    logs_auxiliares = log_create("logsExtras.log", "[EXTRA]", true, LOG_LEVEL_INFO);
    logs_obligatorios = log_create("obligatoriosKernel.log", "[OBLIGATORIOS]", false, LOG_LEVEL_INFO);
    // Comprobacion de logs creador correctamente
    if ( logs_auxiliares == NULL || logs_obligatorios == NULL ) {
        terminarPrograma();
        abort();
    }
}

bool crearConexiones() {
    fd_memoria = crear_conexion(IP_MEMORIA, PUERTO_MEMORIA, logs_auxiliares);
    fd_cpu_dispatch = crear_conexion(IP_CPU, PUERTO_CPU_DISPATCH, logs_auxiliares);
    fd_cpu_interrupt = crear_conexion(IP_CPU, PUERTO_CPU_INTERRUPT, logs_auxiliares);
    return true;
}

void iniciarConfig() {
    // Inicializacion configuracion
    config = iniciar_config(rutaConfiguracion);
    // Comprobacion de configuracion inicializada correctamente
    if ( config == NULL ) {
        terminarPrograma();
        abort();
    }
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
    config_destroy(config);
    liberar_conexion(socket_servidor);
    liberar_conexion(fd_memoria);
    liberar_conexion(fd_cpu_dispatch);
    liberar_conexion(fd_cpu_interrupt);
}