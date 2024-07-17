#include "./includes/memoria.h"

void crear_proceso(int fd_cliente_kernel);
void resize_memoria(int fd_cliente_cpu);
/**
 * este metodo procesa las lecturas a generar en memoria. retorna un paquete para enviar con un listado, por direccion fisica, con los 
 * distintos resultados parciales leidos. Al final de la lista, se devuelve un valor mas que es el valor final buscado.
**/
t_paquete* proceso_lectura_valor_memoria(int* cantidad_direcciones_fisicas_leidas, uint32_t* tamanio_a_leer_en_memoria, int fd_cliente_cpu);
/**
 * este metodo procesa las escrituras a generar en memoria. retorna un paquete para enviar con un listado, por direccion fisica, con los 
 * distintos resultados parciales escritos.
**/
t_paquete* proceso_escritura_valor_memoria(int fd_cliente_cpu, int* cantidad_direcciones_fisicas);
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
    cache_instrucciones = dictionary_create();
    tablas_por_proceso = dictionary_create();
    inicializar_memoria_almacenamiento();    
    
    socketFdMemoria = iniciar_servidor(memConfig.puertoEscucha, loggerAux, loggerError);
    while (server_escuchar(socketFdMemoria)); // server escuchar devuelve 0 o 1 (false o true basicamente)
    
    terminar_programa();
    return 0;
}

void leer_config()
{
    memConfig.puertoEscucha = string_duplicate(config_get_string_value(config, "PUERTO_ESCUCHA"));
    memConfig.tamMemoria = config_get_int_value(config, "TAM_MEMORIA");
    memConfig.tamPagina = config_get_int_value(config, "TAM_PAGINA");
    memConfig.retardoRespuesta = config_get_int_value(config, "RETARDO_RESPUESTA");
    memConfig.pathInstrucciones = string_duplicate(config_get_string_value(config, "PATH_INSTRUCCIONES"));
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
        
        log_info(loggerOblig, "Operacion recibida: %d", op_recibida);

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
            usleep(memConfig.retardoRespuesta*1000);
            devolver_marco(fd_cliente);
            break;
        case LEER_VALOR_MEMORIA:
            uint32_t tamanio_a_leer_en_memoria;
            int cantidad_direcciones_fisicas_leidas;
            
            t_paquete* paquete_con_lecturas_a_devolver = proceso_lectura_valor_memoria(&cantidad_direcciones_fisicas_leidas, &tamanio_a_leer_en_memoria, fd_cliente);
            usleep(memConfig.retardoRespuesta*1000*cantidad_direcciones_fisicas_leidas);
            enviar_paquete(paquete_con_lecturas_a_devolver, fd_cliente);
            eliminar_paquete(paquete_con_lecturas_a_devolver);
            break;
        case ESCRIBIR_VALOR_MEMORIA:
            int cantidad_direcciones_fisicas_escritas;
            
            t_paquete* paquete_con_escrituras_a_devolver = proceso_escritura_valor_memoria(fd_cliente, &cantidad_direcciones_fisicas_escritas);
            usleep(memConfig.retardoRespuesta*1000*cantidad_direcciones_fisicas_escritas);
            //LE AVISO A CPU
            enviar_paquete(paquete_con_escrituras_a_devolver, fd_cliente);
            eliminar_paquete(paquete_con_escrituras_a_devolver);
            break;
        case RESIZE_EN_MEMORIA:
            resize_memoria(fd_cliente);
            usleep(memConfig.retardoRespuesta*1000);
            enviar_codigo_op(OK_OPERACION, fd_cliente);
            break;
        case CREAR_PCB: //EL PAQUETE A RECIBIR DE KERNEL DEBE SER 1°PID 2°Path
            crear_proceso(fd_cliente);
            usleep(memConfig.retardoRespuesta*1000);
            enviar_codigo_op(OK_OPERACION, fd_cliente);
            break;
        case ELIMINAR_PCB: 
            eliminar_estructuras_asociadas_al_proceso(fd_cliente);
            usleep(memConfig.retardoRespuesta*1000);
            enviar_codigo_op(OK_OPERACION, fd_cliente);
            break;
        // Caso FETCH_INSTRUCCION para cuando la CPU pida la siguiente instruccion a ejecutar
        case FETCH_INSTRUCCION: // la cpu envia el pid y el pc para obtener la instruccion deseada
            char* instruccion = fetch_instruccion_de_cliente(fd_cliente);
            usleep(memConfig.retardoRespuesta*1000);
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
    uint32_t numero_pagina = *(uint32_t*) list_get(paquete_recibido, 0);
    // recibirPID
    int pid = *(int*) list_get(paquete_recibido, 1);
   
    int numero_marco = resolver_solicitud_de_marco(numero_pagina, pid);

    if (numero_marco == -1) {
        liberar_lista_de_datos_con_punteros(paquete_recibido);
        enviar_codigo_op(NO_MEMORY, fd_cliente_cpu);
        return;
    }

    t_paquete* paquete_a_enviar = crear_paquete(DEVOLVER_MARCO);
    agregar_a_paquete(paquete_a_enviar, &numero_marco, sizeof(uint32_t));
    enviar_paquete(paquete_a_enviar, fd_cliente_cpu);

    eliminar_paquete(paquete_a_enviar);

    liberar_lista_de_datos_con_punteros(paquete_recibido);
}

void obtener_valores_para_operaciones_rw(t_list* paquete_recibido, int* cantidad_direcciones_fisicas, t_list* direcciones_fisicas, int* pid, uint32_t* tamanio_a_operar_en_memoria, int* indice_valores_paquetes) {
    
    *cantidad_direcciones_fisicas = *(int*) list_get(paquete_recibido, *indice_valores_paquetes); //esto es lo mismo que decir, cuantas paginas necesite!

    for (int i = 0; i < *cantidad_direcciones_fisicas; i++) {
        *indice_valores_paquetes += 1;
        uint32_t* dir_fisica = (uint32_t*) list_get(paquete_recibido, *indice_valores_paquetes);
        list_add_in_index(direcciones_fisicas, i, dir_fisica);
    }

    *indice_valores_paquetes += 1;
    *pid = *(int*) list_get(paquete_recibido, *indice_valores_paquetes);
    *indice_valores_paquetes += 1;
    *tamanio_a_operar_en_memoria = *(uint32_t*) list_get(paquete_recibido, *indice_valores_paquetes);
}

void leer_valor_en_espacio(int pid, uint32_t dir_fisica, void* valor_leido_de_espacio, int cantidad_bytes_leidos, uint32_t tamanio_a_leer_en_memoria) {
    
    //semaforo para acceso a espacio compartido
    pthread_mutex_lock(&espacio_usuario.mx_espacio_usuario);

    memcpy(valor_leido_de_espacio, espacio_usuario.espacio_usuario + dir_fisica, tamanio_a_leer_en_memoria);
    log_info(loggerOblig, "PID: %d - Accion: LEER - Direccion fisica: %d", pid, dir_fisica);

    pthread_mutex_unlock(&espacio_usuario.mx_espacio_usuario);
    //semaforo para acceso a espacio compartido
}


t_paquete* proceso_lectura_valor_memoria(int* cantidad_direcciones_fisicas_leidas, uint32_t* tamanio_a_leer_en_memoria_ptr, int fd_cliente_cpu) {

    t_list *paquete_recibido = recibir_paquete(fd_cliente_cpu);            

    int cantidad_direcciones_fisicas;
    t_list* direcciones_fisicas = list_create();
    int pid;
    int indice_valores_paquetes = 0;
    int cantidad_bytes_leidos = 0;
    uint32_t tamanio_a_leer_en_memoria_valor;
    obtener_valores_para_operaciones_rw(paquete_recibido, &cantidad_direcciones_fisicas, direcciones_fisicas, &pid, tamanio_a_leer_en_memoria_ptr, &indice_valores_paquetes);
    
    t_paquete* paquete_a_enviar = crear_paquete(LEER_VALOR_MEMORIA);

    tamanio_a_leer_en_memoria_valor = *tamanio_a_leer_en_memoria_ptr;
    void* valor_leido_de_espacio = malloc(tamanio_a_leer_en_memoria_valor);
    uint32_t registro_reconstruido = 0;
    
    if (cantidad_direcciones_fisicas == 1) { //esto quiere decir que solo va a haber una pagina en la operacion afectada
        
        uint32_t dir_fisica = *(uint32_t*) list_get(direcciones_fisicas, 0);
        leer_valor_en_espacio(pid, dir_fisica, valor_leido_de_espacio, cantidad_bytes_leidos, tamanio_a_leer_en_memoria_valor);
        agregar_a_paquete(paquete_a_enviar, valor_leido_de_espacio, tamanio_a_leer_en_memoria_valor);
    } else {

        for (int i = 0; i < cantidad_direcciones_fisicas; i++) {

            uint32_t bytes_a_leer;
            uint32_t dir_fisica = *(uint32_t*) list_get(direcciones_fisicas, i);
            uint32_t diferencia_entre_dir_y_tam_pagina = memConfig.tamPagina - dir_fisica;

            if (diferencia_entre_dir_y_tam_pagina == 0 || diferencia_entre_dir_y_tam_pagina >= tamanio_a_leer_en_memoria_valor) {
                bytes_a_leer = tamanio_a_leer_en_memoria_valor;
            } else {
                bytes_a_leer = diferencia_entre_dir_y_tam_pagina;
            }
            
            tamanio_a_leer_en_memoria_valor -= bytes_a_leer;

            void* valor_leido_de_espacio_partes = malloc(bytes_a_leer);
            leer_valor_en_espacio(pid, dir_fisica, valor_leido_de_espacio_partes, cantidad_bytes_leidos, bytes_a_leer);
            agregar_a_paquete(paquete_a_enviar, valor_leido_de_espacio_partes, bytes_a_leer);

            if (cantidad_bytes_leidos == 0) {
                memcpy(valor_leido_de_espacio, valor_leido_de_espacio_partes, bytes_a_leer); //Primer guardado
            } else {
                memcpy(valor_leido_de_espacio + cantidad_bytes_leidos, valor_leido_de_espacio_partes, bytes_a_leer);
            }

            cantidad_bytes_leidos += bytes_a_leer;
        }
    }

    //libero la lista generada del paquete deserializado
    liberar_lista_de_datos_con_punteros(paquete_recibido);
    liberar_lista_de_datos_planos(direcciones_fisicas);
    agregar_a_paquete(paquete_a_enviar, valor_leido_de_espacio, (*tamanio_a_leer_en_memoria_ptr));

    return paquete_a_enviar;
}

void resize_memoria(int fd_cliente_cpu) {
    
    t_list *valoresPaquete = recibir_paquete(fd_cliente_cpu);

    int pid = *(int*) list_get(valoresPaquete, 0);
    int size_to_resize = *(int*) list_get(valoresPaquete, 1);

    resize_proceso(pid, size_to_resize, fd_cliente_cpu);

    liberar_lista_de_datos_con_punteros(valoresPaquete);
}

void escribir_valor_en_espacio(int pid, uint32_t dir_fisica, void* registro, int cantidad_bytes_a_escribir) {
    
        //semaforo para acceso a espacio compartido --> memoria de usuario
        pthread_mutex_lock(&espacio_usuario.mx_espacio_usuario);
        memcpy((void*)(((char*)espacio_usuario.espacio_usuario + dir_fisica)), registro, cantidad_bytes_a_escribir);
        log_info(loggerOblig, "PID: %d - Accion: ESCRIBIR - Direccion fisica: %d", pid, dir_fisica);
        pthread_mutex_unlock(&espacio_usuario.mx_espacio_usuario);
        //semaforo para acceso a espacio compartido --> memoria de usuario
}

t_paquete* proceso_escritura_valor_memoria(int fd_cliente_cpu, int* cantidad_direcciones_fisicas) { //TODO: ver de refactorizar esto para que quede mas lindo con algo mas funcional (ejemplo, pasar la funcion de leer y escribir en memoria por parametro, ya que el comportamiento es casi el mismo)

    t_list *valoresPaquete = recibir_paquete(fd_cliente_cpu);

    t_list* direcciones_fisicas = list_create();
    int pid;
    uint32_t cantidad_bytes_a_escribir;
    uint32_t cantidad_bytes_escritos = 0;
    int indice_valores_paquetes = 0;

    t_paquete* paquete_a_enviar = crear_paquete(ESCRIBIR_VALOR_MEMORIA);

    obtener_valores_para_operaciones_rw(valoresPaquete, cantidad_direcciones_fisicas, direcciones_fisicas, &pid, &cantidad_bytes_a_escribir, &indice_valores_paquetes);

    int cantidad_direcciones_fisicas_valor = *cantidad_direcciones_fisicas;

    indice_valores_paquetes += 1;
    void* registro = list_get(valoresPaquete, indice_valores_paquetes);
    
    if (cantidad_direcciones_fisicas_valor == 1) { //esto quiere decir que solo va a haber una pagina en la operacion afectada
        
        uint32_t dir_fisica = *(uint32_t*) list_get(direcciones_fisicas, 0);
        escribir_valor_en_espacio(pid, dir_fisica, registro, cantidad_bytes_a_escribir);
        agregar_a_paquete(paquete_a_enviar, registro, cantidad_bytes_a_escribir);
    } else {

        for (int i = 0; i < cantidad_direcciones_fisicas_valor; i++) {

            uint32_t bytes_a_guardar;
            uint32_t dir_fisica = *(uint32_t*) list_get(direcciones_fisicas, i);
            uint32_t diferencia_entre_dir_y_tam_pagina = memConfig.tamPagina - dir_fisica;

            if (diferencia_entre_dir_y_tam_pagina == 0 || diferencia_entre_dir_y_tam_pagina >= cantidad_bytes_a_escribir) {
                bytes_a_guardar = cantidad_bytes_a_escribir;
            } else {
                bytes_a_guardar = diferencia_entre_dir_y_tam_pagina;
            }
            
            escribir_valor_en_espacio(pid, dir_fisica, registro + cantidad_bytes_escritos, bytes_a_guardar);
            agregar_a_paquete(paquete_a_enviar, registro + cantidad_bytes_escritos, bytes_a_guardar);
            cantidad_bytes_a_escribir -= bytes_a_guardar;
            cantidad_bytes_escritos += bytes_a_guardar;
        }
    }

    //libero la lista generada del paquete deserializado
    liberar_lista_de_datos_con_punteros(valoresPaquete);
    liberar_lista_de_datos_planos(direcciones_fisicas);

    return paquete_a_enviar;
}

void crear_proceso(int fd_cliente_kernel) {
    
    t_list *paquete_recibido = recibir_paquete(fd_cliente_kernel); 
    // recibirPID
    int pid = *(int*) list_get(paquete_recibido, 0);
    // recibirPath
    char *path_relativo = (char*) list_get(paquete_recibido, 1);

    char* path_absoluto = string_new();
    string_append(&path_absoluto, memConfig.pathInstrucciones);
    string_append(&path_absoluto, path_relativo);

    inicializar_tabla_paginas(pid);
    crear_instrucciones(path_absoluto, pid);
    free(path_relativo);
    free(path_absoluto);
    //libero la lista generada del paquete deserializado
    list_destroy(paquete_recibido);
}

void eliminar_estructuras_asociadas_al_proceso(int fd_cliente_kernel) {
    
    t_list *paquete_recibido = recibir_paquete(fd_cliente_kernel); 
    // recibirPID
    uint32_t pid = *(uint32_t*) list_get(paquete_recibido, 0);

    eliminar_instrucciones(pid);
    eliminar_paginas(pid);
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
    config_destroy(config);
}

void inicializar_semaforos(){
    sem_init(&sem_retardo, 0, 0);
}

void terminar_programa()
{
    log_destroy(loggerAux);
    log_destroy(loggerOblig);
    log_destroy(loggerError);
    config_destroy(config);
    liberar_conexion(socketFdMemoria);
    dictionary_destroy_and_destroy_elements(cache_instrucciones, destroyer_queue_con_datos_simples);
    //free(vector_marcos);
    t_list* todas_las_paginas = dictionary_elements(tablas_por_proceso);
    for (int i = 0; i < list_size(todas_las_paginas); i++) {
        t_pagina pagina = *(t_pagina*) list_get(todas_las_paginas, i);
        pthread_mutex_destroy(pagina.mx_pagina);
    }
    dictionary_destroy_and_destroy_elements(tablas_por_proceso, liberar_lista_de_datos_planos);
    free(espacio_usuario.espacio_usuario);
    list_destroy(todas_las_paginas);
    pthread_mutex_destroy(&espacio_usuario.mx_espacio_usuario);
}
