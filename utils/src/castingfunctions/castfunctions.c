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

    if (*str == '\0') return 0;

    while (*str) {
        if (!isdigit(*str)) return 0;
        str++;
    }

    return 1;
}

uint32_t get_registro_numerico_casteado_32b (void *registro_numerico_mapeado, tipo_de_dato tipo_de_dato_registro_numerico) {
    
    if (tipo_de_dato_registro_numerico == UINT8) {
        uint8_t registro_direccion_valor = *(uint8_t*) (registro_numerico_mapeado);
        return registro_direccion_valor;
    } else {
        return *(uint32_t*) (registro_numerico_mapeado);
    }
}