#include "./instruction_fetcher.h"

//obtiene instruccion del mapa, a partir del pid (hace el list_get con un indice dado [PC])
char* fetch_instruccion(int pid, int pc) {
    
    char* pid_convertido = int_to_string(pid);
    t_list* instrucciones = (t_list*) dictionary_get(cache_instrucciones, pid_convertido);
    
    char* instruccion = list_get(instrucciones, pc);
    return instruccion;
}

void rellenar_cache_de_instrucciones(FILE* file_instructions, int pid) {
   
    size_t len = 0;    // tamanio de la linea leida - USADAS PARA GETLINE
    size_t read;       // cant de caracteres leido - USADAS PARA GETLINE
    char *line = NULL; // linea leida
    t_list* lista_de_instrucciones = list_create(); //lista para almacenar lineas de instrucciones por proceso

    while ((read = getline(&line, &len, file_instructions)) != -1)
    {
        list_add(lista_de_instrucciones, line);
    }

    char* pid_convertido = int_to_string(pid);
    dictionary_put(cache_instrucciones, pid_convertido, lista_de_instrucciones);
    free(line);
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