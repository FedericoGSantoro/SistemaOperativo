#include "./sockets.h"

int iniciar_servidor(char* puerto, t_log* loggerAuxiliar) {
	int socket_servidor;

	struct addrinfo hints, *servinfo, *p;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	getaddrinfo(NULL, puerto, &hints, &servinfo); // NULL para escuchar cualquier IP

	// Creamos el socket de escucha del servidor
	socket_servidor = socket(servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol);
	if (socket_servidor == -1) {
		abort();
	}

	// Asociamos el socket a un puerto
	if (bind(socket_servidor, servinfo->ai_addr, servinfo->ai_addrlen) == -1) {
		abort();
	}

	// Escuchamos las conexiones entrantes
	if (listen(socket_servidor, SOMAXCONN) == -1) { 
		abort();
	}

	freeaddrinfo(servinfo);
    log_info(loggerAuxiliar, "Servidor inicializado en el puerto %s", puerto);

	return socket_servidor;
}

int crear_conexion(char *ip, char* puerto, t_log* loggerAuxiliar) {
    int socket_cliente;

	struct addrinfo hints;
	struct addrinfo *server_info;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	getaddrinfo(ip, puerto, &hints, &server_info);

	// Creamos el socket
	socket_cliente = socket(server_info->ai_family, server_info->ai_socktype, server_info->ai_protocol);
	if (socket_cliente == -1) {
		log_info(loggerAuxiliar, "Error al crear el socket del IP: %s", ip);
		abort();
	}

	// Conectamos el socket
	if (connect(socket_cliente, server_info->ai_addr, server_info->ai_addrlen) == -1) {
		log_info(loggerAuxiliar, "Error al conectarse al IP: %s", ip);
		abort();
	}

	freeaddrinfo(server_info);

	return socket_cliente;
}

int esperar_cliente(int socket_servidor, t_log* loggerAuxiliar) {
    int socket_cliente;

	// Aceptamos un nuevo cliente
	socket_cliente = accept(socket_servidor, NULL, NULL);
	if (socket_cliente == -1) {
		log_info(loggerAuxiliar, "No se pudo conectar el cliente");
	}
	else {
		log_info(loggerAuxiliar, "Se conecto un cliente!");
	}
	return socket_cliente;
}

int recibir_operacion(int socket_cliente) {
	int cod_op;
	// El recibir operacion es bloquante, se queda esperando hasta recibir algo
    // Si se recibe algo menor a 0 se toma que como que el cliente se desconecto
	if (recv(socket_cliente, &cod_op, sizeof(int), MSG_WAITALL) > 0)
		return cod_op;
	else {
		close(socket_cliente);
		return -1;
	}
}

void liberar_conexion(int socket_cliente) {
	close(socket_cliente);
}

void enviar_mensaje(char* mensaje, int socket_cliente) {
	t_paquete* paquete = malloc(sizeof(t_paquete));

	paquete->codigo_operacion = MENSAJE;
	paquete->buffer = malloc(sizeof(t_buffer));
	paquete->buffer->size = strlen(mensaje) + 1;
	paquete->buffer->stream = malloc(paquete->buffer->size);
	memcpy(paquete->buffer->stream, mensaje, paquete->buffer->size);

	int bytes = paquete->buffer->size + 2*sizeof(int);

	void* a_enviar = serializar_paquete(paquete, bytes);

	send(socket_cliente, a_enviar, bytes, 0);

	free(a_enviar);
	eliminar_paquete(paquete);
}

void* serializar_paquete(t_paquete* paquete, int bytes) {
	void * magic = malloc(bytes);
	int desplazamiento = 0;

	memcpy(magic + desplazamiento, &(paquete->codigo_operacion), sizeof(int));
	desplazamiento+= sizeof(int);
	memcpy(magic + desplazamiento, &(paquete->buffer->size), sizeof(int));
	desplazamiento+= sizeof(int);
	memcpy(magic + desplazamiento, paquete->buffer->stream, paquete->buffer->size);
	desplazamiento+= paquete->buffer->size;

	return magic;
}

void *recibir_buffer(int *size, int socket_cliente)
{
	void *buffer;
	// Recibo el tamanio del void*
	recv(socket_cliente, size, sizeof(int), MSG_WAITALL);
	// Creo en memoria el void* segun el tamanio
	buffer = malloc(*size);
	// Recibo void*
	recv(socket_cliente, buffer, *size, MSG_WAITALL);

	return buffer;
}

void recibir_mensaje(int socket_cliente, t_log* loggerAuxiliar)
{
	int size;
	char *buffer = recibir_buffer(&size, socket_cliente);
	log_info(loggerAuxiliar, "Me llego el mensaje: %s", buffer);
	free(buffer);
}

void eliminar_paquete(t_paquete* paquete) {
	free(paquete->buffer->stream);
	free(paquete->buffer);
	free(paquete);
}