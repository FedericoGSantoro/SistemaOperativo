#include "./memoria.h"

int main(void) {
    //inicializando ando
    inicializar_loggers();
    inicializar_config();
    socket_fd_memoria = iniciar_servidor(PUERTO_ESCUCHA, loggerAux, loggerError);
    // comentario con valor sentimental // log_info(loggerAux, "Se crearon los sockets carajo. fede puto"); 

    while (server_escuchar(socket_fd_memoria)); //server escuchar devuelve 0 o 1 (false o true basicamente)
    terminar_programa();
    return 0;
}

void leer_config(){
    PUERTO_ESCUCHA = config_get_string_value(config, "PUERTO_ESCUCHA");
    TAM_MEMORIA = config_get_int_value(config, "TAM_MEMORIA");
    TAM_PAGINA = config_get_int_value(config, "TAM_PAGINA");
    RETARDO_RESPUESTA = config_get_int_value(config, "RETARDO_RESPUESTA");
    PATH_INSTRUCCIONES = config_get_string_value(config, "PATH_INSTRUCCIONES");
}

int server_escuchar(int fd_memoria){
    int fd_cliente = esperar_cliente(socket_fd_memoria, loggerAux, loggerError);
        // en memoria considero  (POR AHORA) que no hace falta almacenar en var globales los fd de los otros modulos.
        // en caso de necesitar "seguridad", deberia hacer que solo kernel me envie cosas para tocar la memoria
        // y para eso debería de reconocer el fd de kernel en algun lado y darle alguna funcion de gestionar_conexion_adminkernel
        // (ASUMO INTUYO PROPONGO SUPONGO), problema a futuro. -Mauro
        if (fd_cliente != -1)  {
            pthread_t hilo_cliente;
            crearHiloDetach(&hilo_cliente, (void *) gestionar_conexion, (void *) &fd_cliente, "Cliente conectado", loggerAux, loggerError);

            return 1;
        }
        
        return 0;
}

void gestionar_conexion(void * puntero_fd_cliente){
    int* transformado = (int *) puntero_fd_cliente;
    int fd_cliente = *transformado; //fd_cliente recuperado de crearHilo

    int op_recibida;

    while( fd_cliente != -1 ) {
        op_recibida = recibir_operacion(fd_cliente);

        if ( op_recibida == -1 ) {
            log_warning(loggerAux, "El cliente se desconecto de Memoria");
            return;
        }

        switch (op_recibida){
        case MENSAJE:
            recibir_mensaje(fd_cliente, loggerAux);
            break;

        case PAQUETE:
            //recibir_paquete
            //deserializar
            //operar
            t_list* valoresPaquete = recibir_paquete(fd_cliente);
            list_iterate(valoresPaquete, (void*) iteradorPaquete);
            break;
        // Caso FETCH_INSTRUCCION para cuando la CPU pida la siguiente instruccion a ejecutar
        case FETCH_INSTRUCCION:
            // La instruccion es una char, ejemplo SET AX 1
            
            break;
        default:
            log_error(loggerError, "NO ENTIENDO QUE ME DECIS PA, BANEADO");

            break;
        }
    }
}

void iteradorPaquete(char* value) {
	log_info(loggerAux,"%s", value);
}

void inicializar_loggers(){
    loggerOblig = log_create("memoria.log", "Modulo_Memoria", 1, LOG_LEVEL_INFO); 
    loggerAux = log_create("memoriaAuxiliar.log", "Modulo_Memoria_AUXILIAR", 1, LOG_LEVEL_INFO);
    loggerError = log_create("memoriaAuxiliar.log", "Modulo_Memoria_ERROR", 1, LOG_LEVEL_INFO);
}

void inicializar_config(){
    config = iniciar_config(rutaConfiguracion, loggerError, (void*)terminar_programa);
    leer_config();
}

void terminar_programa(){
    log_destroy(loggerAux);
    log_destroy(loggerOblig);
    log_destroy(loggerError);
    config_destroy(config);
    liberar_conexion(socket_fd_memoria);    
}