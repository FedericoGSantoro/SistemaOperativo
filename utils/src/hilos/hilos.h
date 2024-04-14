#ifndef KERNEL_H_
#define KERNEL_H_

/*---------LIBRERIAS---------*/

#include <pthread.h>

/*---------FUNCIONES---------*/

/*
Crea un hilo y lo desacopla
thread = variable hilo [&nombre_hilo]
funcion = funcion a aplicar en el hilo [(void*) nombre_funcion]
fd_ = fileDescriptor o socket de la conexion [(void*) fd_conexion]
nombre = nombre de la conexion [Ejemplo: "Memoria"]
*/
void crearHilo(pthread_t* thread, void* (*funcion)(), void* fd_, char* nombre);

#endif