#include "./sockets.h"

int iniciar_servidor(char* puerto, t_log* loggerAuxiliar, t_log* loggerError) {
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
		log_error(loggerError, "El socket no pudo ser creado");
		abort();
	}

	// Asociamos el socket a un puerto
	if (bind(socket_servidor, servinfo->ai_addr, servinfo->ai_addrlen) == -1) {
		log_error(loggerError, "El socket no pudo ser asociado a un puerto");
		abort();
	}

	// Escuchamos las conexiones entrantes
	if (listen(socket_servidor, SOMAXCONN) == -1) { 
		log_error(loggerError, "El socket no puede escuchar las conexiones entrantes");
		abort();
	}

	freeaddrinfo(servinfo);
    log_info(loggerAuxiliar, "Servidor inicializado en el puerto %s", puerto);

	return socket_servidor;
}

int crear_conexion(char *ip, char* puerto, t_log* loggerError) {
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
		log_error(loggerError, "Error al crear el socket del IP: %s", ip);
		abort();
	}

	// Conectamos el socket
	if (connect(socket_cliente, server_info->ai_addr, server_info->ai_addrlen) == -1) {
		log_error(loggerError, "Error al conectarse al IP: %s", ip);
		abort();
	}

	freeaddrinfo(server_info);

	return socket_cliente;
}

int esperar_cliente(int socket_servidor, t_log* loggerAuxiliar, t_log* loggerError) {
    int socket_cliente;

	// Aceptamos un nuevo cliente
	socket_cliente = accept(socket_servidor, NULL, NULL);
	if (socket_cliente == -1) {
		log_error(loggerError, "Error no se pudo conectar el cliente");
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

void enviar_codigo_op(op_codigo codigo_operacion, int socket_cliente) {
	void* a_enviar = malloc(sizeof(int));

	memcpy(a_enviar, &codigo_operacion, sizeof(int));

	send(socket_cliente, a_enviar, sizeof(int), 0);

	free(a_enviar);
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

void *recibir_buffer(int *size, int socket_cliente) {
	void *buffer;
	// Recibo el tamanio del void*
	recv(socket_cliente, size, sizeof(int), MSG_WAITALL);
	// Creo en memoria el void* segun el tamanio
	buffer = malloc(*size);
	// Recibo void*
	recv(socket_cliente, buffer, *size, MSG_WAITALL);

	return buffer;
}

char *recibir_mensaje(int socket_cliente) {
	int size;
	char *buffer = recibir_buffer(&size, socket_cliente);
	return buffer;
}

t_list *recibir_paquete(int socket_cliente) {
	int size;
	int desplazamiento = 0;
	void *buffer;
	t_list *valores = list_create();
	int tamanio;

	buffer = recibir_buffer(&size, socket_cliente);
	while (desplazamiento < size)
	{
		memcpy(&tamanio, buffer + desplazamiento, sizeof(int));
		desplazamiento += sizeof(int);
		void *valor = malloc(tamanio); // era char* pero ponemos void para generalizar
		memcpy(valor, buffer + desplazamiento, tamanio);
		desplazamiento += tamanio;
		list_add(valores, valor);
	}
	free(buffer);
	return valores;
}

void crear_buffer(t_paquete* paquete) {
	paquete->buffer = malloc(sizeof(t_buffer));
	paquete->buffer->size = 0;
	paquete->buffer->stream = NULL;
}

t_paquete* crear_paquete(op_codigo codigo) {
	t_paquete* paquete = malloc(sizeof(t_paquete));
	paquete->codigo_operacion = codigo;
	crear_buffer(paquete);
	return paquete;
}

void agregar_a_paquete(t_paquete* paquete, void* valor, int tamanio) {
	paquete->buffer->stream = realloc(paquete->buffer->stream, paquete->buffer->size + tamanio + sizeof(int));

	memcpy(paquete->buffer->stream + paquete->buffer->size, &tamanio, sizeof(int));
	memcpy(paquete->buffer->stream + paquete->buffer->size + sizeof(int), valor, tamanio);

	paquete->buffer->size += tamanio + sizeof(int);
}

void enviar_paquete(t_paquete* paquete, int socket_cliente) {
	int bytes = paquete->buffer->size + 2*sizeof(int);
	void* a_enviar = serializar_paquete(paquete, bytes);

	send(socket_cliente, a_enviar, bytes, 0);

	free(a_enviar);
}

void eliminar_paquete(t_paquete* paquete) {
	free(paquete->buffer->stream);
	free(paquete->buffer);
	free(paquete);
}
