#ifndef CAST_FUNCTIONS_H_
#define CAST_FUNCTIONS_H_

#include <stdio.h> 
#include <stdlib.h> 
#include <ctype.h>
#include <stdbool.h>
#include "../types/types.h"

char* int_to_string(int input);
int string_to_int(const char* input);
bool is_number(char *str);
uint32_t get_registro_numerico_casteado_32b (void *registro_numerico_mapeado, tipo_de_dato tipo_de_dato_registro_numerico);

#endif /* CAST_FUNCTIONS_H_ */