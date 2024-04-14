#ifndef HILOS_H_
#define HILOS_H_

/*---------LIBRERIAS---------*/

#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>
#include <pthread.h>
#include <commons/log.h>

/*---------FUNCIONES---------*/

/*
Crea un hilo y lo desacopla
thread = variable hilo [&nombre_hilo]
funcion = funcion a aplicar en el hilo [(void*) nombre_funcion]
fd_ = fileDescriptor o socket de la conexion [(void*) fd_conexion]
nombre = nombre de la conexion [Ejemplo: "Memoria"]
loggerAuxiliar = logger para cargar datos extra
loggerError = logger para cargar errores
*/
void crearHilo(pthread_t* thread, void* (*funcion)(), void* fd_, char* nombre, t_log* loggerAuxiliar, t_log* loggerError);

#endif