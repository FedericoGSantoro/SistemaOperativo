#include "./client_handler.h"

int recibirPID(int fd_cliente) {
    
    t_paquete* paquete = malloc(sizeof(t_paquete));
    paquete->buffer = malloc(sizeof(t_buffer));
    int* pid;

    recv(fd_cliente, &(paquete->buffer->size), sizeof(uint32_t), 0);
    paquete->buffer->stream = malloc(paquete->buffer->size);
    recv(fd_cliente, paquete->buffer->stream, paquete->buffer->size, 0);

    void* stream = paquete->buffer->stream;
    // Deserializamos el campo de pid
    memcpy(&pid, stream, sizeof(uint32_t));

    return *pid;
}