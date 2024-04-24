#include "./instruction_fetcher.h"

t_list* fetch_instruction(char* path) {
    
    FILE* file_instructions = fopen(path, "rt");

    if (file_instructions == NULL)
    {
        log_error(loggerError, "No se pudo abrir el archivo de instrucciones");
        terminar_programa();
        abort();
    }

    // parsear archivo pseudocodigo
    t_list* lista_instrucciones = mapear_instruciones_pseudocodigo(file_instructions);

    if (lista_instrucciones == NULL)
    {
        terminar_programa();
        abort();
    }
}

t_nombre_instruccion mapear_nombre_instruccion(char *nombre_instruccion)
{
    t_nombre_instruccion nombre_instruccion_mapped;

    if (string_equals_ignore_case(nombre_instruccion, "SET"))
        nombre_instruccion_mapped = SET;
    else if (string_equals_ignore_case(nombre_instruccion, "SUM"))
        nombre_instruccion_mapped = SUM;
    else if (string_equals_ignore_case(nombre_instruccion, "SUB"))
        nombre_instruccion_mapped = SUB;
    else if (string_equals_ignore_case(nombre_instruccion, "MOV_IN"))
        nombre_instruccion_mapped = MOV_IN;
    else if (string_equals_ignore_case(nombre_instruccion, "MOV_OUT"))
        nombre_instruccion_mapped = MOV_OUT;
    else if (string_equals_ignore_case(nombre_instruccion, "RESIZE"))
        nombre_instruccion_mapped = RESIZE;
    else if (string_equals_ignore_case(nombre_instruccion, "JNZ"))
        nombre_instruccion_mapped = JNZ;
    else if (string_equals_ignore_case(nombre_instruccion, "COPY_STRING"))
        nombre_instruccion_mapped = COPY_STRING;
    else if (string_equals_ignore_case(nombre_instruccion, "IO_GEN_SLEEP"))
        nombre_instruccion_mapped = IO_GEN_SLEEP;
    else if (string_equals_ignore_case(nombre_instruccion, "IO_STDIN_READ"))
        nombre_instruccion_mapped = IO_STDIN_READ;
    else if (string_equals_ignore_case(nombre_instruccion, "IO_STDOUT_WRITE"))
        nombre_instruccion_mapped = IO_STDOUT_WRITE;
    else if (string_equals_ignore_case(nombre_instruccion, "IO_FS_CREATE"))
        nombre_instruccion_mapped = IO_FS_CREATE;
    else if (string_equals_ignore_case(nombre_instruccion, "IO_FS_DELETE"))
        nombre_instruccion_mapped = IO_FS_DELETE;
    else if (string_equals_ignore_case(nombre_instruccion, "IO_FS_TRUNCATE"))
        nombre_instruccion_mapped = IO_FS_TRUNCATE;
    else if (string_equals_ignore_case(nombre_instruccion, "IO_FS_WRITE"))
        nombre_instruccion_mapped = IO_FS_WRITE;
    else if (string_equals_ignore_case(nombre_instruccion, "IO_FS_READ"))
        nombre_instruccion_mapped = IO_FS_READ;
    else if (string_equals_ignore_case(nombre_instruccion, "WAIT"))
        nombre_instruccion_mapped = WAIT;
    else if (string_equals_ignore_case(nombre_instruccion, "SIGNAL"))
        nombre_instruccion_mapped = SIGNAL;
    else if (string_equals_ignore_case(nombre_instruccion, "EXIT"))
        nombre_instruccion_mapped = EXIT;

    return nombre_instruccion_mapped;
}

void add_param_size_to_instruction(t_list *parametros, t_instruccion *instruccion)
{
    int i = 0;
    while (i < instruccion->cant_parametros)
    {
        char *param = list_get(parametros, i);
        instruccion->p_length[i] = strlen(param) + 1;
        i++;
    }
}

t_instruccion *new_instruction(t_nombre_instruccion nombre_instruccion, t_list *parametros)
{
    t_instruccion *tmp = malloc(sizeof(t_instruccion));
    tmp->nombre_instruccion = nombre_instruccion;
    tmp->cant_parametros = list_size(parametros);
    tmp->parametros = parametros;
    for (size_t i = 0; i < 4; i++)
        tmp->p_length[i] = 0; //inicalizamos la lista de las longitudes en 0 para luego, si es necesario, enviarlo en el buffering para el armado del paquete
    add_param_size_to_instruction(parametros, tmp);
    return tmp;
}

t_instruccion* process_line(char *line)
{
    // por cada linea que leo, obtengo los tokens, armo la instrucciones con sus parametros y la agrego a la lista
    char *parsedLIne= strtok(line, "\n");         // separo la primera linea y almaceno en un char
    char **tokens = string_split(parsedLIne, " "); // separo los tokens (nombre de instruccion y parametros) SET AX BX
    // Obtengo el nombre de la instruccion
    char *identificador = tokens[0];
    t_nombre_instruccion nombre_instruccion = mapear_nombre_instruccion(identificador);
    // Agrego a la lista los parametros de la instruccion
    t_list *parameters = list_create();
    int i = 1; // A partir de 1 son parametros - La lista puede estar vacia
    while (tokens[i] != NULL)
    {
        list_add(parameters, (void *)tokens[i]);
        i++;
    }
    t_instruccion *instruccion = new_instruction(nombre_instruccion, parameters);
    free(identificador);
    free(tokens);
    return instruccion;
}

t_list* mapear_instruciones_pseudocodigo(char* file_instructions) {
   
    t_list*list_instructions = list_create();
    size_t len = 0;    // tamanio de la linea leida - USADAS PARA GETLINE
    size_t read;       // cant de caracteres leido - USADAS PARA GETLINE
    char *line = NULL; // linea leida
    while ((read = getline(&line, &len, file_instructions)) != -1)
    {
        t_instruccion *instruccion = process_line(line);
        list_add(list_instructions, instruccion);
    }
    free(line);
    return list_instructions;

}
