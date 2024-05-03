#include "./castfunctions.h"

char* int_to_string(int input){
    
    char *int_convertido; // Puntero a caracteres para almacenar la cadena convertida

    // Asignamos memoria para la cadena convertida
    int_convertido = (char *)malloc(20 * sizeof(char));
    if (int_convertido == NULL) {
        fprintf(stderr, "Error: No se pudo asignar memoria.\n");
        return "";
    }

    // Utilizamos sprintf para convertir el entero a una cadena
    sprintf(int_convertido, "%d", input);
    return int_convertido;
}

int string_to_int(const char* input){
    
    return atoi(input);
}