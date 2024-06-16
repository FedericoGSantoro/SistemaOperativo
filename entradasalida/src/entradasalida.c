#include "entradasalida.h"

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
    op_codigo codigoRecibido;
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
            usleep(tiempoSleep);
            enviar_codigo_op(OK_OPERACION, fd_kernel);
            break;

        case IO_STDIN_READ:
            if(TIPO_INTERFAZ != STDIN){
                log_error(logger_error, "Se envió la instrucción IO_STDIN_READ a la interfaz no STDIN: %s", nombre);
                break;
            }
            int cantidadParametros = list_size(parametrosRecibidos);
            if(1 < cantidadParametros ){
                int direccionesMemoria[cantidadParametros-1];
                for(int i=0; i < cantidadParametros-1; i++){
                    direccionesMemoria[i] = *(int*) list_get(parametrosRecibidos, i);
                }
                int tamanio = *(int*) list_get(parametrosRecibidos, cantidadParametros);
                char* valorLeido = readline("<Ingrese un valor:> ");
                t_paquete* paqueteMemoria = crear_paquete(ESCRITURA);
                for (int i = 0; i < cantidadParametros -1; i++){
                    agregar_a_paquete(paqueteMemoria, &direccionesMemoria[i], sizeof(int));
                }
                agregar_a_paquete(paqueteMemoria, tamanio, sizeof(int));
                agregar_a_paquete(paqueteMemoria, valorLeido, strlen(valorLeido) + 1);
                enviar_paquete(paqueteMemoria, fd_memoria);
                eliminar_paquete(paqueteMemoria);
                //Ver como recibo valor de memoria
                op_codigo op = recibir_operacion(fd_memoria);
                switch (op){
                case OK_OPERACION:
                    log_info(logger_auxiliar, "Se escribio el valor '%s' en memoria", valorLeido);
                    enviar_codigo_op(OK_OPERACION, fd_kernel);
                    break;
                default:
                    log_error(logger_error, "Fallo escritura de Memoria con el valor: %s", valorLeido);
                    enviar_codigo_op(ERROR_OPERACION, fd_kernel);
                    break;
                }
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
            int cantidadParametros = list_size(parametrosRecibidos);
            if(1 < cantidadParametros ){
                int direccionesMemoria[cantidadParametros-1];
                for(int i=0; i < cantidadParametros-1; i++){
                    direccionesMemoria[i] = *(int*) list_get(parametrosRecibidos, i);
                }
                int tamanio = *(int*) list_get(parametrosRecibidos, cantidadParametros);

                t_paquete* paqueteMemoria = crear_paquete(LECTURA);
                for (int i = 0; i < cantidadParametros -1; i++){
                    agregar_a_paquete(paqueteMemoria, &direccionesMemoria[i], sizeof(int));
                }
                agregar_a_paquete(paqueteMemoria, tamanio, sizeof(int));
                enviar_paquete(paqueteMemoria, fd_memoria);
                eliminar_paquete(paqueteMemoria);
                //Ver como recibo valor de memoria
                op_codigo op = recibir_operacion(fd_memoria);
                switch (op){
                case OK_OPERACION:
                    t_list* paqueteRecibido = recibir_paquete(fd_memoria);
                    char* valorAMostrar = string_new();
                    for (int i = 0; i < cantidadParametros-1; i++){
                        string_append_with_format(valorAMostrar, "%s", *(char*)list_get(paqueteRecibido,i));
                    }
                    printf(valorAMostrar);
                    enviar_codigo_op(OK_OPERACION, fd_kernel);
                    break;
                default:
                    log_error(logger_error, "Fallo lectura de Memoria");
                    enviar_codigo_op(ERROR_OPERACION, fd_kernel);
                    break;
                }
            }else{
                log_error(logger_error, "No se recibio lo necesario para leer de memoria");
                enviar_codigo_op(ERROR_OPERACION, fd_kernel);
            }
            break;
        
        case IO_FS_CREATE:
            if(TIPO_INTERFAZ != FS){
                log_error(logger_error, "Se envio la instrucción IO_FS_CREATE a la interfaz no FS: %s", nombre);
                break;
            }
            break;

        case IO_FS_DELETE:
            if(TIPO_INTERFAZ != FS){
                log_error(logger_error, "Se envió la instrucción IO_FS_DELETE a la interfaz no FS: %s", nombre);
                break;
            }
            break;

        case IO_FS_READ:
            if(TIPO_INTERFAZ != FS){
                log_error(logger_error, "Se envió la instrucción IO_FS_READ a la interfaz no FS: %s", nombre);
                break;
            }
            break;

        case IO_FS_WRITE:
            if(TIPO_INTERFAZ != FS){
                log_error(logger_error, "Se envió la instrucción IO_FS_WRITE a la interfaz no FS: %s", nombre);
                break;
            }
            break;

        case IO_FS_TRUNCATE:
            if(TIPO_INTERFAZ != FS){
                log_error(logger_error, "Se envió la instrucción IO_FS_TRUNCATE a la interfaz no FS: %s", nombre);
                break;
            }
            break;
        
        default:
            log_error(logger_error, "Se recibio una instruccion no esperada: %d", tipoInstruccion);
            break;
        }
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

            default:
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

/*
void enviarMsj(){
    char* comandoLeido = readline("String > ");
    enviar_mensaje(comandoLeido, socketAEnviar);
    log_info(logger_auxiliar, "Mensaje enviado");
    free(comandoLeido);
}

void enviarPaquete() {
    char* comandoLeido;
	t_paquete* paquete = crear_paquete(PAQUETE);

	// Leemos y esta vez agregamos las lineas al paquete
	comandoLeido = readline("String > "); // Leo de consola
	while (strcmp(comandoLeido, "")){ // Mientras no sea cadena vacia
		agregar_a_paquete(paquete, comandoLeido, strlen(comandoLeido)+1); // Agregamos al paquete el stream
		comandoLeido = readline("String > "); // Leo nueva linea
	}
	enviar_paquete(paquete, socketAEnviar); // Enviamos el paquete
    log_info(logger_auxiliar, "Paquete enviado");
	free(comandoLeido);
	eliminar_paquete(paquete);
}

void enviarOperacionA() {
    char* moduloNombre; // No hace falta liberar ya que es cadena literal
    pthread_t hiloAEnviar;
    if ( !strcmp(enviarA, "KERNEL") ) {
        hiloAEnviar = hilo_kernel;
        socketAEnviar = fd_kernel;
        moduloNombre = "Kernel";
    }
    else if ( !strcmp(enviarA, "MEMORIA") ) {
        hiloAEnviar = hilo_memoria;
        socketAEnviar = fd_memoria;
        moduloNombre = "Memoria";
    }
    else {
        log_error(logger_error, "Destino Incorrecto");
        return;
    }
    crearHiloJoin(&hiloAEnviar, (void*) tipoOperacion, NULL, moduloNombre, logger_auxiliar, logger_error);
}

op_codigo transformarAOperacion(char* operacionLeida) {
    string_to_upper(operacionLeida);
    if ( !strcmp(operacionLeida, "MENSAJE") ) { // strcmp devuelve 0 si son iguales
        return MENSAJE;
    } else if ( !strcmp(operacionLeida, "PAQUETE") ) {
        return PAQUETE;
    } else {
        return -1; // Valor por defecto para indicar error
    }
}
*/

void inicializar(){
    inicializarLogs();

    inicializarConfig();

    inicializarConexiones();

    parametrosRecibidos = list_create();
}

void inicializarLogs(){
    logger_obligatorio = log_create("entradasalida.log", "LOG_OBLIGATORIO_ENTRADA-SALIDA", true, LOG_LEVEL_INFO);
    logger_auxiliar = log_create("entradasalidaExtras.log", "LOG_EXTRA_ENTRADA_SALIDA", true, LOG_LEVEL_INFO);
    logger_error = log_create("entradasalidaExtras.log", "LOG_ERROR_ENTRADA_SALIDA", true, LOG_LEVEL_ERROR);
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
    log_info(logger_auxiliar, "Tipo interfaz: %d", TIPO_INTERFAZ);
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
    //crearHiloDetach(&hilo_memoria, (void*)enviarMsjMemoria, NULL, "Memoria", logger_auxiliar, logger_error);

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

void terminarPrograma() {
    log_destroy(logger_obligatorio);
    log_destroy(logger_auxiliar);
    log_destroy(logger_error);
    config_destroy(configuracion);
    liberar_conexion(fd_memoria);
    liberar_conexion(fd_kernel);
}
