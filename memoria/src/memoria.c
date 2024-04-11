#include "./memoria.h"

//TODO: HACER CONEXION DE SOCKETS -11/4 20hs: avanzado, falta ver hilos aún.

int main(void) {

    config = iniciar_config("../memoria.config");
    loggerOblig = log_create("memoria.log", "Modulo_Memoria", 1, LOG_LEVEL_INFO); 
    loggerAux = log_create("memoriaAuxiliar.log", "Modulo_Memoria_AUXILIAR", 1, LOG_LEVEL_INFO);
    log_info(loggerAux, "Se crearon los sockets carajo. fede puto");
    if(config == NULL){
        log_error(loggerAux, "No se encontró el archivo");
        terminar_programa();
        abort();
    }
    leer_config();

    socket_fd_memoria = iniciar_servidor(PUERTO_ESCUCHA, loggerAux);
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
};

int server_escuchar(fd_memoria){
    int fd_cliente = esperar_cliente(socket_fd_memoria, loggerAux);
        log_info(loggerAux, "Che loco, se me conectó un cliente");

        if (fd_cliente != -1)  {
            //MAURO: creo que hay que usar hilos para manejar la triple conexión,
            //tengo que chusmear como funciona la libreria de hilos. Lo siguiente es ""pseudocodigo""
            /*
            1)declarar un dato tipo hilo,
            2/3)reservar memoria y guardar el puntero del fd_cliente que llegue
            4)crear el hilo y mandarlo a resolver el mensaje que reciba del cliente. Ver operaciones futuras.
            5) ¿¿free del (2/3)?? ver bien como funcionan los hilos para evitar memory leaks
            */

            //2)
            int *fd_conexion = malloc(sizeof(int));
            fd_conexion = &fd_cliente;

            return 1;
        }
        else{
            return 0;
        }
}

void gestionar_conexion(/*unaOperacion? Handshake? ver */);


void terminar_programa(){
    log_destroy(loggerAux);
    log_destroy(loggerOblig);
    config_destroy(config);    
};