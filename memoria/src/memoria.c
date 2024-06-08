#include "./includes/memoria.h"

void inicializar_diccionario(t_dictionary* diccionario);
void inicializar_memoria_almacenamiento();
void crear_proceso(int fd_cliente_kernel);
void leer_valor_memoria(int fd_cliente_cpu);
void escribir_valor_memoria(int fd_cliente_cpu);
void devolver_marco(int fd_cliente_cpu);
void eliminar_estructuras_asociadas_al_proceso(int fd_cliente_kernel);
char* fetch_instruccion_de_cliente(int fd_cliente);
void return_instruccion(char* instruccion, int fd_cliente);

int main(void)
{

    // inicializando ando
    inicializar_loggers();
    inicializar_config();
    inicializar_semaforos();
    //al ser dinámico los diccionarios se inicializan en el main y no como constante global, pues la memoria cambia
    inicializar_diccionario(cache_instrucciones);
    inicializar_diccionario(tablas_por_proceso);
    inicializar_memoria_almacenamiento();    
    
    socketFdMemoria = iniciar_servidor(memConfig.puertoEscucha, loggerAux, loggerError);
    while (server_escuchar(socketFdMemoria)); // server escuchar devuelve 0 o 1 (false o true basicamente)
    
    terminar_programa();
    return 0;
}

void leer_config()
{
    memConfig.puertoEscucha = config_get_string_value(config, "PUERTO_ESCUCHA");
    memConfig.tamMemoria = config_get_int_value(config, "TAM_MEMORIA");
    memConfig.tamPagina = config_get_int_value(config, "TAM_PAGINA");
    memConfig.retardoRespuesta = config_get_int_value(config, "RETARDO_RESPUESTA");
    memConfig.pathInstrucciones = config_get_string_value(config, "PATH_INSTRUCCIONES");
}

int server_escuchar(int fd_memoria)
{
    int fd_cliente = esperar_cliente(socketFdMemoria, loggerAux, loggerError);
    // en memoria considero  (POR AHORA) que no hace falta almacenar en var globales los fd de los otros modulos.
    // en caso de necesitar "seguridad", deberia hacer que solo kernel me envie cosas para tocar la memoria
    // y para eso debería de reconocer el fd de kernel en algun lado y darle alguna funcion de gestionar_conexion_adminkernel
    // (ASUMO INTUYO PROPONGO SUPONGO), problema a futuro. -Mauro
    if (fd_cliente != -1)
    {
        pthread_t hilo_cliente;
        crearHiloDetach(&hilo_cliente, (void *)gestionar_conexion, (void *)&fd_cliente, "Cliente conectado", loggerAux, loggerError);

        return 1;
    }

    return 0;
}

void gestionar_conexion(void *puntero_fd_cliente)
{
    int *transformado = (int *)puntero_fd_cliente;
    int fd_cliente = *transformado; // fd_cliente recuperado de crearHilo

    int op_recibida;

    while (fd_cliente != -1)
    {
        op_recibida = recibir_operacion(fd_cliente);

        if (op_recibida == -1)
        {
            log_warning(loggerAux, "El cliente se desconecto de Memoria");
            return;
        }

        switch (op_recibida)
        {
        case MENSAJE:
            char* mensaje = recibir_mensaje(fd_cliente);
            log_info(loggerAux, "Me llegó el mensaje %s", mensaje);
            free(mensaje);
            break;
        case PAQUETE:
            // recibir_paquete
            // deserializar
            // operar
            t_list *valoresPaquete = recibir_paquete(fd_cliente);
            list_iterate(valoresPaquete, (void *)iteradorPaquete);
            break;
        case DEVOLVER_TAM_PAGINA:
            char* tam_pagina_str = int_to_string(memConfig.tamPagina);
            enviar_mensaje(tam_pagina_str, fd_cliente);
            free(tam_pagina_str);
            break;
        case DEVOLVER_MARCO:
            signal(SIGALRM, manejar_retardo); //agrego manejo del retardo de instruc. de cpu
            alarm(memConfig.retardoRespuesta / 1000);
            devolver_marco(fd_cliente);
            break;
        case LEER_VALOR_MEMORIA:
            signal(SIGALRM, manejar_retardo); //agrego manejo del retardo de instruc. de cpu
            alarm(memConfig.retardoRespuesta / 1000);
            leer_valor_memoria(fd_cliente);
            break;
        case ESCRIBIR_VALOR_MEMORIA:
            signal(SIGALRM, manejar_retardo); //agrego manejo del retardo de instruc. de cpu
            alarm(memConfig.retardoRespuesta / 1000);
            escribir_valor_memoria(fd_cliente);
            break;
        case CREAR_PCB: //EL PAQUETE A RECIBIR DE KERNEL DEBE SER 1°PID 2°Path
            crear_proceso(fd_cliente);
            enviar_codigo_op(OK_OPERACION, fd_cliente);
            break;
        case ELIMINAR_PCB: 
            eliminar_estructuras_asociadas_al_proceso(fd_cliente);
            enviar_codigo_op(OK_OPERACION, fd_cliente);
            break;
        // Caso FETCH_INSTRUCCION para cuando la CPU pida la siguiente instruccion a ejecutar
        case FETCH_INSTRUCCION: // la cpu envia el pid y el pc para obtener la instruccion deseada
            signal(SIGALRM, manejar_retardo); //agrego manejo del retardo de instruc. de cpu
            alarm(memConfig.retardoRespuesta / 1000);
            char* instruccion = fetch_instruccion_de_cliente(fd_cliente);
            sem_wait(&sem_retardo);
            return_instruccion(instruccion, fd_cliente);
            break;
        default:
            log_error(loggerError, "Instrucción no reconocida %d.", op_recibida);
            break;
        }
    }
}

void devolver_marco(int fd_cliente_cpu) {

    t_list *paquete_recibido = recibir_paquete(fd_cliente_cpu); 
    
    // recibirNumPagina
    int numero_pagina = *(int*) list_get(paquete_recibido, 0);
    // recibirPID
    int pid = *(int*) list_get(paquete_recibido, 1);
   
    int numero_marco = resolver_solicitud_de_marco(numero_pagina, pid);

    send(fd_cliente_cpu, &numero_marco, sizeof(int),0);
}

void leer_valor_memoria(int fd_cliente_cpu) {

    t_list *valoresPaquete = recibir_paquete(fd_cliente_cpu);            
    int dir_fisica = *(int*) list_get(valoresPaquete, 0);
    int pid = *(int*) list_get(valoresPaquete, 1);

    uint32_t *valor_leido_de_espacio = (uint32_t *)malloc(sizeof(uint32_t));
            
    //semaforo para acceso a espacio compartido
    pthread_mutex_lock(&espacio_usuario.mx_espacio_usuario);
    memcpy(valor_leido_de_espacio, espacio_usuario.espacio_usuario + dir_fisica, sizeof(uint32_t));
    log_info(loggerOblig, "PID: %d - Accion: LEER - Direccion fisica: %d", pid, dir_fisica);
    pthread_mutex_unlock(&espacio_usuario.mx_espacio_usuario);
    //semaforo para acceso a espacio compartido

    t_paquete* paquete_a_enviar = crear_paquete(WRITE);
    agregar_a_paquete(paquete_a_enviar, valor_leido_de_espacio, sizeof(uint32_t));
    enviar_paquete(paquete_a_enviar, fd_cliente_cpu);

    free(valor_leido_de_espacio);
}

void modificar_tabla_paginas(int pid, int num_pag)
{
    char* pid_str = int_to_string(pid);
    t_list* tabla_proceso = dictionary_get(tablas_por_proceso, pid_str);
    t_pagina *pagina_a_modificar = list_get(tabla_proceso, num_pag);

    pagina_a_modificar->modificado = 1;
    free(pid_str);
}

void escribir_valor_memoria(int fd_cliente_cpu) {

    t_list *valoresPaquete = recibir_paquete(fd_cliente_cpu);

    int dir_fisica = *(int*) list_get(valoresPaquete, 0);
    int pid = *(int*) list_get(valoresPaquete, 1);

    uint32_t* registro = (uint32_t*) list_get(valoresPaquete, 2);

    int num_pagina = *(int*) list_get(valoresPaquete, 2);
            
    //semaforo para acceso a espacio compartido --> memoria de usuario
    pthread_mutex_lock(&espacio_usuario.mx_espacio_usuario);
    memcpy(espacio_usuario.espacio_usuario + dir_fisica, registro, sizeof(uint32_t));
    log_info(loggerOblig, "PID: %d - Accion: ESCRIBIR - Direccion fisica: %d", pid, dir_fisica);
    pthread_mutex_unlock(&espacio_usuario.mx_espacio_usuario);
    //semaforo para acceso a espacio compartido --> memoria de usuario

    //MODIFICO LA PAGINA
    modificar_tabla_paginas(pid, num_pagina);

    //LE AVISO A CPU
    t_paquete* paquete_a_enviar = crear_paquete(WRITE);
    enviar_paquete(paquete_a_enviar, fd_cliente_cpu);
}

void crear_proceso(int fd_cliente_kernel) {
    
    t_list *paquete_recibido = recibir_paquete(fd_cliente_kernel); 
    // recibirPID
    int pid = *(int*) list_get(paquete_recibido, 0);
    // recibirPath
    char *path = (char*) list_get(paquete_recibido, 1);
    
    inicializar_tabla_paginas(pid);
    crear_instrucciones(path, pid);
    
    //libero la lista generada del paquete deserializado
    liberar_lista_de_datos_con_punteros(paquete_recibido);
}

void eliminar_estructuras_asociadas_al_proceso(int fd_cliente_kernel) {
    
    t_list *paquete_recibido = recibir_paquete(fd_cliente_kernel); 
    // recibirPID
    uint32_t pid = *(uint32_t*) list_get(paquete_recibido, 0);

    eliminar_instrucciones(pid);
    //libero la lista generada del paquete deserializado
    liberar_lista_de_datos_con_punteros(paquete_recibido);
}

char* fetch_instruccion_de_cliente(int fd_cliente_cpu) {

    t_list* paquete = recibir_paquete(fd_cliente_cpu);

    int pc = *(int*) list_get(paquete, 0);
    int pid = *(int*) list_get(paquete, 1);

    char* instruccion = fetch_instruccion(pid, pc);

    liberar_lista_de_datos_con_punteros(paquete);

    return instruccion;
}

void return_instruccion(char* instruccion, int fd_cliente) {
    enviar_mensaje(instruccion, fd_cliente);
}

void manejar_retardo(){
    sem_post(&sem_retardo);
}

void iteradorPaquete(char *value)
{
    log_info(loggerAux, "%s", value);
}

void inicializar_loggers()
{
    loggerOblig = log_create("memoria.log", "Modulo_Memoria", 1, LOG_LEVEL_INFO);
    loggerAux = log_create("memoriaAuxiliar.log", "Modulo_Memoria_AUXILIAR", 1, LOG_LEVEL_INFO);
    loggerError = log_create("memoriaAuxiliar.log", "Modulo_Memoria_ERROR", 1, LOG_LEVEL_INFO);
}

void inicializar_config()
{
    config = iniciar_config(rutaConfiguracion, loggerError, (void *)terminar_programa);
    leer_config();
}

void inicializar_semaforos(){
    sem_init(&sem_retardo, 0, 0);
}

void inicializar_diccionario(t_dictionary* diccionario){
    diccionario = dictionary_create();
}

void inicializar_memoria_almacenamiento() {

    int cant_marcos = memConfig.tamMemoria / memConfig.tamPagina;
    espacio_usuario.espacio_usuario = malloc(sizeof(memConfig.tamMemoria));// inicializa todos sus bytes a cero.
    pthread_mutex_init(&espacio_usuario.mx_espacio_usuario, NULL);
    vector_marcos = calloc(cant_marcos, sizeof(int));

    for(int i=0; i < cant_marcos; i++) //creo los marcos/paginas 
    {
       vector_marcos[i] =  0;
    }
    log_info(loggerAux, "Memoria dividida en %d marcos creada", cant_marcos);
}

void terminar_programa()
{
    log_destroy(loggerAux);
    log_destroy(loggerOblig);
    log_destroy(loggerError);
    config_destroy(config);
    liberar_conexion(socketFdMemoria);
    dictionary_destroy_and_destroy_elements(cache_instrucciones, destroyer_queue_con_datos_simples);
    dictionary_destroy_and_destroy_elements(tablas_por_proceso, liberar_lista_de_datos_planos);
    free(vector_marcos);
    free(espacio_usuario.espacio_usuario);
    pthread_mutex_destroy(&espacio_usuario.mx_espacio_usuario);
}