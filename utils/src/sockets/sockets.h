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

int iniciar_servidor(char* puerto, char* ip, t_log* logger, char* msj_server);
int crear_conexion(char *ip, char* puerto);
int esperar_cliente(int socket_servidor);
int recibir_operacion(int socket_cliente);

#endif