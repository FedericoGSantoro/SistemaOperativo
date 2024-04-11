#include "./memoria.h"

//TODO: HACER CONEXION DE SOCKETS

int main(void) {

    config = iniciar_config("../memoria.config");
    logger = log_create("memoria.log", "Modulo_Memoria", 1, LOG_LEVEL_INFO); 
    // log_info( bla bla bla) para probarlo
    if(config == NULL){
        log_error(logger, "No se encontró el archivo");
        terminar_programa();
        exit(-1);
    }
    leer_config();

}
 
void leer_config(){
    IP = config_get_string_value(config, "IP_ESCUCHA");
    PUERTO_ESCUCHA = config_get_string_value(config, "PUERTO_ESCUCHA");
    TAM_MEMORIA = config_get_int_value(config, "TAM_MEMORIA");
    TAM_PAGINA = config_get_int_value(config, "TAM_PAGINA");
    RETARDO_RESPUESTA = config_get_int_value(config, "RETARDO_RESPUESTA");
};


void terminar_programa(){
    log_destroy(logger);
    config_destroy(config);    
};