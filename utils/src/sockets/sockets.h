#ifndef SOCKETS_H_
#define SOCKETS_H_

#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <commons/log.h>
#include <commons/collections/list.h>
#include "../types/types.h"

/* ------------ FUNCIONES --------*/

/*
Inicia el servidor y devuelve el descriptor del socket
puerto = puerto al cual escuchar
loggerAuxiliar = logger para cargar datos extras
loggerError = logger para cargar los errores
*/
int iniciar_servidor(char* puerto, t_log* loggerAuxiliar, t_log* loggerError);
/*
Crea la conexion con el servidor y devuelve el descriptor del socket
ip = ip a la cual conectarse
puerto = puerto al cual conectarse
loggerError = logger para cargar los errores
*/
int crear_conexion(char *ip, char* puerto, t_log* loggerError);
/*
Espera a que el cliente se conecte y devuelve el descriptor del socket
socket_servidor = socket al cual se van a conectar
loggerAuxiliar = logger para cargar datos extras
loggerError = logger para cargar los errores
*/
int esperar_cliente(int socket_servidor, t_log* loggerAuxiliar, t_log* loggerError);
/*
Recibe mensaje del cliente y devuelve el codigo de operacion
socket_cliente = socket por el cual se va a recibir información
*/
int recibir_operacion(int socket_cliente);
/*
Libera la conexion de espacio de memoria
socket = socket a liberar
*/
void liberar_conexion(int socket);
/*
Envia un mensaje string hacia el servidor indicado
mensaje = string a enviar
socket_cliente = socket por el cual enviar el mensaje
*/
void enviar_mensaje(char* mensaje, int socket_servidor);
/*
Serializa un paquete
paquete = paquete a serializar
bytes = bytes del contenido del paquete (tamaño buffer [sizeof(int)], buffer, tamaño codigo de op[sizeof(int)])
*/
void* serializar_paquete(t_paquete* paquete, int bytes);
/*
Recibe size y void* del mensaje
size = lo que pesa en bytes el void*
*/
void *recibir_buffer(int *size, int socket_cliente);
/*
Recibe mensaje del cliente
*/
char* recibir_mensaje(int socket_cliente);
/*
Elimina un paquete
paquete = paquete a eliminar de la memoria
*/
void eliminar_paquete(t_paquete* paquete);
/*
Recibe un paquete y devuelve los valores
socket_cliente = socket por el cual recibe el paquete
*/
t_list *recibir_paquete(int socket_cliente);
/*
Envia un paquete
paquete = paquete a enviar
socket_cliente = socket por el cual envia el paquete
*/
void enviar_paquete(t_paquete* paquete, int socket_cliente);
/*
Agrega un valor al paquete
paquete = paquete al cual agregar el valor
valor = valor a agregar
tamanio = tamaño de lo que se va a agregar
*/
void agregar_a_paquete(t_paquete* paquete, void* valor, int tamanio);
/*
Crea y devuelve un paquete
*/
t_paquete* crear_paquete(op_codigo codigo);
/*
Crea un buffer
paquete = paquete al cual crearle el buffer
*/
void crear_buffer(t_paquete* paquete);
/*
Itera sobre el paquete (Pasar esto a list_iterate)
value = valor sobre el que se va a aplicar la funcion [(void*) iteradorPaquete]
*/
void iteradorPaquete(char* value);


#endif