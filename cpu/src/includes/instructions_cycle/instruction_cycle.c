#include "./instruction.h"

t_instruccion* instruccion;



uint32_t* mapear_registro(char *nombre_registro) {
    
    if (string_equals_ignore_case(nombre_registro, "AX"))
        return &(registros_cpu.ax);
    else if (string_equals_ignore_case(nombre_registro, "PC"))
        return &(registros_cpu.pc);
    else if (string_equals_ignore_case(nombre_registro, "BX"))
        return &(registros_cpu.bx);
    else if (string_equals_ignore_case(nombre_registro, "CX"))
        return &(registros_cpu.cx);
    else if (string_equals_ignore_case(nombre_registro, "DX"))
        return &(registros_cpu.dx);
    else if (string_equals_ignore_case(nombre_registro, "EAX"))
        return &(registros_cpu.eax);
    else if (string_equals_ignore_case(nombre_registro, "EBX"))
        return &(registros_cpu.ebx);
    else if (string_equals_ignore_case(nombre_registro, "ECX"))
        return &(registros_cpu.ecx);
    else if (string_equals_ignore_case(nombre_registro, "EDX"))
        return &(registros_cpu.edx);
    else if (string_equals_ignore_case(nombre_registro, "SI"))
        return &(registros_cpu.si);
    else if (string_equals_ignore_case(nombre_registro, "DI"))
        return &(registros_cpu.di);
    else {
        valor_registro_numerico = string_to_int(nombre_registro);
        return &valor_registro_numerico;
    }
}

//Funciones para ejecutar instrucciones (execute)

void sum_instruction(int cantidad_parametros, t_list* parametros) {
    
    char* destino = (char*) list_get(parametros, 0);
    char* origen = (char*) list_get(parametros, 1);

    uint32_t* registro_destino = mapear_registro(destino);
    uint32_t* registro_origen = mapear_registro(origen);

    *registro_destino += *registro_origen;
}

void sub_instruction(int cantidad_parametros, t_list* parametros) {
    
    char* destino = (char*) list_get(parametros, 0);
    char* origen = (char*) list_get(parametros, 1);

    uint32_t* registro_destino = mapear_registro(destino);
    uint32_t* registro_origen = mapear_registro(origen);

    *registro_destino -= *registro_origen;
}

void set_instruction(int cantidad_parametros, t_list* parametros) {

    char* destino = (char*) list_get(parametros, 0);
    char* origen = (char*) list_get(parametros, 1);

    uint32_t* registro_destino = mapear_registro(destino);
    uint32_t* registro_origen = mapear_registro(origen);

    *registro_destino = *registro_origen;
}

void exit_instruction(int cantidad_parametros, t_list* parametros) {
    motivo_bloqueo = INTERRUPCION_FIN_EVENTO;
}

void jnz_instruction(int cantidad_parametros, t_list* parametros) {

    char* registro = (char*) list_get(parametros, 0);
    char* instruccion_a_moverse = (char*) list_get(parametros, 1);

    uint32_t* registro_mapeado = mapear_registro(registro);
    uint32_t* instruccion_a_moverse_mapeada = mapear_registro(instruccion_a_moverse);

    if (registro_mapeado != 0) {
        registros_cpu.pc = *instruccion_a_moverse_mapeada - 2; //restamos uno porque despues vamos a sumar uno con el proximo PC++, al finalizar un ciclo de instruccion que tenga a JNZ. Me llevo a consultar si queda en bulce infinito, o como cortamos el JNZ. El otro -1 es para que no se nos vaya la posicion del array
    }
}

//Mapeo y lectura de instrucciones (decode)

t_tipo_instruccion mapear_tipo_instruccion(char *nombre_instruccion) {

    t_tipo_instruccion tipo_instruccion_mapped;

    if (string_equals_ignore_case(nombre_instruccion, "SET")) {
        tipo_instruccion_mapped.nombre_instruccion = SET;
        tipo_instruccion_mapped.execute = set_instruction;
    } else if (string_equals_ignore_case(nombre_instruccion, "SUM")) {
        tipo_instruccion_mapped.nombre_instruccion = SUM;
        tipo_instruccion_mapped.execute = sum_instruction;
    }
    else if (string_equals_ignore_case(nombre_instruccion, "SUB")) {
        tipo_instruccion_mapped.nombre_instruccion = SUB;
        tipo_instruccion_mapped.execute = sub_instruction;
    }
    else if (string_equals_ignore_case(nombre_instruccion, "MOV_IN"))
        tipo_instruccion_mapped.nombre_instruccion = MOV_IN;
    else if (string_equals_ignore_case(nombre_instruccion, "MOV_OUT"))
        tipo_instruccion_mapped.nombre_instruccion = MOV_OUT;
    else if (string_equals_ignore_case(nombre_instruccion, "RESIZE"))
        tipo_instruccion_mapped.nombre_instruccion = RESIZE;
    else if (string_equals_ignore_case(nombre_instruccion, "JNZ")) {
        tipo_instruccion_mapped.nombre_instruccion = JNZ;
        tipo_instruccion_mapped.execute = jnz_instruction;
    }
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
    else if (string_equals_ignore_case(nombre_instruccion, "EXIT")) {
        tipo_instruccion_mapped.nombre_instruccion = EXIT_PROGRAM;
        tipo_instruccion_mapped.execute = exit_instruction;
    }

    return tipo_instruccion_mapped;
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
    int i = 1; // A partir de 1 son parametros - La lista puede estar vacia.
    char* registro_mapeado;
    while (tokens[i] != NULL) {
        registro_mapeado = tokens[i];
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