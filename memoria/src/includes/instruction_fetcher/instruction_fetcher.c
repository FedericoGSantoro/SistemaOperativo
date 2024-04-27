#include "./instruction_fetcher.h"

//TODO: Ver donde ponerla (dificil)
char* int_to_string(int a){
    char* int_convertido = "";
    sprintf(int_convertido, "%d", a);
    return int_convertido;
}

//obtiene instruccion del mapa, a partir del pid (hace el pop en la cola de instrucciones)
char* fetch_instruccion(int pid) {
    
    char* pid_convertido = int_to_string(pid);
    t_queue* instrucciones = (t_queue*) dictionary_get(cache_instrucciones, pid_convertido);
    
    char* instruccion = (char*) queue_pop(instrucciones);
    
    if (queue_is_empty(instrucciones)) {
        dictionary_remove_and_destroy(cache_instrucciones, pid_convertido, destroyer_queue_con_datos_simples);
    } 

    return instruccion;
}

void rellenar_cache_de_instrucciones(FILE* file_instructions, int pid) {
   
    size_t len = 0;    // tamanio de la linea leida - USADAS PARA GETLINE
    size_t read;       // cant de caracteres leido - USADAS PARA GETLINE
    char *line = NULL; // linea leida
    t_queue* cola_de_instrucciones = queue_create(); //cola para almacenar lineas de instrucciones por proceso

    while ((read = getline(&line, &len, file_instructions)) != -1)
    {
        queue_push(cola_de_instrucciones, line);
    }


    char* pid_convertido = int_to_string(pid);
    dictionary_put(cache_instrucciones, pid_convertido, cola_de_instrucciones);
    free(line);
}

//rellena el mapa de instrucciones del proceso (pid) con el path obtenido 
//(key = pid, value = cola de instrucciones)
void crear_instrucciones(char* path, int pid) {
    
    FILE* file_instructions = fopen(path, "rt");

    if (file_instructions == NULL)
    {
        log_error(loggerError, "No se pudo abrir el archivo de instrucciones");
        abort();
    }

    // rellenar diccionario con lineas del archivo pseudocodigo (instrucciones)
    rellenar_cache_de_instrucciones(file_instructions, pid);
}