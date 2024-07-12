#ifndef ENTRADASALIDA_H_
#define ENTRADASALIDA_H_

/*---------LIBRERIAS---------*/

#include "../../utils/src/config/configs.h"
#include "../../utils/src/sockets/sockets.h"
#include "../../utils/src/hilos/hilos.h"
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <commons/string.h>
#include <commons/bitarray.h>
#include <readline/readline.h>
#include <math.h>
#include <dirent.h>
#include <sys/types.h>

/*---------DEFINES---------*/

#define rutaConfiguracion "../entradasalida.config"

/*---------ENUMS---------*/
typedef enum {
    GENERICA,
	STDIN,
    STDOUT,
    FS,
} tipo_de_interfaz;

/*---------DATOS DE LA CONFIGURACION---------*/

tipo_de_interfaz TIPO_INTERFAZ;
int TIEMPO_UNIDAD_TRABAJO;
char*IP_KERNEL;
char* PUERTO_KERNEL;
char* IP_MEMORIA;
char* PUERTO_MEMORIA;
char* PATH_BASE_DIALFS;
int BLOCK_SIZE;
int BLOCK_COUNT;
int RETRASO_COMPACTACION;
FILE *archivo_bloques_dat;
FILE *archivo_bitmap;
void* bloques_datos_addr;
t_bitarray* bitmap_mapeado;
t_list* nombres_archivos_bitmap; //TODO: eliminemos esto dsps, es para pruebitas y logs internos nomas
int indice_global_lista = 0;
void* bitmap_addr;
int fd_bitmap;
int fd_bloque_de_datos;
t_dictionary* map_archivos_metadata;

/*---------ESTRUCTURAS PARA INFORMACION---------*/

typedef struct {
    uint32_t bloque_inicial; //Bloque donde empieza el archivo
    uint32_t tamanio_archivo; //Tamaño del archivo en bytes
} t_metadata_archivo;

// Estructuras para informacion
t_log* logger_obligatorio;
t_log* logger_auxiliar;
t_log* logger_error;
t_config* configuracion;

/*---------FILE DESCRIPTORS CONEXIONES---------*/

int fd_memoria;
int fd_kernel;

/*---------VARIABLES---------*/

// Variable para guardar el nombre que se envia a kernel
char* nombre;
//Variable para guardar el path del config
char* path_config;
// Puntero a funcion para indicar que debe hacer el hilo
void (*tipoOperacion)();

// Parametro para guardar el tipo de instruccion (por ej: IO_GEN_SLEEP)
t_nombre_instruccion tipoInstruccion;
// Lista para guardar los parametros
t_list* parametrosRecibidos;


/*---------HILOS---------*/

pthread_t hilo_memoria;
pthread_t hilo_kernel;

/*---------FUNCIONES---------*/
// Envia un mensaje al socket indicado despues de leer la cadena
void enviarMsj();
// Envia un paquete al socket indicado despues de leer cadenas
void enviarPaquete();
// Determina si enviar la operacion a kernel o memoria
void enviarOperacionA();
// Transforma la operacionLeida en un codigo de operacion para utilizar en un switch por ejemplo
op_codigo transformarAOperacion(char* operacionLeida);

// Inicializa las variables
void inicializar();
char* enumToString(t_nombre_instruccion nombreDeInstruccion);
void recibirIoDetail(t_list* listaPaquete, int ultimo_indice);
void inicializarLogs();
void inicializarConexionKernel();
void inicializarConfig();
void inicializarConexiones();
void inicializarConexionKernel();
void inicializarConexionMemoria();
void leerConfig();
void enviarMsjMemoria();
void enviarMsjKernel();
void levantarArchivoDeBloques();
void levantarArchivoDeBitmap();
void levantarArchivoMetadata();
void io_fs_create(char *nombre_archivo_a_crear);
void io_fs_delete(char* nombre_archivo_a_borrar);
void io_fs_truncate(char *nombre_archivo_a_truncar, uint32_t nuevo_tamanio_archivo, uint32_t pid);
void io_fs_write(int cantidadParametros, t_list* parametrosRecibidos, uint32_t pid);
void io_fs_read(int cantidadParametros, t_list* parametrosRecibidos, uint32_t pid);
// Liberaramos espacio de memoria
void terminarPrograma();

#endif