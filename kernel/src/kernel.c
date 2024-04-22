#include "./kernel.h"

int main(int argc, char* argv[]){
    // Inicializar variables
    inicializarVariables();
    
    // Handshake
    enviar_handshake();

    // Inicializar consola interactiva
    iniciarConsolaInteractiva();

    // Inicia la planificacion
    iniciarPlanificacion();

    // Escucho las conexiones entrantes
    while(escucharServer(socket_servidor));

    // Liberar espacio de memoria
    terminarPrograma();
    
    return 0;
}

void iniciarPlanificacion() {
    planificacionLargoPlazo();
    planificacionCortoPlazo();
}

void planificacionCortoPlazo() {
    pthread_t CortoPlazoReady;
    //pthread_t CortoPlazoBlocked;
    //pthread_t CortoPlazoRunning;
    crearHiloDetach(&CortoPlazoReady, (void*) corto_plazo_ready, NULL, "Planificacion corto plazo READY", logs_auxiliares, logs_error);
    //crearHiloDetach(&CortoPlazoBlocked, (void*) corto_plazo_running, NULL, "Planificacion corto plazo RUNNING", logs_auxiliares, logs_error);
    //crearHiloDetach(&CortoPlazoRunning, (void*) corto_plazo_blocked, NULL, "Planificacion corto plazo BLOCKED", logs_auxiliares, logs_error);
}

void corto_plazo_ready() {
    while(1) {
        if ( queue_is_empty(cola_running) ) {
            switch (ALGORITMO_PLANIFICACION) {
            case FIFO:
                queue_push(cola_running, queue_pop(cola_ready));
                //mensaje_cpu_dispatch();
                break;
            case RR:
                break;
            case VRR:
                break;
            default:
                log_error(logs_error, "No se reconocio el algoritmo cargado");
                break;
            }
        }
        
    }
}

void planificacionLargoPlazo() {
    pthread_t LargoPlazoNew;
    pthread_t LargoPlazoExit;
    crearHiloDetach(&LargoPlazoNew, (void*) largo_plazo_new, NULL, "Planificacion largo plazo NEW", logs_auxiliares, logs_error);
    crearHiloDetach(&LargoPlazoExit, (void*) largo_plazo_exit, NULL, "Planificacion largo plazo EXIT", logs_auxiliares, logs_error);
}

void largo_plazo_new() {
    while(1) {
        int programasActuales = queue_size(cola_ready) + queue_size(cola_blocked) + queue_size(cola_running);
        if ( programasActuales < GRADO_MULTIPROGRAMACION ) {
            if ( !queue_is_empty(cola_new) ) {
                queue_push(cola_ready,queue_pop(cola_new));
            }
        }
    }
}

void largo_plazo_exit() {
    while(1) {
        if ( !queue_is_empty(cola_exit) ) {
            if ( eliminar_pcb(queue_peek(cola_exit)) ) {
                queue_pop(cola_exit);
            }
        }
    }
}

bool eliminar_pcb(t_pcb* pcb) {
    return (bool) mensaje_memoria(ELIMINAR_PCB, pcb);
}

void crear_pcb(int quantum, char* nombreConsola, t_punteros_memoria punteros) {
    
    t_pcb* pcb = malloc(sizeof(t_pcb));
    pcb->contexto_ejecucion.pid = pid_siguiente;
    pid_siguiente++;
    pcb->quantum_faltante = quantum; // Como lo recibo o de donde vrga lo saco?
    pcb->io_identifier = nombreConsola; // Cambiar
    pcb->motivo_bloqueo = -1;
    pcb->path_archivo = pathArchivo;
    pcb->contexto_ejecucion.registro_estados = 0;
    iniciarRegistrosCPU(pcb);
    pcb->contexto_ejecucion.punteros_memoria = punteros;
    //pcb->contexto_ejecucion.instruction_pointer = 0; 
    pcb->contexto_ejecucion.registros_cpu.pc = pcb->contexto_ejecucion.punteros_memoria.code_pointer;
    pcb->contexto_ejecucion.state = NEW;
    queue_push(cola_new, pcb);
    log_info(logs_obligatorios, "Se crea el proceso %d en NEW", pcb->contexto_ejecucion.pid);
}

void iniciarRegistrosCPU(t_pcb* pcb) {
    pcb->contexto_ejecucion.registros_cpu.ax = 0;
    pcb->contexto_ejecucion.registros_cpu.bx = 0;
    pcb->contexto_ejecucion.registros_cpu.cx = 0;
    pcb->contexto_ejecucion.registros_cpu.dx = 0;
    pcb->contexto_ejecucion.registros_cpu.eax = 0;
    pcb->contexto_ejecucion.registros_cpu.ebx = 0;
    pcb->contexto_ejecucion.registros_cpu.ecx = 0;
    pcb->contexto_ejecucion.registros_cpu.edx = 0;
    pcb->contexto_ejecucion.registros_cpu.si = 0;
    pcb->contexto_ejecucion.registros_cpu.di = 0;
}

void* mensaje_memoria(op_codigo comandoMemoria, t_pcb* pcb) {
    t_paquete* paquete;
    switch (comandoMemoria) {
    case CREAR_PCB:
        t_punteros_memoria* stream;
        paquete = malloc(sizeof(t_paquete));
        paquete->codigo_operacion = CREAR_PCB;
        paquete->buffer = malloc(sizeof(t_buffer));
        paquete->buffer->size = strlen(pathArchivo) + 1;
        paquete->buffer->stream = malloc(paquete->buffer->size);
        memcpy(paquete->buffer->stream, pathArchivo, paquete->buffer->size);
        enviar_paquete(paquete, fd_memoria);
        recv(fd_memoria, &stream, sizeof(t_punteros_memoria), MSG_WAITALL); //recibo puntero a memoria (t_punteros_memoria)
        eliminar_paquete(paquete);
        return (void*) stream;
        break;
    case ELIMINAR_PCB:
        bool eliminacionCorrecta; 
        paquete = malloc(sizeof(t_paquete));
        paquete->codigo_operacion = ELIMINAR_PCB;
        paquete->buffer = malloc(sizeof(t_buffer));
        paquete->buffer->size = sizeof(t_punteros_memoria);
        paquete->buffer->stream = malloc(paquete->buffer->size);
        memcpy(paquete->buffer->stream, &(pcb->contexto_ejecucion.punteros_memoria), paquete->buffer->size);
        enviar_paquete(paquete, fd_memoria);
        recv(fd_memoria, &eliminacionCorrecta, sizeof(bool), 0); //recibo puntero a memoria (t_punteros_memoria)
        eliminar_paquete(paquete);
        return (void*) eliminacionCorrecta;
        break;
    default:
        log_error(logs_error, "Operacion desconocida");
        break;
    }
}

void inicializarColas() {
    cola_new = queue_create();
    cola_ready = queue_create();
    cola_blocked = queue_create();
    cola_running = queue_create();
    cola_exit = queue_create();
}

void inicializarVariables(){
    // Creacion de logs
    crearLogs();
    
    // Leer y almacenar los datos de la configuracion
    iniciarConfig();

    // Inicializacion servidor
    socket_servidor = iniciar_servidor(PUERTO_ESCUCHA, logs_auxiliares, logs_error);

    // Crear las conexiones hacia cpu y memoria
    // if ( crearConexiones() ) {
    //     log_info(logs_auxiliares, "Conexiones creadas correctamente");
    // }
    // Inicializar colas
    inicializarColas();

}

void iniciarConsolaInteractiva() {
    crearHiloDetach(&thread_consola_interactiva, (void*) atender_consola_interactiva, NULL, "Consola interactiva", logs_auxiliares, logs_error);
}

void atender_consola_interactiva() {
    char** arrayComando;
    char* leido;
    while(1) {
        leido = readline("COMANDO > ");
        if ( !strcmp(leido, "exit") ) {
            free(arrayComando);
            free(leido);
            break;
        }
        add_history(leido);
        arrayComando = string_split(leido, " ");
        ejecutar_comando_consola(arrayComando);
        
    }
}

void ejecutar_comando_consola(char** arrayComando) {
    comando = transformarAOperacion(arrayComando[0]);
    switch (comando) {
    case EJECUTAR_SCRIPT:
        //pathArchivo = arrayComando[1];
        // Que hace?
        log_info(logs_auxiliares, "Script de ' %s ' ejecutado", pathArchivo);
        break;
    case INICIAR_PROCESO:
        char* pathArchivo = arrayComando[1];
        //t_punteros_memoria punteros_memoria = mensaje_memoria();
        //crear_pcb(80, );
        //crearHiloDetach(&thread_memoria, (void*) mensaje_memoria, path, "Memoria", logs_auxiliares, logs_error);
        // Crea el pcb del proceso indicado
        break;
    case FINALIZAR_PROCESO:
        uint32_t pid = atoi(arrayComando[1]);
        // (deberá liberar recursos, archivos y memoria)
        log_info(logs_auxiliares, "Proceso %d finalizado", pid);
        break;
    case DETENER_PLANIFICACION:
        // Variable planificacion_ejecutando = false;
        // El proceso que se encuentra en ejecución NO es desalojado, 
        // pero una vez que salga de EXEC se va a pausar el manejo de su motivo de desalojo. 
        // De la misma forma, los procesos bloqueados van a pausar su transición a la cola de Ready.
        break;
    case INICIAR_PLANIFICACION:
        // Variable planificacion_ejecutando = true;
        break;
    case MULTIPROGRAMACION:
        GRADO_MULTIPROGRAMACION = atoi(arrayComando[1]);
        log_info(logs_auxiliares, "Grado de multiprogramacion cambiado a: %d", GRADO_MULTIPROGRAMACION);
        break;
    case PROCESO_ESTADO:
        // Listar los procesos por su estado
        break;
    default:
        log_info(logs_auxiliares, "Comando desconocido");
        break;
    }
}

comando_consola transformarAOperacion(char* operacionLeida) {
    string_to_upper(operacionLeida);
    if ( !strcmp(operacionLeida, "EJECUTAR_SCRIPT") ) { // strcmp devuelve 0 si son iguales
        return EJECUTAR_SCRIPT;
    } else if ( !strcmp(operacionLeida, "INICIAR_PROCESO") ) {
        return INICIAR_PROCESO;
    } else if ( !strcmp(operacionLeida, "FINALIZAR_PROCESO") ) {
        return FINALIZAR_PROCESO;
    } else if ( !strcmp(operacionLeida, "DETENER_PLANIFICACION") ) {
        return DETENER_PLANIFICACION;
    } else if ( !strcmp(operacionLeida, "INICIAR_PLANIFICACION") ) {
        return INICIAR_PLANIFICACION;
    } else if ( !strcmp(operacionLeida, "MULTIPROGRAMACION") ) {
        return MULTIPROGRAMACION;
    } else if ( !strcmp(operacionLeida, "PROCESO_ESTADO") ) {
        return PROCESO_ESTADO;
    } else {
        return -1; // Valor por defecto para indicar error
    }
}

void atender_cliente(void* argumentoVoid) {
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
        case CREAR_PCB:
            // Recibir nombre de la consola y pathArchivo (Global)
            char* nombre;
            t_punteros_memoria* valorIntermedio =(t_punteros_memoria*) mensaje_memoria(CREAR_PCB, NULL);
            t_punteros_memoria punteros_memoria = *valorIntermedio;
            crear_pcb(80, nombre, punteros_memoria);
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
    //crearHiloDetach(&thread_memoria, (void*) atender_cliente, (void*) &fd_memoria, "Memoria", logs_auxiliares, logs_error);

    fd_cpu_dispatch = crear_conexion(IP_CPU, PUERTO_CPU_DISPATCH, logs_error);
    //crearHiloDetach(&thread_cpu_dispatch, (void*) atender_cliente, (void*) &fd_cpu_dispatch, "CPU Dispatch", logs_auxiliares, logs_error);

    fd_cpu_interrupt = crear_conexion(IP_CPU, PUERTO_CPU_INTERRUPT, logs_error);
    //crearHiloDetach(&thread_cpu_interrupt, (void*) atender_cliente, (void*) &fd_cpu_interrupt, "CPU Interrupt", logs_auxiliares, logs_error);

    return true;
}

void iniciarConfig() {
    // Inicializacion configuracion
    config = iniciar_config(rutaConfiguracion, logs_error, (void*)terminarPrograma); 
    leerConfig();
    return;
}

alg_planificacion obtenerAlgoritmo() {
    char* algoritmo = config_get_string_value(config, "ALGORITMO_PLANIFICACION");
    if ( !strcmp(algoritmo, "FIFO") ) {
        return FIFO;
    } else if ( !strcmp(algoritmo, "RR") ) {
        return RR;
    } else if ( !strcmp(algoritmo, "VRR") ) {
        return VRR;
    } else {
        log_error(logs_error, "Algoritmo no reconocido: %s", algoritmo);
        abort();
    }
}

void leerConfig() {
    PUERTO_ESCUCHA = config_get_string_value(config, "PUERTO_ESCUCHA");
    IP_MEMORIA = config_get_string_value(config, "IP_MEMORIA");
    PUERTO_MEMORIA = config_get_string_value(config, "PUERTO_MEMORIA");
    IP_CPU = config_get_string_value(config, "IP_CPU");
    PUERTO_CPU_DISPATCH = config_get_string_value(config, "PUERTO_CPU_DISPATCH");
    PUERTO_CPU_INTERRUPT = config_get_string_value(config, "PUERTO_CPU_INTERRUPT");
    ALGORITMO_PLANIFICACION = obtenerAlgoritmo();
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
    queue_destroy(cola_new);
    queue_destroy(cola_ready);
    queue_destroy(cola_running);
    queue_destroy(cola_blocked);
    queue_destroy(cola_exit);
}