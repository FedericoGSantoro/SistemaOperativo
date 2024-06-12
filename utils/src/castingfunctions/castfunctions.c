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

bool is_number(char *str) {
    // Check if the string is empty
    if (*str == '\0') return 0;

    // Check each character of the string
    while (*str) {
        // If any character is not a digit, return false
        if (!isdigit(*str)) return 0;
        str++;
    }

    // If all characters are digits, return true
    return 1;
}