#include "./includes/memoria.h"

void inicializar_diccionario();
char* fetch_instruccion_de_cliente(int fd_cliente);
void return_instruccion(char* instruccion, int fd_cliente);

int main(void)
{
    // inicializando ando
    inicializar_loggers();
    inicializar_config();
    //al ser dinámico el diccionario se inicializa en el main y no como constante global, pues la memoria cambia
    inicializar_diccionario();
    socketFdMemoria = iniciar_servidor(memConfig.puertoEscucha, loggerAux, loggerError);

    while (server_escuchar(socketFdMemoria))
        ; // server escuchar devuelve 0 o 1 (false o true basicamente)
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
        case CREAR_PROCESO: //EL PAQUETE A RECIBIR DE KERNEL DEBE SER 1°PID 2°Path
            t_list *paquete_recibido = recibir_paquete(fd_cliente); 
            // recibirPID
            int pid = *(int*) list_get(paquete_recibido, 0);
            // recibirPath
            char *path = (char*) list_get(paquete_recibido, 1);
            crear_instrucciones(path, pid);
            //libero la lista generada del paquete deserializado
            liberar_lista_de_datos_planos(paquete_recibido);
            break;
        // Caso FETCH_INSTRUCCION para cuando la CPU pida la siguiente instruccion a ejecutar
        case FETCH_INSTRUCCION: // la cpu envia el pid y el pc para obtener la instruccion deseada
            char* instruccion = fetch_instruccion_de_cliente(fd_cliente);
            return_instruccion(instruccion, fd_cliente);
            break;
        default:
            log_error(loggerError, "Instrucción no reconocida.");
            break;
        }
    }
}

char* fetch_instruccion_de_cliente(int fd_cliente_cpu) {

    t_list* paquete = recibir_paquete(fd_cliente_cpu);

    int pc = *(int*) list_get(paquete, 0);
    int pid = *(int*) list_get(paquete, 1);

    char* instruccion = fetch_instruccion(pid, pc);

    list_destroy(paquete);

    return instruccion;
}

void return_instruccion(char* instruccion, int fd_cliente) {

    t_paquete* paquete = crear_paquete(DEVOLVER_INSTRUCCION);
    agregar_a_paquete(paquete, &(instruccion), strlen(instruccion) +1); // le agrego +1 por el caracter nulo
    enviar_paquete(paquete, fd_cliente);
    eliminar_paquete(paquete);
    free(instruccion);
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

void inicializar_diccionario(){
    cache_instrucciones = dictionary_create();
}

void terminar_programa()
{
    log_destroy(loggerAux);
    log_destroy(loggerOblig);
    log_destroy(loggerError);
    config_destroy(config);
    liberar_conexion(socketFdMemoria);
    dictionary_destroy_and_destroy_elements(cache_instrucciones, destroyer_queue_con_datos_simples);
}