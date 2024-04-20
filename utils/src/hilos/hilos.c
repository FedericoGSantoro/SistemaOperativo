#include "./hilos.h"

void crearHiloDetach(pthread_t* thread, void* (*funcion)(), void* fd_, char* nombre, t_log* loggerAuxiliar, t_log* loggerError) {
    if ( !pthread_create(thread, NULL, funcion, fd_) ) {
        log_info(loggerAuxiliar, "Hilo %s creado correctamente", nombre);
        if ( !pthread_detach(*thread) ) {
            log_info(loggerAuxiliar, "Hilo %s desacoplado", nombre);
        }
        else {
            log_error(loggerError, "Hilo %s no pudo ser desacoplado", nombre);
        }
    }
    else {
        log_error(loggerError, "Hilo %s no pudo ser creado", nombre);
    }
}

void crearHiloJoin(pthread_t* thread, void* (*funcion)(), void* fd_, char* nombre, t_log* loggerAuxiliar, t_log* loggerError) {
    if ( !pthread_create(thread, NULL, funcion, fd_) ) {
        log_info(loggerAuxiliar, "Hilo %s creado correctamente", nombre);
        pthread_join(*thread, NULL);
        pthread_exit(thread);
    }
    else {
        log_error(loggerError, "Hilo %s no pudo ser creado", nombre);
    }
}