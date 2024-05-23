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
    pthread_t CortoPlazoBlocked;
    //pthread_t CortoPlazoRunning; // Creo que solo comprueba quantum para desalojar a ready y exit
    crearHiloDetach(&CortoPlazoReady, (void*) corto_plazo_ready, NULL, "Planificacion corto plazo READY", logs_auxiliares, logs_error);
    crearHiloDetach(&CortoPlazoBlocked, (void*) corto_plazo_blocked, NULL, "Planificacion corto plazo RUNNING", logs_auxiliares, logs_error);
    //crearHiloDetach(&CortoPlazoRunning, (void*) corto_plazo_running, NULL, "Planificacion corto plazo BLOCKED", logs_auxiliares, logs_error);
}

void corto_plazo_blocked() {
    while(1) {
        pthread_mutex_lock(&sem_planificacion);
        while( !planificacionEjecutandose ) {
            pthread_cond_wait(&condicion_planificacion, &sem_planificacion);
        }
        pthread_mutex_unlock(&sem_planificacion);
        sem_wait(&semContadorColaBlocked);
        // Nueva nueva idea:
        // Agarro el primer elemento de la cola y le mando a interfaz para que lo solucione y espero su respuesta, cuando la recibo de que termino entonces mando el proceso a ready

        // Nueva idea:
        // Creo que espera a recibir una interrupcion de que finalizo el evento
        // que bloquea un proceso, cuando llega busca ese proceso y lo manda a ready
        
        // t_pcb* pcb = queue_peek(cola_ready);
        
        // Vieja idea:
        // Chequea si el motivo del bloqueo fue solucionado
        // Si esta solucionado lo envia a ready, si no lo deja bloqueado
        
        // Chequea para eliminarlo tambien
    }
}

//cargamos en el contexto del proceso, el io_detail para las operaciones con las entradas y salidas.
void cargar_io_detail_en_context(t_pcb* pcb, t_list* contexto, int ultimo_indice) {

    ultimo_indice++;
    uint32_t cantidad_parametros_io_detail = *(uint32_t*)list_get(contexto, ultimo_indice);

    if (cantidad_parametros_io_detail != 0) {
        pcb->contexto_ejecucion.io_detail.parametros = list_create();

        for (int i = 0; i < cantidad_parametros_io_detail; i++) {

            ultimo_indice++;
            tipo_de_dato tipo_de_dato_parametro_io = *(tipo_de_dato*) list_get(contexto, ultimo_indice);

            t_params_io* parametro_io_a_guardar;
            
            ultimo_indice++;
            void* valor_parametro_io_recibido = (void*) list_get(contexto, ultimo_indice);
            void* valor_parametro_a_guardar;

            switch (tipo_de_dato_parametro_io)
            {
            case INT:
                parametro_io_a_guardar = malloc(sizeof(int)*2);
                valor_parametro_a_guardar = malloc(sizeof(int));
                valor_parametro_a_guardar = (int*)valor_parametro_io_recibido;
                break;

            default:
                break;
            }

            parametro_io_a_guardar->tipo_de_dato = tipo_de_dato_parametro_io; //almaceno el tipo de dato del parametro de la instruccion de io 
            //(esto va a servir mas adelante para que kernel pueda usarlo correctamente, ya que puede recibir char* o int)
            parametro_io_a_guardar->valor = valor_parametro_a_guardar; //almaceno el valor del parametro de la instruccion de io

            list_add_in_index(pcb->contexto_ejecucion.io_detail.parametros, i, parametro_io_a_guardar); //almaceno el parametro en la lista de parametros que usara kernel luego
        }
        ultimo_indice++;
        pcb->contexto_ejecucion.io_detail.nombre_io = (char *)list_get(contexto, ultimo_indice); //obtengo el nombre de la IO
    }
}

void cargar_contexto_recibido(t_list* contexto, t_pcb* pcb) {
    
    int ultimo_indice = 13; //ESTE ULTIMO INDICE ES PARA CONTABILIZAR HASTA EL ULTIMO DE LOS PARAMETROS QUE SON ESTATICOS. }
    //SI AGREGAMOS OTRO PARAMETRO ESTATICO, DEBEMOS INCREMENTAR ESTE NUMERO, YA QUE LUEGO SE DESERIALIZAN DEL PAQUETE CAMPOS QUE SON DINAMICOS (COMO LISTAS)
    
    pcb->contexto_ejecucion.registro_estados = *(uint64_t*)list_get(contexto, 1);
    pcb->contexto_ejecucion.registros_cpu.pc = *(uint32_t*)list_get(contexto, 2);
    pcb->contexto_ejecucion.registros_cpu.ax = *(uint8_t*)list_get(contexto, 3);
    pcb->contexto_ejecucion.registros_cpu.bx = *(uint8_t*)list_get(contexto, 4);
    pcb->contexto_ejecucion.registros_cpu.cx = *(uint8_t*)list_get(contexto, 5);
    pcb->contexto_ejecucion.registros_cpu.dx = *(uint8_t*)list_get(contexto, 6);
    pcb->contexto_ejecucion.registros_cpu.eax = *(uint32_t*)list_get(contexto, 7);
    pcb->contexto_ejecucion.registros_cpu.ebx = *(uint32_t*)list_get(contexto, 8);
    pcb->contexto_ejecucion.registros_cpu.ecx = *(uint32_t*)list_get(contexto, 9);
    pcb->contexto_ejecucion.registros_cpu.edx = *(uint32_t*)list_get(contexto, 10);
    pcb->contexto_ejecucion.registros_cpu.si = *(uint32_t*)list_get(contexto, 11);
    pcb->contexto_ejecucion.registros_cpu.di = *(uint32_t*)list_get(contexto, 12);
    pcb->contexto_ejecucion.motivo_bloqueo = *(blocked_reason*) list_get(contexto, ultimo_indice); //ojo con el ultimo_indice, explicado mas arriba
    cargar_io_detail_en_context(pcb, contexto, ultimo_indice);
    log_info(logs_auxiliares, "AX: %d, BX: %d", pcb->contexto_ejecucion.registros_cpu.ax, pcb->contexto_ejecucion.registros_cpu.bx);
}

t_pcb* quitarPcbCola(t_queue* cola, pthread_mutex_t semaforo) {
    pthread_mutex_lock(&semaforo);
    t_pcb* pcb = queue_pop(cola);
    pthread_mutex_unlock(&semaforo);
    return pcb;
}

void agregarPcbCola(t_queue* cola, pthread_mutex_t semaforo, t_pcb* pcb) {
    pthread_mutex_lock(&semaforo);
    queue_push(cola, pcb);
    pthread_mutex_unlock(&semaforo);
}

void finalizar_proceso(t_pcb* pcb) {
    quitarPcbCola(cola_exec, sem_cola_exec);
    sem_post(&semContadorColaExec);
    agregarPcbCola(cola_exit, sem_cola_exit, pcb);
    sem_post(&semContadorColaExit);
    cambiarEstado(EXIT, pcb);
}

void cambiarContexto(t_list* contexto, t_pcb* pcb) {
    cargar_contexto_recibido(contexto, pcb);

    switch (pcb->contexto_ejecucion.motivo_bloqueo) { 
    case INTERRUPCION_RELOJ:
        quitarPcbCola(cola_exec, sem_cola_exec);
        sem_post(&semContadorColaExec);
        log_info(logs_auxiliares, "cambiarContexto");
        agregarPcbCola(cola_ready, sem_cola_ready, pcb);
        log_info(logs_auxiliares, "cambiarContextoSalida");
        sem_post(&semContadorColaReady);
        cambiarEstado(READY, pcb);
        log_info(logs_obligatorios, "PID: %d - Desalojado por fin de Quantum", pcb->contexto_ejecucion.pid);
        break;
    case LLAMADA_SISTEMA:
        /*quitarPcbCola(cola_exec, sem_cola_exec);
        sem_post(&semContadorColaExec);
        // TODO: Antes de añadir a bloqueados comprobar si esta la interfaz disponible y si lo esta añadirla, creo que cola bloqueado maneja el tema de enviar a las interfaces las cosas para hacer
        agregarPcbCola(cola_blocked, sem_cola_blocked, pcb);
        sem_post(&semContadorColaBlocked);
        cambiarEstado(BLOCKED, pcb);
        */
        
        /*TODO: ELIMINAR LAS LINEAS DE ABAJO Y EL COMENTARIO DE ARRIBA. 
        CAMBIAMOS EL ESTADO PARA SEGUIR EJECUTANDO HASTA QUE ESTE LA 
        LOGICA DE LLAMADA AL SISTEMA*/
        cambiarEstado(EXEC, pcb);
        agregarPcbCola(cola_ready, sem_cola_ready, pcb);
        sem_post(&semContadorColaExec);
        sem_post(&semContadorColaReady);
        planificacionEjecutandose = true;
        pthread_cond_broadcast(&condicion_planificacion);
        break;
    case INTERRUPCION_FIN_EVENTO:
        finalizar_proceso(pcb);
        break;
    default:
        log_error(logs_error, "Error con estado recibido, no reconocido: %d", pcb->contexto_ejecucion.state);
        break;
    }
}

void empaquetar_registros_cpu(t_paquete* paquete, t_pcb* pcb) {
    agregar_a_paquete(paquete, &(pcb->contexto_ejecucion.registros_cpu.pc), sizeof(uint32_t));
    agregar_a_paquete(paquete, &(pcb->contexto_ejecucion.registros_cpu.ax), sizeof(uint8_t));
    agregar_a_paquete(paquete, &(pcb->contexto_ejecucion.registros_cpu.bx), sizeof(uint8_t));
    agregar_a_paquete(paquete, &(pcb->contexto_ejecucion.registros_cpu.cx), sizeof(uint8_t));
    agregar_a_paquete(paquete, &(pcb->contexto_ejecucion.registros_cpu.dx), sizeof(uint8_t));
    agregar_a_paquete(paquete, &(pcb->contexto_ejecucion.registros_cpu.eax), sizeof(uint32_t));
    agregar_a_paquete(paquete, &(pcb->contexto_ejecucion.registros_cpu.ebx), sizeof(uint32_t));
    agregar_a_paquete(paquete, &(pcb->contexto_ejecucion.registros_cpu.ecx), sizeof(uint32_t));
    agregar_a_paquete(paquete, &(pcb->contexto_ejecucion.registros_cpu.edx), sizeof(uint32_t));
    agregar_a_paquete(paquete, &(pcb->contexto_ejecucion.registros_cpu.si), sizeof(uint32_t));
    agregar_a_paquete(paquete, &(pcb->contexto_ejecucion.registros_cpu.di), sizeof(uint32_t));
}

// void empaquetar_punteros_memoria(t_paquete* paquete, t_pcb* pcb) {
//     agregar_a_paquete(paquete, &(pcb->contexto_ejecucion.punteros_memoria.stack_pointer), sizeof(uint64_t));
//     agregar_a_paquete(paquete, &(pcb->contexto_ejecucion.punteros_memoria.heap_pointer), sizeof(uint64_t));
//     agregar_a_paquete(paquete, &(pcb->contexto_ejecucion.punteros_memoria.data_pointer), sizeof(uint64_t));
//     agregar_a_paquete(paquete, &(pcb->contexto_ejecucion.punteros_memoria.code_pointer), sizeof(uint64_t));
// }

void empaquetar_contexto_ejecucion(t_paquete* paquete, t_pcb* pcb) {
    agregar_a_paquete(paquete, &(pcb->contexto_ejecucion.pid), sizeof(uint32_t));
    agregar_a_paquete(paquete, &(pcb->contexto_ejecucion.registro_estados), sizeof(uint64_t));
    empaquetar_registros_cpu(paquete, pcb);
    // empaquetar_punteros_memoria(paquete, pcb);
    agregar_a_paquete(paquete, &(pcb->contexto_ejecucion.state), sizeof(int));
    agregar_a_paquete(paquete, &(pcb->contexto_ejecucion.motivo_bloqueo), sizeof(int));
}

void mensaje_cpu_interrupt() {
    t_paquete* paquete = crear_paquete(INTERRUPCION);
    agregar_a_paquete(paquete, &pcbADesalojar, sizeof(uint32_t));
    enviar_paquete(paquete, fd_cpu_interrupt);
    eliminar_paquete(paquete);
}

void mensaje_cpu_dispatch(op_codigo codigoOperacion, t_pcb* pcb) {
    t_paquete* paquete;
    switch (codigoOperacion) {
    case CONTEXTO_EJECUCION:
        pcb->contexto_ejecucion.motivo_bloqueo = UNKNOWN;
        paquete = crear_paquete(CONTEXTO_EJECUCION);
        empaquetar_contexto_ejecucion(paquete, pcb);
        enviar_paquete(paquete, fd_cpu_dispatch);
        if ( ALGORITMO_PLANIFICACION == VRR || ALGORITMO_PLANIFICACION == RR ) {
            pcbADesalojar = pcb->contexto_ejecucion.pid;
            signal(SIGALRM, mensaje_cpu_interrupt);
            alarm(QUANTUM);
        }
        pcbADesalojar = pcb->contexto_ejecucion.pid;
        signal(SIGALRM, mensaje_cpu_interrupt);
        alarm(1);
        eliminar_paquete(paquete);
        cambiarEstado(EXEC, pcb);
        op_codigo codigoOperacion = recibir_operacion(fd_cpu_dispatch);
        alarm(0);
        if ( codigoOperacion == OK_OPERACION ) {
            t_list* contextoNuevo = recibir_paquete(fd_cpu_dispatch);
            cambiarContexto(contextoNuevo, pcb);
        } else {
            log_error(logs_error, "Problema con la operacion");
        }
        break;
    default:
        break;
    }
}

char* enumEstadoAString(process_state estado) {
    switch (estado) {
    case NEW:
        return "NEW";
    case READY:
        return "READY";
    case EXEC:
        return "EXEC";
    case BLOCKED:
        return "BLOCKED";
    case EXIT:
        return "EXIT";
    default:
        return "ERROR ESTADO";
        break;
    }
}

void cambiarEstado(process_state estadoNuevo, t_pcb* pcb) {
    process_state estadoViejo = pcb->contexto_ejecucion.state;
    log_info(logs_obligatorios, "PID: %d - Estado Anterior: %s - Estado Actual: %s",
            pcb->contexto_ejecucion.pid,
            enumEstadoAString(estadoViejo),
            enumEstadoAString(estadoNuevo)
            );
    switch (estadoNuevo) {
    case READY:
        pcb->contexto_ejecucion.state = READY;
        log_info(logs_obligatorios, "Cola Ready %s: [%s]", config_get_string_value(config, "ALGORITMO_PLANIFICACION"), obtenerPids(cola_ready, sem_cola_ready));
        break;
    case EXEC:
        pcb->contexto_ejecucion.state = EXEC;
        break;
    case BLOCKED:
        pcb->contexto_ejecucion.state = EXEC;
        break;
    case EXIT:
        pcb->contexto_ejecucion.state = EXIT;
        break;
    default:
        break;
    }
    
}

void corto_plazo_ready() {
    t_pcb* pcb;
    while(1) {
        pthread_mutex_lock(&sem_planificacion);
        while( !planificacionEjecutandose ) {
            pthread_cond_wait(&condicion_planificacion, &sem_planificacion);
        }
        pthread_mutex_unlock(&sem_planificacion);
        sem_wait(&semContadorColaExec);
        // if ( ALGORITMO_PLANIFICACION == VRR && !queue_is_empty(cola_ready_aux) ) {
        //     // Agregar bloqueo cola ready aux
        //     pcb = queue_pop(cola_ready_aux);
        //     agregarPcbCola(cola_exec, sem_cola_exec, pcb);
        //     mensaje_cpu_dispatch(CONTEXTO_EJECUCION, pcb);
        //     continue;
        // }
        sem_wait(&semContadorColaReady);
        pcb = quitarPcbCola(cola_ready, sem_cola_ready);
        agregarPcbCola(cola_exec, sem_cola_exec, pcb);
        mensaje_cpu_dispatch(CONTEXTO_EJECUCION, pcb);
    }
}

void planificacionLargoPlazo() {
    pthread_t LargoPlazoNew;
    pthread_t LargoPlazoExit;
    crearHiloDetach(&LargoPlazoNew, (void*) largo_plazo_new, NULL, "Planificacion largo plazo NEW", logs_auxiliares, logs_error);
    crearHiloDetach(&LargoPlazoExit, (void*) largo_plazo_exit, NULL, "Planificacion largo plazo EXIT", logs_auxiliares, logs_error);
}

int elementosEnCola(t_queue* cola, pthread_mutex_t semaforo) {
    int cantidadProgramasEnCola;
    pthread_mutex_lock(&semaforo);
    cantidadProgramasEnCola = queue_size(cola);
    pthread_mutex_unlock(&semaforo);
    return cantidadProgramasEnCola;
}

int elementosEjecutandose() {
    int cantidadProgramas = 0;
    cantidadProgramas += elementosEnCola(cola_ready, sem_cola_ready);
    cantidadProgramas += elementosEnCola(cola_exec, sem_cola_exec);
    cantidadProgramas += elementosEnCola(cola_blocked, sem_cola_blocked);
    return cantidadProgramas;
}

void largo_plazo_new() {
    while(1) {
        pthread_mutex_lock(&sem_planificacion);
        while( !planificacionEjecutandose ) {
            pthread_cond_wait(&condicion_planificacion, &sem_planificacion);
        }
        pthread_mutex_unlock(&sem_planificacion);
        // Sumar la cola de ready aux?
        pthread_mutex_lock(&sem_grado_multiprogramacion);
        if ( elementosEjecutandose() < GRADO_MULTIPROGRAMACION ) {
            pthread_mutex_unlock(&sem_grado_multiprogramacion);
            sem_wait(&semContadorColaNew);
            t_pcb* pcb = quitarPcbCola(cola_new, sem_cola_new);
            mensaje_memoria(CREAR_PCB, pcb);
            agregarPcbCola(cola_ready, sem_cola_ready, pcb);
            sem_post(&semContadorColaReady);
            cambiarEstado(READY, pcb);
        }
    }
}

void largo_plazo_exit() {
    while(1) {
        pthread_mutex_lock(&sem_planificacion);
        while( !planificacionEjecutandose ) {
            pthread_cond_wait(&condicion_planificacion, &sem_planificacion);
        }
        pthread_mutex_unlock(&sem_planificacion);
        sem_wait(&semContadorColaExit);
        t_pcb* pcb = quitarPcbCola(cola_exit, sem_cola_exit);
        eliminar_pcb(pcb);
        /*
        En caso de que el proceso se encuentre ejecutando en CPU, 
        se deberá enviar una señal de interrupción a través de la conexión 
        de interrupt con el mismo y aguardar a que éste retorne el Contexto 
        de Ejecución antes de iniciar la liberación de recursos.
        */
       /*
       LOG OBLIGATORIO:
       Fin de Proceso: "Finaliza el proceso <PID> - 
       Motivo: <SUCCESS / INVALID_RESOURCE / INVALID_WRITE>"
       */
    }
}

void eliminar_io_detail(t_pcb* pcb) {
    
    t_io_detail io_detail_de_contexto = pcb->contexto_ejecucion.io_detail;

    for (int i = 0; i < (io_detail_de_contexto.parametros->elements_count); i++) {
        void* parametro_a_eliminar = list_get(io_detail_de_contexto.parametros, i);
        free(parametro_a_eliminar);
    }
    list_clean(io_detail_de_contexto.parametros);
}

void eliminar_pcb(t_pcb* pcb) {
    mensaje_memoria(ELIMINAR_PCB, pcb);
    eliminar_io_detail(pcb);
    free(pcb);
}

void crear_pcb() {
    t_pcb* pcb = malloc(sizeof(t_pcb));
    pcb->contexto_ejecucion.pid = pid_siguiente;
    pid_siguiente++;
    pcb->quantum_faltante = QUANTUM;
    pcb->io_identifier = numeroConsola;
    numeroConsola++;
    pcb->contexto_ejecucion.motivo_bloqueo = 0;
    pcb->path_archivo = pathArchivo;
    pcb->contexto_ejecucion.registro_estados = 0;
    iniciarRegistrosCPU(pcb);
    // iniciarPunterosMemoria(pcb);
    pcb->contexto_ejecucion.state = NEW;
    agregarPcbCola(cola_new, sem_cola_new, pcb);
    sem_post(&semContadorColaNew);
    log_info(logs_obligatorios, "Se crea el proceso %d en NEW", pcb->contexto_ejecucion.pid);
}

// void iniciarPunterosMemoria(t_pcb* pcb) {
//     pcb->contexto_ejecucion.punteros_memoria.stack_pointer = -1;
//     pcb->contexto_ejecucion.punteros_memoria.heap_pointer = -1;
//     pcb->contexto_ejecucion.punteros_memoria.data_pointer = -1;
//     pcb->contexto_ejecucion.punteros_memoria.code_pointer = -1;
// }

void iniciarRegistrosCPU(t_pcb* pcb) {
    pcb->contexto_ejecucion.registros_cpu.pc = 0;
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

// void asignar_punteros_memoria(t_list* punteros, t_pcb* pcb) {
//     pcb->contexto_ejecucion.punteros_memoria.stack_pointer = *(uint64_t*) list_get(punteros, 0);
//     pcb->contexto_ejecucion.punteros_memoria.heap_pointer = *(uint64_t*) list_get(punteros, 1);
//     pcb->contexto_ejecucion.punteros_memoria.data_pointer = *(uint64_t*) list_get(punteros, 2);
//     pcb->contexto_ejecucion.punteros_memoria.code_pointer = *(uint64_t*) list_get(punteros, 3);
//     pcb->contexto_ejecucion.registros_cpu.pc = (uint32_t)pcb->contexto_ejecucion.punteros_memoria.code_pointer;
// }

void evaluar_respuesta_de_operacion(int fd_cliente, char* nombre_modulo_server, op_codigo codigo_operacion) {

    op_codigo respuesta_recibida = recibir_operacion(fd_cliente);
    switch (respuesta_recibida) {
        case OK_OPERACION:
            log_info(logs_auxiliares, "Operacion OK desde %s", nombre_modulo_server);
            break;
        default:
            log_error(logs_error, "Error al ejecutar operacion %s en %d", nombre_modulo_server, codigo_operacion);
            break;
    }
}

void mensaje_memoria(op_codigo comandoMemoria, t_pcb* pcb) {
    t_paquete* paquete;
    switch (comandoMemoria) {
    case CREAR_PCB:
        paquete = crear_paquete(CREAR_PCB);
        agregar_a_paquete(paquete, &(pcb->contexto_ejecucion.pid), sizeof(uint32_t));
        agregar_a_paquete(paquete, pathArchivo, strlen(pathArchivo) + 1);
        enviar_paquete(paquete, fd_memoria);
        evaluar_respuesta_de_operacion(fd_memoria, MEMORIA_SERVER, CREAR_PCB);
        eliminar_paquete(paquete);
        break;
    case ELIMINAR_PCB:
        paquete = crear_paquete(ELIMINAR_PCB);
        agregar_a_paquete(paquete, &(pcb->contexto_ejecucion.pid), sizeof(uint32_t));
        // empaquetar_punteros_memoria(paquete, pcb);
        enviar_paquete(paquete, fd_memoria);
        evaluar_respuesta_de_operacion(fd_memoria, MEMORIA_SERVER, ELIMINAR_PCB);
        eliminar_paquete(paquete);
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
    cola_exec = queue_create();
    cola_exit = queue_create();
    cola_ready_aux = queue_create();
}

void inicializarSemaforos() {
    pthread_mutex_init(&sem_grado_multiprogramacion, NULL);
    pthread_mutex_init(&sem_planificacion, NULL);
    pthread_cond_init(&condicion_planificacion, NULL);
    pthread_mutex_init(&sem_cola_ready, NULL);
    pthread_mutex_init(&sem_cola_ready_aux, NULL);
    pthread_mutex_init(&sem_cola_new, NULL);
    pthread_mutex_init(&sem_cola_blocked, NULL);
    pthread_mutex_init(&sem_cola_exec, NULL);
    pthread_mutex_init(&sem_cola_exit, NULL);
    sem_init(&semContadorColaNew, 0, 0);
    sem_init(&semContadorColaReady, 0, 0);
    sem_init(&semContadorColaReadyAux, 0, 0);
    sem_init(&semContadorColaExec, 0, 1);
    sem_init(&semContadorColaBlocked, 0, 0);
    sem_init(&semContadorColaExit, 0, 0);
}

void inicializarVariables() {
    // Creacion de logs
    crearLogs();

    // Leer y almacenar los datos de la configuracion
    iniciarConfig();

    // Inicializar semaforos
    inicializarSemaforos();
    
    // Inicializacion servidor
    socket_servidor = iniciar_servidor(PUERTO_ESCUCHA, logs_auxiliares, logs_error);

    // Crear las conexiones hacia cpu y memoria
    if ( crearConexiones() ) {
        log_info(logs_auxiliares, "Conexiones creadas correctamente");
    }
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
        add_history(leido);
        arrayComando = string_split(leido, " ");
        ejecutar_comando_consola(arrayComando);
    }
    free(arrayComando);
    free(leido);
}

char* obtenerPids (t_queue* cola, pthread_mutex_t semaforo) {
    pthread_mutex_lock(&semaforo);
    char* pids = string_new();
    for( int i = 0; i < list_size(cola->elements); i++ ) {
        t_pcb* pcb = list_get(cola->elements, i);
        if ( i < list_size(cola->elements) -1 ) {
            string_append_with_format(&pids, "%d, ", pcb->contexto_ejecucion.pid);
        } else {
            string_append_with_format(&pids, "%d", pcb->contexto_ejecucion.pid);
        }
    }
    pthread_mutex_unlock(&semaforo);
    return pids;
}

void ejecutar_script(char* pathScript) {
    FILE* archivoScript = fopen(pathScript, "r");
    char* lineaLeida = NULL;
    size_t longitud = 0;
    ssize_t cantLeida;
    if ( archivoScript == NULL ) {
        log_error(logs_error, "Error al abrir el archivo %s", pathScript);
        return;
    }
    while ((cantLeida = getline(&lineaLeida, &longitud, archivoScript)) != -1) {
        char** arrayComando = string_split(lineaLeida, " ");
        ejecutar_comando_consola(arrayComando);
    }
    fclose(archivoScript);
    free(lineaLeida);
}

void ejecutar_comando_consola(char** arrayComando) {
    comando = transformarAOperacion(arrayComando[0]);
    switch (comando) {
    case EJECUTAR_SCRIPT:
        char* pathScript = arrayComando[1];
        ejecutar_script(pathScript);
        log_info(logs_auxiliares, "Script de ' %s ' ejecutado", pathScript);
        break;
    case INICIAR_PROCESO:
        pathArchivo = arrayComando[1];
        crear_pcb();
        break;
    case FINALIZAR_PROCESO:
        uint32_t pid = atoi(arrayComando[1]);
        // (deberá liberar recursos, archivos y memoria)
        pthread_mutex_lock(&sem_planificacion);
        planificacionEjecutandose = false;
        // Encontrar en que cola esta
        // Sacarlo de la cola y eliminarlo
        // si esta en exec Mando interrupcion
        log_info(logs_auxiliares, "Proceso %d finalizado", pid);
        planificacionEjecutandose = true;
        pthread_cond_broadcast(&condicion_planificacion);
        pthread_mutex_unlock(&sem_planificacion);
        break;
    case DETENER_PLANIFICACION:
        pthread_mutex_lock(&sem_planificacion);
        planificacionEjecutandose = false;
        pthread_mutex_unlock(&sem_planificacion);
        log_info(logs_auxiliares, "Planificacion pausada");
        break;
    case INICIAR_PLANIFICACION:
        pthread_mutex_lock(&sem_planificacion);
        planificacionEjecutandose = true;
        pthread_cond_broadcast(&condicion_planificacion);
        pthread_mutex_unlock(&sem_planificacion);
        log_info(logs_auxiliares, "Planificacion ejecutandose");
        break;
    case MULTIPROGRAMACION:
        pthread_mutex_lock(&sem_grado_multiprogramacion);
        GRADO_MULTIPROGRAMACION = atoi(arrayComando[1]);
        pthread_mutex_unlock(&sem_grado_multiprogramacion);
        log_info(logs_auxiliares, "Grado de multiprogramacion cambiado a: %d", GRADO_MULTIPROGRAMACION);
        break;
    case PROCESO_ESTADO:
        log_info(logs_obligatorios, "Cola NEW: [%s]", obtenerPids(cola_new, sem_cola_new));
        log_info(logs_obligatorios, "Cola READY: [%s]", obtenerPids(cola_ready, sem_cola_ready));
        log_info(logs_obligatorios, "Cola EXEC: [%s]", obtenerPids(cola_exec, sem_cola_exec));
        log_info(logs_obligatorios, "Cola BLOCKED: [%s]", obtenerPids(cola_blocked, sem_cola_blocked));
        log_info(logs_obligatorios, "Cola EXIT: [%s]", obtenerPids(cola_exit, sem_cola_exit));
        break;
    default:
        log_info(logs_auxiliares, "Comando desconocido");
        break;
    }
}

comando_consola transformarAOperacion(char* operacionLeida) {
    
    if ( string_equals_ignore_case(operacionLeida, "EJECUTAR_SCRIPT") ) { // strcmp devuelve 0 si son iguales
        return EJECUTAR_SCRIPT;
    } else if ( string_equals_ignore_case(operacionLeida, "INICIAR_PROCESO") ) {
        return INICIAR_PROCESO;
    } else if ( string_equals_ignore_case(operacionLeida, "FINALIZAR_PROCESO") ) {
        return FINALIZAR_PROCESO;
    } else if ( string_equals_ignore_case(operacionLeida, "DETENER_PLANIFICACION") ) {
        return DETENER_PLANIFICACION;
    } else if ( string_equals_ignore_case(operacionLeida, "INICIAR_PLANIFICACION") ) {
        return INICIAR_PLANIFICACION;
    } else if ( string_equals_ignore_case(operacionLeida, "MULTIPROGRAMACION") ) {
        return MULTIPROGRAMACION;
    } else if ( string_equals_ignore_case(operacionLeida, "PROCESO_ESTADO") ) {
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
            char* mensaje = recibir_mensaje(socket_cliente);
            log_info(logs_auxiliares, "Me llegó el mensaje %s", mensaje);
            free(mensaje);
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
    enviar_mensaje("Soy Kernel por dispatch!", fd_cpu_dispatch);
    enviar_mensaje("Soy Kernel por interrupt!", fd_cpu_interrupt);
}

void crearLogs() {
    logs_auxiliares = log_create("logsExtras.log", "[EXTRA]", false, LOG_LEVEL_INFO);
    logs_obligatorios = log_create("obligatoriosKernel.log", "[OBLIGATORIOS]", false, LOG_LEVEL_INFO);
    logs_error = log_create("logsExtras.log", "[ERROR]", false, LOG_LEVEL_INFO);
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
    if ( string_equals_ignore_case(algoritmo, "FIFO") ) {
        return FIFO;
    } else if ( string_equals_ignore_case(algoritmo, "RR") ) {
        return RR;
    } else if ( string_equals_ignore_case(algoritmo, "VRR") ) {
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
    queue_destroy_and_destroy_elements(cola_new, (void*)eliminar_pcb);
    queue_destroy_and_destroy_elements(cola_ready, (void*)eliminar_pcb);
    queue_destroy_and_destroy_elements(cola_exec, (void*)eliminar_pcb);
    queue_destroy_and_destroy_elements(cola_blocked, (void*)eliminar_pcb);
    queue_destroy_and_destroy_elements(cola_exit, (void*)eliminar_pcb);
    queue_destroy_and_destroy_elements(cola_ready_aux, (void*)eliminar_pcb);
    pthread_mutex_destroy(&sem_planificacion);
    pthread_mutex_destroy(&sem_cola_new);
    pthread_mutex_destroy(&sem_cola_ready);
    pthread_mutex_destroy(&sem_cola_exec);
    pthread_mutex_destroy(&sem_cola_blocked);
    pthread_mutex_destroy(&sem_cola_exit);
    pthread_mutex_destroy(&sem_grado_multiprogramacion);
    pthread_cond_destroy(&condicion_planificacion);
    sem_destroy(&semContadorColaNew);
    sem_destroy(&semContadorColaReady);
    sem_destroy(&semContadorColaExec);
    sem_destroy(&semContadorColaBlocked);
    sem_destroy(&semContadorColaExit);
}