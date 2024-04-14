#include "./hilos.h"

void crearHilo(pthread_t* thread, void* (*funcion)(), void* fd_, char* nombre) {
    if ( !pthread_create(thread, NULL, funcion, fd_) ) {
        log_info(logs_auxiliares, "Hilo %s creado correctamente", nombre);
        if ( !pthread_detach(*thread) ) {
            log_info(logs_auxiliares, "Hilo %s desacoplado", nombre);
        }
        else {
            log_error(logs_error, "Hilo %s no pudo ser desacoplado", nombre);
        }
    }
    else {
        log_error(logs_error, "Hilo %s no pudo ser creado", nombre);
    }
}