#include "entradasalida.h"

void mostrar_bloques_libres();

void escribir_valor_en_memoria(t_list* parametrosRecibidos, uint32_t cantidadDirecciones, uint32_t pid, uint32_t tamanio, char* valorLeido, int fd_memoria) {

    int indice_direcciones_memoria = 0;
    uint32_t direccionesMemoria[cantidadDirecciones];
    t_paquete *paqueteMemoria = crear_paquete(ESCRIBIR_VALOR_MEMORIA);
    agregar_a_paquete(paqueteMemoria, &cantidadDirecciones, sizeof(int));
    for (int i = 1; i <= cantidadDirecciones; i++)
    {
        direccionesMemoria[indice_direcciones_memoria] = *(uint32_t *)list_get(parametrosRecibidos, i);
        agregar_a_paquete(paqueteMemoria, &direccionesMemoria[indice_direcciones_memoria], sizeof(uint32_t));
        indice_direcciones_memoria++;
    }
    agregar_a_paquete(paqueteMemoria, &pid, sizeof(int));
    agregar_a_paquete(paqueteMemoria, &tamanio, sizeof(uint32_t));
    agregar_a_paquete(paqueteMemoria, valorLeido, tamanio);
    enviar_paquete(paqueteMemoria, fd_memoria);
    eliminar_paquete(paqueteMemoria);
}

//devuelve el paquete recibido desde memoria
t_list * leer_valor_de_memoria(int fd_memoria, uint32_t cantidadDirecciones, t_list* paquete_con_direcciones, uint32_t pid, uint32_t tamanio) {
    
    uint32_t direccionesMemoria[cantidadDirecciones];
    int indice_direcciones_memoria = 0;

    t_paquete *paqueteMemoria = crear_paquete(LEER_VALOR_MEMORIA);
    // agregar_a_paquete(paqueteMemoria, &(cantidadParametros-1), sizeof(int));
    agregar_a_paquete(paqueteMemoria, &cantidadDirecciones, sizeof(int));

    for (int i = 1; i <= cantidadDirecciones; i++)
    {
        direccionesMemoria[indice_direcciones_memoria] = *(uint32_t *)list_get(paquete_con_direcciones, i);
        agregar_a_paquete(paqueteMemoria, &direccionesMemoria[indice_direcciones_memoria], sizeof(uint32_t));
        indice_direcciones_memoria++;
    }

    agregar_a_paquete(paqueteMemoria, &pid, sizeof(int));
    agregar_a_paquete(paqueteMemoria, &tamanio, sizeof(uint32_t));
    enviar_paquete(paqueteMemoria, fd_memoria);
    eliminar_paquete(paqueteMemoria);
    // Ver como recibo valor de memoria
    op_codigo op = recibir_operacion(fd_memoria);
    while (op != LEER_VALOR_MEMORIA)
    {
        op = recibir_operacion(fd_memoria);
    }

    switch (op)
    {
    case LEER_VALOR_MEMORIA:
        t_list *paqueteRecibido = recibir_paquete(fd_memoria);
        return paqueteRecibido;
        break;
    default:
        log_error(logger_error, "Fallo lectura de Memoria");
        enviar_codigo_op(ERROR_OPERACION, fd_kernel);
        return NULL;
        break;
    }
}

int main(int argc, char* argv[]) {
    //Inicializa todo
    if(argc != 3){
        printf("Error faltan argumentos, tengo %d", argc);
        for(int i = 0; i < argc; i++) {
            printf("Argumentos recibidos: %s",argv[i]);
        }
        return 1;
    }
    nombre = argv[1];
    path_config = argv[2];
    inicializar();
    indice_global_lista = 0;
    op_codigo codigoRecibido;
    int cantidadParametros;
    while( fd_kernel != -1 ){
        codigoRecibido = recibir_operacion(fd_kernel);
        if ( codigoRecibido == -1 ) {
            log_warning(logger_auxiliar, "Desconexion de kernel");
            break;
        }
        t_list* paquete = recibir_paquete(fd_kernel);
        u_int32_t pid = *(uint32_t*)list_get(paquete, 0);
        recibirIoDetail(paquete, 1);
        log_info(logger_obligatorio, "PID: %d - Operacion: %s",pid, enumToString(tipoInstruccion));
        switch (tipoInstruccion) {
        case IO_GEN_SLEEP:
            //chequeo que solo interfaces genericas puedan hacer io_gen_sleep Y lo mismo en los ifs de abajo
            if(TIPO_INTERFAZ != GENERICA){
                log_error(logger_error, "Se envió la instrucción IO_GEN_SLEEP a la interfaz no genérica: %s", nombre);
                break;
            }
            int cantidadUnidadesTrabajo = *(int*) list_get(parametrosRecibidos, 0);
            int tiempoSleep = cantidadUnidadesTrabajo * TIEMPO_UNIDAD_TRABAJO;
            usleep(tiempoSleep*1000);
            enviar_codigo_op(OK_OPERACION, fd_kernel);
            break;

        case IO_STDIN_READ:
            if(TIPO_INTERFAZ != STDIN){
                log_error(logger_error, "Se envió la instrucción IO_STDIN_READ a la interfaz no STDIN: %s", nombre);
                break;
            }
            cantidadParametros = list_size(parametrosRecibidos) - 1;
            if(0 < cantidadParametros ){
                uint32_t cantidadDirecciones = *(uint32_t *) list_get(parametrosRecibidos, 0);
                
                uint32_t tamanio = *(uint32_t*) list_get(parametrosRecibidos, cantidadParametros);
                
                char* valorLeido = readline("Ingrese cadena > ");
                if (valorLeido != NULL) {
                    // Si la longitud de la cadena excede 'cantidad', trunca la cadena
                    if (strlen(valorLeido) > tamanio) {
                        (valorLeido)[tamanio] = '\0';
                    }
                }                
                tamanio++;

                escribir_valor_en_memoria(parametrosRecibidos, cantidadDirecciones, pid, tamanio, valorLeido, fd_memoria);

                //Ver como recibo valor de memoria
                op_codigo op = recibir_operacion(fd_memoria);
                /*switch (op){
                case ESCRIBIR_VALOR_MEMORIA:
                    log_info(logger_auxiliar, "Se escribio el valor '%s' en memoria", valorLeido);
                    enviar_codigo_op(OK_OPERACION, fd_kernel);
                    break;
                default:
                    log_error(logger_error, "Fallo escritura de Memoria con el valor: %s", valorLeido);
                    //enviar_codigo_op(ERROR_OPERACION, fd_kernel);
                    break;
                }
                */
                enviar_codigo_op(OK_OPERACION, fd_kernel);
                free(valorLeido);
            }else{
                log_error(logger_error, "No se recibio lo necesario para escribir en memoria");
                enviar_codigo_op(ERROR_OPERACION, fd_kernel);
            }
            break;

        case IO_STDOUT_WRITE:
            if(TIPO_INTERFAZ != STDOUT){
                log_error(logger_error, "Se envió la instrucción IO_STDOUT_WRITE a la interfaz no STDOUT: %s", nombre);
                break;
            }
            cantidadParametros = list_size(parametrosRecibidos) - 1;
            if(0 < cantidadParametros ){
                uint32_t cantidad_direcciones = *(uint32_t *)list_get(parametrosRecibidos, 0);
                
                uint32_t tamanio = *(uint32_t *)list_get(parametrosRecibidos, cantidadParametros);
                tamanio++;

                t_list* lecturas_memoria = leer_valor_de_memoria(fd_memoria, cantidad_direcciones, parametrosRecibidos, pid, tamanio);
                char *valorAMostrar = list_get(lecturas_memoria, list_size(lecturas_memoria) - 1); // En el ultimo valor de la lista de valores leidos, se encuentra el valor completo (o final)
                log_info(logger_auxiliar, "%s", valorAMostrar);
                enviar_codigo_op(OK_OPERACION, fd_kernel);
                liberar_lista_de_datos_con_punteros(lecturas_memoria);
            }else{
                log_error(logger_error, "No se recibio lo necesario para leer de memoria");
                enviar_codigo_op(ERROR_OPERACION, fd_kernel);
            }
            break;
        
        case IO_FS_CREATE:
            log_info(logger_auxiliar, "Inicio función IO_FS_CREATE");
            usleep(TIEMPO_UNIDAD_TRABAJO*1000);
            
            if(TIPO_INTERFAZ != FS){
                log_error(logger_error, "Se envio la instrucción IO_FS_CREATE a la interfaz no FS: %s", nombre);
                break;
            }
            cantidadParametros = list_size(parametrosRecibidos);
            if (0 < cantidadParametros){
                char* nombre_archivo_a_crear = (char*) list_get(parametrosRecibidos, 0);
                log_info(logger_obligatorio, "PID: %d - Crear Archivo: %s", pid, nombre_archivo_a_crear);
                io_fs_create(nombre_archivo_a_crear);
                mostrar_bloques_libres();
            }
            enviar_codigo_op(OK_OPERACION, fd_kernel);
            break;

        case IO_FS_DELETE:
            log_info(logger_auxiliar, "Inicio función IO_FS_DELETE");
            usleep(TIEMPO_UNIDAD_TRABAJO*1000);

            if(TIPO_INTERFAZ != FS){
                log_error(logger_error, "Se envió la instrucción IO_FS_DELETE a la interfaz no FS: %s", nombre);
                break;
            }
            cantidadParametros = list_size(parametrosRecibidos);
            if (0 < cantidadParametros){
                char* nombre_archivo_a_borrar = (char*) list_get(parametrosRecibidos, 0);
                log_info(logger_obligatorio, "PID: %d - Eliminar Archivo: %s", pid, nombre_archivo_a_borrar); //lo hago local para aclarar el nombre.
                io_fs_delete(nombre_archivo_a_borrar);
                mostrar_bloques_libres();
            }
            enviar_codigo_op(OK_OPERACION, fd_kernel);
            break;

        case IO_FS_READ:
            log_info(logger_auxiliar, "Inicio función IO_FS_READ");
            usleep(TIEMPO_UNIDAD_TRABAJO*1000);

            if(TIPO_INTERFAZ != FS){
                log_error(logger_error, "Se envió la instrucción IO_FS_READ a la interfaz no FS: %s", nombre);
                break;
            }
            cantidadParametros = list_size(parametrosRecibidos);
            if (0 < cantidadParametros){
                io_fs_read(cantidadParametros, parametrosRecibidos, pid);
                mostrar_bloques_libres();
            }
            enviar_codigo_op(OK_OPERACION, fd_kernel);
            break;

        case IO_FS_WRITE:
            log_info(logger_auxiliar, "Inicio función IO_FS_WRITE");
            usleep(TIEMPO_UNIDAD_TRABAJO*1000);

            if(TIPO_INTERFAZ != FS){
                log_error(logger_error, "Se envió la instrucción IO_FS_WRITE a la interfaz no FS: %s", nombre);
                break;
            }
            cantidadParametros = list_size(parametrosRecibidos);
            if (0 < cantidadParametros){
                io_fs_write(cantidadParametros, parametrosRecibidos, pid);
                mostrar_bloques_libres();
            }
            enviar_codigo_op(OK_OPERACION, fd_kernel);
            break;

        case IO_FS_TRUNCATE:
            log_info(logger_auxiliar, "Inicio función IO_FS_TRUNCATE");
            usleep(TIEMPO_UNIDAD_TRABAJO*1000);

            if(TIPO_INTERFAZ != FS){
                log_error(logger_error, "Se envio la instrucción IO_FS_TRUNCATE a la interfaz no FS: %s", nombre);
                break;
            }
            cantidadParametros = list_size(parametrosRecibidos);
            if (0 < cantidadParametros){
                char* nombre_archivo_a_truncar = (char*) list_get(parametrosRecibidos, 0);
                uint32_t nuevo_tamanio_archivo = *(uint32_t*) list_get(parametrosRecibidos, 1);
                log_info(logger_obligatorio, "PID: %d - Truncar Archivo: %s - Tamaño: %d", pid, nombre_archivo_a_truncar, nuevo_tamanio_archivo);
                io_fs_truncate(nombre_archivo_a_truncar, nuevo_tamanio_archivo, pid);
                mostrar_bloques_libres();
            }
            enviar_codigo_op(OK_OPERACION, fd_kernel);
            break;
        
        default:
            log_error(logger_error, "Se recibio una instruccion no esperada: %d", tipoInstruccion);
            break;
        }
        list_clean(parametrosRecibidos);
        list_destroy(paquete);
    }
    terminarPrograma();
    return 0;
}

void recibirIoDetail(t_list* listaPaquete, int ultimo_indice) {

    uint32_t cantidad_parametros_io_detail = *(uint32_t*)list_get(listaPaquete, ultimo_indice);

    if (cantidad_parametros_io_detail != 0) {

        for (int i = 0; i < cantidad_parametros_io_detail; i++) {

            ultimo_indice++;
            tipo_de_dato tipo_de_dato_parametro_io = *(tipo_de_dato*) list_get(listaPaquete, ultimo_indice);

            //t_params_io* parametro_io_a_guardar;
            
            ultimo_indice++;
            void* valor_parametro_io_recibido = (void*) list_get(listaPaquete, ultimo_indice);
            void* valor_parametro_a_guardar;

            switch (tipo_de_dato_parametro_io) {
            case INT:
                //parametro_io_a_guardar = malloc(sizeof(int)*2);
                valor_parametro_a_guardar = malloc(sizeof(int));
                valor_parametro_a_guardar = (int*)valor_parametro_io_recibido;
                break;
            case UINT32:
                valor_parametro_a_guardar = malloc(sizeof(uint32_t));
                valor_parametro_a_guardar = (uint32_t *)valor_parametro_io_recibido;
                //log_info(logger_auxiliar, "Se envia el parametro %d", *(uint32_t*)valor_parametro_a_guardar);
                break;
            case STRING:
                valor_parametro_a_guardar = (char *)valor_parametro_io_recibido;
                break;
            default:
                log_error(logger_error, "Error tipo de dato recibido");
                break;
            }

            //parametro_io_a_guardar->tipo_de_dato = tipo_de_dato_parametro_io; //almaceno el tipo de dato del parametro de la instruccion de io 
            //(esto va a servir mas adelante para que kernel pueda usarlo correctamente, ya que puede recibir char* o int)
            //parametro_io_a_guardar->valor = valor_parametro_a_guardar; //almaceno el valor del parametro de la instruccion de io

            list_add_in_index(parametrosRecibidos, i, valor_parametro_a_guardar); //almaceno el parametro en la lista de parametros que usara kernel luego
        }
        ultimo_indice++;
        //pcb->contexto_ejecucion.io_detail.nombre_io = (char *)list_get(contexto, ultimo_indice); //obtengo el nombre de la IO
        
        //ultimo_indice++;
        tipoInstruccion = *(t_nombre_instruccion *)list_get(listaPaquete, ultimo_indice); //obtengo el nombre de la instruccion contra IO
    }
}

void inicializar(){
    inicializarLogs();

    inicializarConfig();

    inicializarConexiones();

    if (TIPO_INTERFAZ == FS) {
        levantarArchivoDeBloques();
        levantarArchivoDeBitmap();
        levantarArchivoMetadata();
    }

    parametrosRecibidos = list_create();
}

void inicializarLogs(){
    logger_obligatorio = log_create("entradasalida.log", "LOG_OBLIGATORIO_ENTRADA-SALIDA", true, LOG_LEVEL_INFO);
    logger_auxiliar = log_create("entradasalidaExtras.log", "LOG_EXTRA_ENTRADA_SALIDA", true, LOG_LEVEL_INFO);
    logger_error = log_create("entradasalidaExtras.log", "LOG_ERROR_ENTRADA_SALIDA", false, LOG_LEVEL_ERROR);
    // Compruebo que los logs se hayan creado correctamente
    if (logger_auxiliar == NULL || logger_obligatorio == NULL || logger_error == NULL) {
        terminarPrograma();
        abort();
    }
}

void inicializarConfig(){
    configuracion = iniciar_config(path_config, logger_error, (void*)terminarPrograma);
    leerConfig();
}

tipo_de_interfaz leerTipoDeInterfaz() {
    char* tipo = config_get_string_value(configuracion, "TIPO_INTERFAZ");
    if ( string_equals_ignore_case(tipo, "GENERICA") ) {
        return GENERICA;
    } else if ( string_equals_ignore_case(tipo, "STDIN") ) {
        return STDIN;
    } else if ( string_equals_ignore_case(tipo, "STDOUT") ) {
        return STDOUT;
    } else if ( string_equals_ignore_case(tipo, "DIALFS") ) {
        return FS;
    } else {
        log_error(logger_error, "Tipo de interfaz no reconocida: %s", tipo);
        abort();
    }
}

void leerConfig() {
    IP_KERNEL = config_get_string_value(configuracion, "IP_KERNEL");
    PUERTO_KERNEL = config_get_string_value(configuracion, "PUERTO_KERNEL");
    TIPO_INTERFAZ = leerTipoDeInterfaz();
    //log_info(logger_auxiliar, "Tipo interfaz: %d", TIPO_INTERFAZ);
    switch (TIPO_INTERFAZ) {
    case GENERICA:
        TIEMPO_UNIDAD_TRABAJO = config_get_int_value(configuracion, "TIEMPO_UNIDAD_TRABAJO");            
        break;
    case STDIN:
        IP_MEMORIA = config_get_string_value(configuracion, "IP_MEMORIA");
        PUERTO_MEMORIA = config_get_string_value(configuracion, "PUERTO_MEMORIA");
        break;
    case STDOUT:
        TIEMPO_UNIDAD_TRABAJO = config_get_int_value(configuracion, "TIEMPO_UNIDAD_TRABAJO");
        IP_MEMORIA = config_get_string_value(configuracion, "IP_MEMORIA");
        PUERTO_MEMORIA = config_get_string_value(configuracion, "PUERTO_MEMORIA");
        break;
    case FS:
        TIEMPO_UNIDAD_TRABAJO = config_get_int_value(configuracion, "TIEMPO_UNIDAD_TRABAJO");
        IP_MEMORIA = config_get_string_value(configuracion, "IP_MEMORIA");
        PUERTO_MEMORIA = config_get_string_value(configuracion, "PUERTO_MEMORIA");
        PATH_BASE_DIALFS = config_get_string_value(configuracion, "PATH_BASE_DIALFS");
        BLOCK_SIZE = config_get_int_value(configuracion, "BLOCK_SIZE");
        BLOCK_COUNT = config_get_int_value(configuracion, "BLOCK_COUNT");
        RETRASO_COMPACTACION = config_get_int_value(configuracion, "RETRASO_COMPACTACION");
        break;        
    }
}

void inicializarConexiones(){
    inicializarConexionKernel();
    if ( TIPO_INTERFAZ != GENERICA ) {
        inicializarConexionMemoria();
    }
}

void inicializarConexionKernel()
{
    fd_kernel = crear_conexion(IP_KERNEL, PUERTO_KERNEL, logger_error);
    t_paquete* paquete = crear_paquete(OK_OPERACION);

	agregar_a_paquete(paquete, nombre, strlen(nombre)+1); // Agregamos al paquete el stream
	agregar_a_paquete(paquete, &TIPO_INTERFAZ, sizeof(int)); // Agregamos al paquete el stream
    enviar_paquete(paquete, fd_kernel);
	eliminar_paquete(paquete);
    //crearHiloDetach(&hilo_kernel, (void*)enviarPaquete, NULL, "Kernel", logger_auxiliar, logger_error);
}

void inicializarConexionMemoria()
{
    fd_memoria = crear_conexion(IP_MEMORIA, PUERTO_MEMORIA, logger_error);
    enviarMsjMemoria();
    //crearHiloDetach(&hilo_memoria, (void*)enviarMsjMemoria, NULL, "Memoria", logger_auxiliar, logger_error);

}

// Función para sincronizar los cambios en el archivo mapeado
void sync_file(void *addr, size_t length) {
    if (msync(addr, length, MS_SYNC) == -1) {
        perror("msync");
        exit(EXIT_FAILURE);
    }
    bloques_datos_addr = mmap(NULL, BLOCK_COUNT * BLOCK_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd_bloque_de_datos, 0);
}

void levantarArchivoDeBloques() {

    char* path_bloques_de_datos = string_new();
    string_append(&path_bloques_de_datos, PATH_BASE_DIALFS);
    string_append(&path_bloques_de_datos, "/bloques.dat");
    fd_bloque_de_datos;

    if (access(path_bloques_de_datos, F_OK) == -1) { //validar si no existe el archivo
        fd_bloque_de_datos = open(path_bloques_de_datos, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
        if (fd_bloque_de_datos == -1) {
            perror("Error al abrir bloques.dat");
            exit(EXIT_FAILURE);
        }
        if (ftruncate(fd_bloque_de_datos, BLOCK_COUNT * BLOCK_SIZE) == -1) {
            perror("Error al truncar bloques.dat");
            exit(EXIT_FAILURE);
        }
    } else {
        fd_bloque_de_datos = open(path_bloques_de_datos, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
    }

    bloques_datos_addr = mmap(NULL, BLOCK_COUNT * BLOCK_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd_bloque_de_datos, 0);
    
    if (bloques_datos_addr == MAP_FAILED) {
        perror("Error al mapear bloques.dat");
        exit(EXIT_FAILURE);
    }
}

void levantarArchivoDeBitmap() {

    char* path_bitmap = string_new();
    string_append(&path_bitmap, PATH_BASE_DIALFS);
    string_append(&path_bitmap, "/bitmap.dat");
    fd_bitmap;
    if (access(path_bitmap, F_OK) == -1) { //validar si no existe el archivo
        fd_bitmap = open(path_bitmap, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
        if (fd_bitmap == -1) {
            perror("Error al abrir bitmap.dat");
            exit(EXIT_FAILURE);
        }
        if (ftruncate(fd_bitmap, BLOCK_COUNT) == -1) {
            perror("Error al truncar bitmap.dat");
            exit(EXIT_FAILURE);
        }
    } else {
        fd_bitmap = open(path_bitmap, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
    }

    bitmap_addr = mmap(NULL, BLOCK_COUNT, PROT_READ | PROT_WRITE, MAP_SHARED, fd_bitmap, 0);
    
    if (bitmap_addr == MAP_FAILED) {
        perror("Error al mapear bitmap.dat");
        exit(EXIT_FAILURE);
    }

    // Crear el bitmap utilizando la estructura t_bitarray
    bitmap_mapeado = bitarray_create_with_mode((char *)bitmap_addr, BLOCK_COUNT, LSB_FIRST);
}

t_metadata_archivo* leer_metadata_archivo(char* nombre_archivo_a_leer) {
    
    if (dictionary_has_key(map_archivos_metadata, nombre_archivo_a_leer)) {
        return (t_metadata_archivo*) dictionary_get(map_archivos_metadata, nombre_archivo_a_leer);
    }

    char* linea = string_new();
    char* clave = string_new();
    char* valor = string_new();
    char* path_metadata = string_new();
    string_append(&path_metadata, PATH_BASE_DIALFS);
    string_append(&path_metadata, "/");
    string_append(&path_metadata, nombre_archivo_a_leer); //TODO: funcion para refactor esto y no duplicar comportamiento. Ahora no, muy tarde xd
    
    t_metadata_archivo* metadata_a_leer = malloc(sizeof(t_metadata_archivo));
    
    t_config* metadata_leida = config_create(path_metadata);

    metadata_a_leer->bloque_inicial = (uint32_t) config_get_int_value(metadata_leida, "BLOQUE_INICIAL");
    metadata_a_leer->tamanio_archivo = (uint32_t) config_get_int_value(metadata_leida, "TAMANIO_ARCHIVO");

    return metadata_a_leer;
}

void levantarArchivoMetadata() {

    map_archivos_metadata = dictionary_create();

    DIR *directorio;
    struct dirent *entrada;

    // Abrir el directorio
    directorio = opendir(PATH_BASE_DIALFS);
    
    if (directorio == NULL) {
        perror("Error al abrir el directorio");
        return;
    }
    
    // Leer y mostrar los nombres de archivo del directorio
    while ((entrada = readdir(directorio)) != NULL) {
        // Ignorar los directorios "." y ".."
        char* nombre_archivo = (char*) (entrada->d_name);
        if (entrada->d_type == DT_REG && !string_equals_ignore_case(nombre_archivo, "bloques.dat") && !string_equals_ignore_case(nombre_archivo, "bitmap.dat")) { // Verificar si es un archivo regular, si no es el de bloques y si no es el de bitmap
            t_metadata_archivo* metadata_archivo = leer_metadata_archivo((char*)(nombre_archivo));
            dictionary_put(map_archivos_metadata, nombre_archivo, metadata_archivo);
        }
    }
    
    // Cerrar el directorio
    closedir(directorio);
}

void escribir_metadata(t_metadata_archivo* metadata, char* nombre_archivo) {

    char* path_metadata = string_new();
    string_append(&path_metadata, PATH_BASE_DIALFS);
    string_append(&path_metadata, "/");
    string_append(&path_metadata, nombre_archivo);

    int fd_metadata;

    FILE *archivo;
    // Abrir el archivo en modo escritura binaria ("wb")
    archivo = fopen(path_metadata, "wb");
    // Escribir la estructura en el archivo

    // Escribir en el archivo con el formato especificado
    fprintf(archivo, "BLOQUE_INICIAL=%d\n", metadata->bloque_inicial);
    fprintf(archivo, "TAMANIO_ARCHIVO=%d\n", metadata->tamanio_archivo);

    dictionary_put(map_archivos_metadata, nombre_archivo, metadata);

    fclose(archivo);
}

uint32_t buscar_primer_bloque_libre() {
    
    uint32_t bloque_libre = 0;
    while (bitarray_test_bit(bitmap_mapeado, bloque_libre) && bloque_libre < BLOCK_COUNT) {
        bloque_libre++;
    }

    return bloque_libre;
}

void io_fs_create(char *nombre_archivo_a_crear) {

    t_metadata_archivo* metadata = malloc(sizeof(t_metadata_archivo));

    // Encontrar un bloque libre en el bitmap
    uint32_t bloque_libre = buscar_primer_bloque_libre();

    // Si no hay bloques libres
    if (bloque_libre >= BLOCK_COUNT) {
        fprintf(stderr, "Error: No hay bloques libres disponibles.\n");
        exit(EXIT_FAILURE);
    }

    // Marcar el bloque como ocupado en el bitmap
    bitarray_set_bit(bitmap_mapeado, bloque_libre);
    indice_global_lista++;
    sync_file(bitmap_addr, BLOCK_COUNT);

    // Escribir metadata del archivo en el archivo de metadata
    metadata->bloque_inicial = bloque_libre;
    metadata->tamanio_archivo = 0; // Empieza con tamaño 0 bytes
    //escribir metadata
    escribir_metadata(metadata, nombre_archivo_a_crear);
}

void liberar_bloques(uint32_t actual_nro_bloque_final_archivo, uint32_t cantidad_a_liberar_en_bloques, char* nombre_archivo) {
    for (int i = 1; i <= cantidad_a_liberar_en_bloques; i++) {
        bitarray_clean_bit(bitmap_mapeado, actual_nro_bloque_final_archivo);
        actual_nro_bloque_final_archivo--;
    }
}

void achicar_archivo(char* nombre_archivo_a_truncar, uint32_t nuevo_tamanio_archivo, t_metadata_archivo* metadata_archivo_a_truncar, uint32_t actual_nro_bloque_final_archivo) {

    uint32_t tamanio_a_truncar_en_bytes = metadata_archivo_a_truncar->tamanio_archivo - nuevo_tamanio_archivo;
    float resultado_tamanio_a_truncar_en_bytes = ((float)tamanio_a_truncar_en_bytes / (float)BLOCK_SIZE);
    uint32_t tamanio_a_truncar_en_bloques = ceil(resultado_tamanio_a_truncar_en_bytes);

    // Liberar los bloques que exceden al archivo
    liberar_bloques(actual_nro_bloque_final_archivo, tamanio_a_truncar_en_bloques, nombre_archivo_a_truncar);
    metadata_archivo_a_truncar->tamanio_archivo = nuevo_tamanio_archivo;

    //escribir metadata
    escribir_metadata(metadata_archivo_a_truncar, nombre_archivo_a_truncar);
    sync_file(bitmap_addr, BLOCK_COUNT);
}

uint32_t obtener_cantidad_de_bloques_libres_al_final_de_archivo(uint32_t actual_nro_bloque_final_archivo) {

    uint32_t cantidad_bloques_libres = 0;

    actual_nro_bloque_final_archivo++;
    while (actual_nro_bloque_final_archivo < BLOCK_COUNT && !bitarray_test_bit(bitmap_mapeado, actual_nro_bloque_final_archivo))
    {
        cantidad_bloques_libres++;
        actual_nro_bloque_final_archivo++;
    }

    return cantidad_bloques_libres;
}

void ocupar_bloques(uint32_t nro_bloque_inicio, uint32_t cantidad_bloques, char* nombre_archivos_bitmap) {
    for (int i = nro_bloque_inicio; i < nro_bloque_inicio + cantidad_bloques; i++) {
        bitarray_set_bit(bitmap_mapeado, i);
    }
}

int buscar_espacio_libre_contiguo_en_disco(uint32_t tamanio_a_truncar_en_bloques) {
    
    uint32_t nro_bloque_a_evaluar = 0;
    uint32_t cantidad_bloques_libres = 0;

    for (int i = 0; i < BLOCK_COUNT && cantidad_bloques_libres < tamanio_a_truncar_en_bloques; i++){
        if (bitarray_test_bit(bitmap_mapeado, i)) {
            nro_bloque_a_evaluar = i;
            cantidad_bloques_libres = 0;
        } else {
            cantidad_bloques_libres++;
        }
    }

    return cantidad_bloques_libres == tamanio_a_truncar_en_bloques ? nro_bloque_a_evaluar + 1 : -1;
}

uint32_t bloques_libres() {
    
    uint32_t cantidad_bloques_libres = 0;
    
    for (int i = 0; i < BLOCK_COUNT; i++){
        if (!bitarray_test_bit(bitmap_mapeado, i)) {
            cantidad_bloques_libres++;
        }
    }

    return cantidad_bloques_libres;
}

void mostrar_bloques_libres() {

    uint32_t cantidad_de_espacios_libres = bloques_libres();
    char* bloques_libres_con_formato = string_new();

    for (int i = 0; i < BLOCK_COUNT; i++) {
        string_append_with_format(&bloques_libres_con_formato, "%d-", bitarray_test_bit(bitmap_mapeado, i));
    }

    log_info(logger_auxiliar, "cantidad bloques libres: %d con formato: %s", cantidad_de_espacios_libres, bloques_libres_con_formato);
}

void clear_bitmap(char* nombre_archivos_bitmap) {
    for (int i = 0; i < BLOCK_COUNT; i++) {
        bitarray_clean_bit(bitmap_mapeado, i);
    }
}

void fill_bitmap(uint32_t cantidad_a_fillear) {
    for (int i = 0; i < cantidad_a_fillear; i++) {
        bitarray_set_bit(bitmap_mapeado, i);
    }
}


//compacta todo el FS y devuelve la nueva metadata actualizada
t_metadata_archivo* compactar(char* nombre_archivo_a_truncar, uint32_t tamanio_actual_en_bloques_de_archivo_a_truncar, uint32_t tamanio_diferencia_a_truncar_en_bloques, uint32_t actual_nro_bloque_final_archivo, uint32_t tamanio_a_truncar_en_bloques, uint32_t nuevo_tamanio_archivo_bytes) {
    
    void* bloques_de_archivos_nuevo = malloc(BLOCK_COUNT * BLOCK_SIZE);

    t_list* nombre_archivos_metadata = dictionary_keys(map_archivos_metadata);
    int pointer_memory_bloque_de_datos = -1;
    int aux_pointer_memory_bloque_de_datos = 0;

    for (int i = 0; i < list_size(nombre_archivos_metadata); i++) {
        char* nombre_metadata_archivo = list_get(nombre_archivos_metadata, i);
        if (string_equals_ignore_case(nombre_metadata_archivo, nombre_archivo_a_truncar)) {
            continue;
        }

        t_metadata_archivo metadata_archivo = *(t_metadata_archivo*) dictionary_get(map_archivos_metadata, nombre_metadata_archivo);
        
        float resultado_tamanio_actual =  ((float) metadata_archivo.tamanio_archivo / (float) BLOCK_SIZE);
        uint32_t tamanio_actual_en_bloques = ceil(resultado_tamanio_actual);
        
        uint32_t limite = metadata_archivo.bloque_inicial + tamanio_actual_en_bloques;

        uint32_t nuevo_bloque_inicial = pointer_memory_bloque_de_datos;
        for (int j = metadata_archivo.bloque_inicial; j < limite; j++) {
            pointer_memory_bloque_de_datos++;
            memcpy(bloques_de_archivos_nuevo + (pointer_memory_bloque_de_datos * BLOCK_SIZE), bloques_datos_addr + (j * BLOCK_SIZE), BLOCK_SIZE);
        }

        metadata_archivo.bloque_inicial = nuevo_bloque_inicial;
    }

    //reseteamos todo el bitmap a 0 para luego volver a setearlo correspondiente a los bloques de datos que hayamos ocupado
    clear_bitmap(nombre_archivo_a_truncar);
    fill_bitmap(pointer_memory_bloque_de_datos + tamanio_a_truncar_en_bloques);

    //cargamos en el nuevo de bloque de datos el archivo a truncar
    t_metadata_archivo* metadata_archivo_a_truncar = (t_metadata_archivo*) dictionary_get(map_archivos_metadata, nombre_archivo_a_truncar);

    aux_pointer_memory_bloque_de_datos = pointer_memory_bloque_de_datos;
    for (int i = metadata_archivo_a_truncar->bloque_inicial; i < actual_nro_bloque_final_archivo; i++) {
        memcpy(bloques_de_archivos_nuevo + (aux_pointer_memory_bloque_de_datos * BLOCK_SIZE), bloques_datos_addr + (i * BLOCK_SIZE), BLOCK_SIZE);
        aux_pointer_memory_bloque_de_datos++;
    }

    metadata_archivo_a_truncar->bloque_inicial = pointer_memory_bloque_de_datos;
    metadata_archivo_a_truncar->tamanio_archivo = nuevo_tamanio_archivo_bytes;

    memcpy(bloques_datos_addr, bloques_de_archivos_nuevo, BLOCK_COUNT * BLOCK_SIZE);

    //escribimos en disco las modificaciones
    sync_file(bitmap_addr, BLOCK_COUNT);
    sync_file(bloques_datos_addr, BLOCK_COUNT * BLOCK_SIZE);

    usleep(RETRASO_COMPACTACION*1000);

    return metadata_archivo_a_truncar;
}

void io_fs_read(int cantidadParametros, t_list* parametrosRecibidos, uint32_t pid) {
    
    uint32_t cantidad_direcciones = *(uint32_t *)list_get(parametrosRecibidos, 0);
    uint32_t tamanio_a_leer = *(uint32_t *)list_get(parametrosRecibidos, cantidadParametros - 3);
    uint32_t registro_puntero_archivo = *(uint32_t *)list_get(parametrosRecibidos, cantidadParametros - 2);

    char* nombre_archivo = (char *)list_get(parametrosRecibidos, cantidadParametros - 1);

    log_info(logger_obligatorio, "PID: %d - Leer Archivo: %s - Tamaño a Leer: %d - Puntero Archivo: %d", pid, nombre_archivo, tamanio_a_leer, registro_puntero_archivo);

    void* valor_leido = malloc(tamanio_a_leer);
    t_metadata_archivo metadata = *(t_metadata_archivo*) dictionary_get(map_archivos_metadata, nombre_archivo);
    memcpy(valor_leido, bloques_datos_addr + (metadata.bloque_inicial * BLOCK_SIZE) + registro_puntero_archivo, tamanio_a_leer);

    escribir_valor_en_memoria(parametrosRecibidos, cantidad_direcciones, pid, tamanio_a_leer, valor_leido, fd_memoria);
}

void io_fs_write(int cantidadParametros, t_list* parametrosRecibidos, uint32_t pid) {

    uint32_t cantidad_direcciones = *(uint32_t *)list_get(parametrosRecibidos, 0);
    uint32_t tamanio_a_escribir = *(uint32_t *)list_get(parametrosRecibidos, cantidadParametros - 3);
    uint32_t registro_puntero_archivo = *(uint32_t *)list_get(parametrosRecibidos, cantidadParametros - 2);
    
    char* nombre_archivo = (char *)list_get(parametrosRecibidos, cantidadParametros - 1);

    log_info(logger_obligatorio, "PID: %d - Escribir Archivo: %s - Tamaño a Escribir: %d - Puntero Archivo: %d", pid, nombre_archivo, tamanio_a_escribir, registro_puntero_archivo);

    t_list *lecturas_memoria = leer_valor_de_memoria(fd_memoria, cantidad_direcciones, parametrosRecibidos, pid, tamanio_a_escribir);

    char* valor_leido = string_duplicate(list_get(lecturas_memoria, list_size(lecturas_memoria) - 1)); // En el ultimo valor de la lista de valores leidos, se encuentra el valor completo (o final)
    
    t_metadata_archivo metadata = *(t_metadata_archivo*) dictionary_get(map_archivos_metadata, nombre_archivo);
    memcpy(bloques_datos_addr + (metadata.bloque_inicial * BLOCK_SIZE) + registro_puntero_archivo, valor_leido, tamanio_a_escribir);
    sync_file(bloques_datos_addr, BLOCK_COUNT * BLOCK_SIZE);
    
    liberar_lista_de_datos_con_punteros(lecturas_memoria);
}

void io_fs_truncate(char* nombre_archivo_a_truncar, uint32_t nuevo_tamanio_archivo, uint32_t pid) {
    
    t_metadata_archivo* metadata_archivo_a_truncar = leer_metadata_archivo(nombre_archivo_a_truncar);

    float resultado_tamanio_actual_en_bloques_de_archivo_a_truncar =  ((float) metadata_archivo_a_truncar->tamanio_archivo / (float) BLOCK_SIZE);
    uint32_t tamanio_actual_en_bloques_de_archivo_a_truncar = ceil(resultado_tamanio_actual_en_bloques_de_archivo_a_truncar);

    if (tamanio_actual_en_bloques_de_archivo_a_truncar == 0) {
        tamanio_actual_en_bloques_de_archivo_a_truncar = 1;
    }

    uint32_t actual_nro_bloque_inicial_archivo = metadata_archivo_a_truncar->bloque_inicial;
    uint32_t actual_nro_bloque_final_archivo = actual_nro_bloque_inicial_archivo + tamanio_actual_en_bloques_de_archivo_a_truncar - 1;


    //Descartamos si el tamanio a truncar es igual al tamanio del archivo actual
    if (nuevo_tamanio_archivo == metadata_archivo_a_truncar->tamanio_archivo) {
        log_info(logger_auxiliar, "El tamanio a truncar del archivo: %s es igual al que ya tenia: %d", nombre_archivo_a_truncar, nuevo_tamanio_archivo);
        return;
    }

    //Primer chequeo: Debe crecer o achicarse el archivo
    if (nuevo_tamanio_archivo < metadata_archivo_a_truncar->tamanio_archivo) {
        achicar_archivo(nombre_archivo_a_truncar, nuevo_tamanio_archivo, metadata_archivo_a_truncar, actual_nro_bloque_final_archivo);
        return;
    }

    if (nuevo_tamanio_archivo > metadata_archivo_a_truncar->tamanio_archivo) {
        uint32_t tamanio_a_truncar_en_bytes = nuevo_tamanio_archivo - metadata_archivo_a_truncar->tamanio_archivo;
        float resultado_tamanio_a_truncar_en_bytes = ((float)tamanio_a_truncar_en_bytes / (float)BLOCK_SIZE);
        uint32_t tamanio_a_truncar_en_bloques = ceil(resultado_tamanio_a_truncar_en_bytes);

        uint32_t bloques_libres_al_final = obtener_cantidad_de_bloques_libres_al_final_de_archivo(actual_nro_bloque_final_archivo);
        
        //validamos si hay suficientes bloques libres una vez finalizado el archivo. 
        //si solo ocupa un bloque el nuevo tamaño, usamos ese directo, sin importar si mas adelante hay o no libres

        if (tamanio_a_truncar_en_bloques == 1 || bloques_libres_al_final >= tamanio_a_truncar_en_bloques) {
            if (tamanio_a_truncar_en_bloques != 1) {
                ocupar_bloques(actual_nro_bloque_final_archivo, tamanio_a_truncar_en_bloques, nombre_archivo_a_truncar);
            }
            metadata_archivo_a_truncar->tamanio_archivo = nuevo_tamanio_archivo;
            //escribir metadata
            escribir_metadata(metadata_archivo_a_truncar, nombre_archivo_a_truncar);
            sync_file(bitmap_addr, BLOCK_COUNT);
            return;
        }

        //si no hay suficientes bloques libres al final del archivo...
        if (bloques_libres_al_final < tamanio_a_truncar_en_bloques) {
            //Primero, antes de compactar, hacemos una busqueda a ver si no entra en algun lugar del disco el archivo completo (si encontramos, no es necesario compactar)
            int bloque_inicio_de_espacio_encontrado = buscar_espacio_libre_contiguo_en_disco(tamanio_a_truncar_en_bloques);
            
            uint32_t tamanio_a_truncar_en_bytes = nuevo_tamanio_archivo;
            float resultado_tamanio_a_truncar_en_bytes = ((float)tamanio_a_truncar_en_bytes / (float)BLOCK_SIZE);
            //El tamanio no debe ser la diferencia, tiene que ser el total de bloques
            uint32_t tamanio_a_truncar_en_bloques = ceil(resultado_tamanio_a_truncar_en_bytes);
            
            if (bloque_inicio_de_espacio_encontrado != -1) {
                //Se encontro un espacio contiguo libre suficiente en el disco
                usleep(RETRASO_COMPACTACION*1000);
                liberar_bloques(actual_nro_bloque_inicial_archivo, tamanio_actual_en_bloques_de_archivo_a_truncar, nombre_archivo_a_truncar);
                metadata_archivo_a_truncar->bloque_inicial = bloque_inicio_de_espacio_encontrado;
                metadata_archivo_a_truncar->tamanio_archivo = nuevo_tamanio_archivo;
                ocupar_bloques(bloque_inicio_de_espacio_encontrado, tamanio_a_truncar_en_bloques, nombre_archivo_a_truncar);
                escribir_metadata(metadata_archivo_a_truncar, nombre_archivo_a_truncar);
                sync_file(bitmap_addr, BLOCK_COUNT);
                return;
            }

            uint32_t tamanio_diferencia_a_truncar_en_bloques = tamanio_a_truncar_en_bloques - tamanio_actual_en_bloques_de_archivo_a_truncar;

            if (bloques_libres() >= tamanio_diferencia_a_truncar_en_bloques) {
                //Quedan espacios libres, pero estan dispersos, no contiguos, por lo tanto debemos compactar
                log_info(logger_obligatorio, "PID: %d - Inicio Compactación.", pid);
                t_metadata_archivo* metadata_archivo_a_truncar_actualizada = compactar(nombre_archivo_a_truncar, tamanio_actual_en_bloques_de_archivo_a_truncar, tamanio_diferencia_a_truncar_en_bloques, actual_nro_bloque_final_archivo, tamanio_a_truncar_en_bloques, nuevo_tamanio_archivo);
                log_info(logger_obligatorio, "PID: %d - Fin Compactación.", pid);
                escribir_metadata(metadata_archivo_a_truncar_actualizada, nombre_archivo_a_truncar);
                return;
            }

            log_error(logger_error, "No queda mas espacio para truncar archivo con tamanio en bloques: %d", tamanio_a_truncar_en_bloques);
        }

    }
}

void io_fs_delete(char* nombre_archivo_a_borrar) {

    char* path_archivo_a_eliminar = string_new();
    string_append(&path_archivo_a_eliminar, PATH_BASE_DIALFS);
    string_append(&path_archivo_a_eliminar, "/");
    string_append(&path_archivo_a_eliminar, nombre_archivo_a_borrar);


    log_info(logger_auxiliar, "Inicio función FS_delete");
    //Abrir archivo de metadata y leer primer bloque + tamaño en bytes
    t_metadata_archivo* metadata_archivo_a_borrar = leer_metadata_archivo(nombre_archivo_a_borrar);
    //aca se podría abstraer en un calcular_desplazamiento_de_bloques(archivo) o algo así
    float calculo_bloques_a_borrar = ((float) metadata_archivo_a_borrar->tamanio_archivo / (float) BLOCK_SIZE);
    uint32_t bloques_a_borrar = ceil(calculo_bloques_a_borrar); //incluyendo al inicial
    uint32_t primer_bloque = metadata_archivo_a_borrar->bloque_inicial;

    //limpiar en el bitmap los bloques correspondientes
    liberar_bloques(primer_bloque + bloques_a_borrar - 1, bloques_a_borrar, nombre_archivo_a_borrar); 
    // el -1 para evitar borrar de mas y eliminar "desde hasta" (si entendi bien)

    //cerrar y eliminar el archivo de metadata (remove)
    if (remove(path_archivo_a_eliminar) == 0) {
        log_info(logger_auxiliar, "El archivo \"%s\" fue eliminado correctamente.\n", nombre_archivo_a_borrar);
    } else {
        log_error(logger_error, "Error al intentar eliminar el archivo \"%s\".\n", nombre_archivo_a_borrar);
    }
    //abrir bloques.dat y limpiar los correspondientes (no hace falta segun issue3987)
}

char* enumToString(t_nombre_instruccion nombreDeInstruccion){
    switch(nombreDeInstruccion){
        case IO_GEN_SLEEP:
            return "IO_GEN_SLEEP";
        case IO_STDIN_READ:
            return "IO_STDIN_READ";
        case IO_STDOUT_WRITE:
            return "IO_STDOUT_WRITE";
        case IO_FS_CREATE:
            return "IO_FS_CREATE";
        case IO_FS_DELETE:
            return "IO_FS_DELETE";
        case IO_FS_TRUNCATE:
            return "IO_FS_TRUNCATE";
        case IO_FS_WRITE:
            return "IO_FS_WRITE";
        case IO_FS_READ:
            return "IO_FS_READ";
        default:
            return "COMANDO NO RECONOCIDO";
    };
}

void enviarMsjMemoria(){
    enviar_mensaje("Hola, soy I/O!", fd_memoria);
}

void enviarMsjKernel(){
    enviar_mensaje("Hola, soy I/O!", fd_kernel);
}

void cerrar_archivos() {
    close(fd_bitmap);
    close(fd_bloque_de_datos);
}

void liberar_archivo_map_metadata(void* map_metadata_element) {
    free(map_metadata_element);
}

void finalizar_fs() {
    bitarray_destroy(bitmap_mapeado);
    free(bloques_datos_addr);
    free(bitmap_addr);
    dictionary_destroy_and_destroy_elements(map_archivos_metadata, liberar_archivo_map_metadata);
}

void terminarPrograma() {
    log_destroy(logger_obligatorio);
    log_destroy(logger_auxiliar);
    log_destroy(logger_error);
    config_destroy(configuracion);
    liberar_conexion(fd_memoria);
    liberar_conexion(fd_kernel);
    cerrar_archivos();
    finalizar_fs();
}
