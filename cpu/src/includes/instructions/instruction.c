#include "./instruction.h"

t_instruccion* instruccion;

//Funciones para ejecutar instrucciones (execute)

void sum(int cantidad_parametros, t_instruccion* instruccion) {

    t_list* parametros = instruccion->parametros;
    
    uint32_t origen = *(uint32_t*) list_get(parametros, 0);
    uint32_t destino = *(uint32_t*) list_get(parametros, 1);

    destino += origen;
}

void set(int cantidad_parametros, t_instruccion* instruccion) {

    t_list* parametros = instruccion->parametros;
    
    uint32_t origen = *(uint32_t*) list_get(parametros, 0);
    uint32_t destino = *(uint32_t*) list_get(parametros, 1);

    destino = origen;
}


//Mapeo y lectura de instrucciones (decode)

t_tipo_instruccion mapear_tipo_instruccion(char *nombre_instruccion) {

    t_tipo_instruccion tipo_instruccion_mapped;

    if (string_equals_ignore_case(nombre_instruccion, "SET"))
        tipo_instruccion_mapped.nombre_instruccion = SET;
    else if (string_equals_ignore_case(nombre_instruccion, "SUM"))
        tipo_instruccion_mapped.nombre_instruccion = SUM;
    else if (string_equals_ignore_case(nombre_instruccion, "SUB"))
        tipo_instruccion_mapped.nombre_instruccion = SUB;
    else if (string_equals_ignore_case(nombre_instruccion, "MOV_IN"))
        tipo_instruccion_mapped.nombre_instruccion = MOV_IN;
    else if (string_equals_ignore_case(nombre_instruccion, "MOV_OUT"))
        tipo_instruccion_mapped.nombre_instruccion = MOV_OUT;
    else if (string_equals_ignore_case(nombre_instruccion, "RESIZE"))
        tipo_instruccion_mapped.nombre_instruccion = RESIZE;
    else if (string_equals_ignore_case(nombre_instruccion, "JNZ"))
        tipo_instruccion_mapped.nombre_instruccion = JNZ;
    else if (string_equals_ignore_case(nombre_instruccion, "COPY_STRING"))
        tipo_instruccion_mapped.nombre_instruccion = COPY_STRING;
    else if (string_equals_ignore_case(nombre_instruccion, "IO_GEN_SLEEP"))
        tipo_instruccion_mapped.nombre_instruccion = IO_GEN_SLEEP;
    else if (string_equals_ignore_case(nombre_instruccion, "IO_STDIN_READ"))
        tipo_instruccion_mapped.nombre_instruccion = IO_STDIN_READ;
    else if (string_equals_ignore_case(nombre_instruccion, "IO_STDOUT_WRITE"))
        tipo_instruccion_mapped.nombre_instruccion = IO_STDOUT_WRITE;
    else if (string_equals_ignore_case(nombre_instruccion, "IO_FS_CREATE"))
        tipo_instruccion_mapped.nombre_instruccion = IO_FS_CREATE;
    else if (string_equals_ignore_case(nombre_instruccion, "IO_FS_DELETE"))
        tipo_instruccion_mapped.nombre_instruccion = IO_FS_DELETE;
    else if (string_equals_ignore_case(nombre_instruccion, "IO_FS_TRUNCATE"))
        tipo_instruccion_mapped.nombre_instruccion = IO_FS_TRUNCATE;
    else if (string_equals_ignore_case(nombre_instruccion, "IO_FS_WRITE"))
        tipo_instruccion_mapped.nombre_instruccion = IO_FS_WRITE;
    else if (string_equals_ignore_case(nombre_instruccion, "IO_FS_READ"))
        tipo_instruccion_mapped.nombre_instruccion = IO_FS_READ;
    else if (string_equals_ignore_case(nombre_instruccion, "WAIT"))
        tipo_instruccion_mapped.nombre_instruccion = WAIT;
    else if (string_equals_ignore_case(nombre_instruccion, "SIGNAL"))
        tipo_instruccion_mapped.nombre_instruccion = SIGNAL;
    else if (string_equals_ignore_case(nombre_instruccion, "EXIT"))
        tipo_instruccion_mapped.nombre_instruccion = EXIT_PROGRAM;

    return tipo_instruccion_mapped;
}

uint32_t* mapear_registro(char *nombre_instruccion) {
    
    uint32_t* registroMapeado;

    if (string_equals_ignore_case(nombre_instruccion, "AX"))
        registroMapeado = &registros_cpu.ax;
    else if (string_equals_ignore_case(nombre_instruccion, "PC"))
        registroMapeado = &registros_cpu.pc;
    else if (string_equals_ignore_case(nombre_instruccion, "BX"))
        registroMapeado = &registros_cpu.bx;
    else if (string_equals_ignore_case(nombre_instruccion, "CX"))
        registroMapeado = &registros_cpu.cx;
    else if (string_equals_ignore_case(nombre_instruccion, "DX"))
        registroMapeado = &registros_cpu.dx;
    else if (string_equals_ignore_case(nombre_instruccion, "EAX"))
        registroMapeado = &registros_cpu.eax;
    else if (string_equals_ignore_case(nombre_instruccion, "EBX"))
        registroMapeado = &registros_cpu.ebx;
    else if (string_equals_ignore_case(nombre_instruccion, "ECX"))
        registroMapeado = &registros_cpu.ecx;
    else if (string_equals_ignore_case(nombre_instruccion, "EDX"))
        registroMapeado = &registros_cpu.edx;
    else if (string_equals_ignore_case(nombre_instruccion, "SI"))
        registroMapeado = &registros_cpu.si;
    else if (string_equals_ignore_case(nombre_instruccion, "DI"))
        registroMapeado = &registros_cpu.di;

    return registroMapeado;
}

t_instruccion *new_instruction(t_tipo_instruccion tipo_instruccion, t_list *parametros) {
    t_instruccion *tmp = malloc(sizeof(t_instruccion));
    tmp->tipo_instruccion = tipo_instruccion;
    tmp->cant_parametros = list_size(parametros);
    tmp->parametros = parametros;
    return tmp;
}

t_instruccion* procesar_instruccion(char *instruccion_entrante) {
    // Nos quedamos con el string hasta encontrar el \n
    char *parsed_instruccion_entrante= strtok(instruccion_entrante, "\n");
    // Separamos los tokens (nombre de instruccion y parametros) SET AX BX
    char **tokens = string_split(parsed_instruccion_entrante, " ");
    // Obtenemos el nombre de la instruccion
    char *identificador = tokens[0];
    t_tipo_instruccion tipo_instruccion = mapear_tipo_instruccion(identificador);
    // Agregamos a la lista los parametros de la instruccion
    t_list *parameters = list_create();
    int i = 1; // A partir de 1 son parametros - La lista puede estar vacia
    uint32_t* registro_mapeado;
    while (tokens[i] != NULL) {
        registro_mapeado = mapear_registro((void *)tokens[i]);
        list_add(parameters, registro_mapeado);
        i++;
    }
    t_instruccion *instruccion_obtenida = new_instruction(tipo_instruccion, parameters);
    free(identificador);
    free(tokens);
    return instruccion_obtenida;
}

//Funcion para eliminar instruccion de memoria

void liberar_instruccion() {
    liberar_lista_de_datos_planos(instruccion->parametros);
    free(instruccion);
}