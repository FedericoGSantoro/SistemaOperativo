#include "./sockets.h"

// Iniciamos el servidor 
int iniciar_servidor(char* puerto, t_log* logger, char* msj_server)
{
	int socket_servidor;

	struct addrinfo hints, *servinfo, *p;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	getaddrinfo(NULL, puerto, &hints, &servinfo);

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
    log_trace(logger, "Mensaje Server: %s", msj_server);

	return socket_servidor;
}

// Creamos conexion contra el servidor
int crear_conexion(char *ip, char* puerto)
{
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
		abort();
	}

	// Conectamos el socket
	if (connect(socket_cliente, server_info->ai_addr, server_info->ai_addrlen) == -1) {
		abort();
	}

	freeaddrinfo(server_info);

	return socket_cliente;
}

// Esperamos a que el cliente se conecte
int esperar_cliente(int socket_servidor, t_log* logger)
{
    int socket_cliente;

	// Aceptamos un nuevo cliente
	socket_cliente = accept(socket_servidor, NULL, NULL);
	if (socket_cliente == -1) {
		abort();
	}
	log_info(logger, "Se conecto un cliente!");

	return socket_cliente;
}

// Recibimos mensaje del cliente
int recibir_operacion(int socket_cliente)
{
	int cod_op;

	if (recv(socket_cliente, &cod_op, sizeof(int), MSG_WAITALL) > 0)
		return cod_op;
	else {
		close(socket_cliente);
		return -1;
	}
}