#include "./instruction_fetcher.h"

t_list* obtener_instrucciones_cacheadas(int pid) {

    char* pid_convertido = int_to_string(pid);
    t_list* instrucciones = (t_list*) dictionary_get(cache_instrucciones, pid_convertido);

    free(pid_convertido);
    return instrucciones;
}

//obtiene instruccion del mapa, a partir del pid (hace el list_get con un indice dado [PC])
char* fetch_instruccion(int pid, int pc) {
    
    t_list* instrucciones = obtener_instrucciones_cacheadas(pid);

    char* instruccion = "";
    if (pc < list_size(instrucciones)) {
        instruccion = (char*) list_get(instrucciones, pc);
    }
    
    return instruccion;
}

void rellenar_cache_de_instrucciones(FILE* file_instructions, int pid) {
   
    size_t len = 0;    // tamanio de la linea leida - USADAS PARA GETLINE
    size_t read;       // cant de caracteres leido - USADAS PARA GETLINE
    char *line = NULL; // linea leida
    t_list* lista_de_instrucciones = list_create(); //lista para almacenar lineas de instrucciones por proceso
    int index = 0; // index para agregar ordenadamente en lista

    while ((read = getline(&line, &len, file_instructions)) != -1)
    {
            char *linea_a_agregar = string_duplicate(line);
            list_add_in_index(lista_de_instrucciones, index, linea_a_agregar);
            index++;
    }

    char* pid_convertido = int_to_string(pid);
    dictionary_put(cache_instrucciones, pid_convertido, lista_de_instrucciones);
}

//rellena el mapa de instrucciones del proceso (pid) con el path obtenido 
//(key = pid, value = lista de instrucciones)
void crear_instrucciones(char* path, int pid) {
    
    FILE* file_instructions = fopen(path, "r");

    if (file_instructions == NULL)
    {
        log_error(loggerError, "No se pudo abrir el archivo de instrucciones");
        abort();
    }

    // rellenar diccionario con lineas del archivo pseudocodigo (instrucciones)
    rellenar_cache_de_instrucciones(file_instructions, pid);
}

//eliminar el cache de instrucciones del proceso (pid) 
//(key = pid, value = lista de instrucciones)
void eliminar_instrucciones(int pid) {

    char* pid_convertido = int_to_string(pid);
    destroyer_dictionary_key_con_estructuras(cache_instrucciones, pid_convertido, liberar_lista_de_datos_con_punteros);
    free(pid_convertido);
}