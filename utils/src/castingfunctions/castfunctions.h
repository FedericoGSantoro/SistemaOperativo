#ifndef CAST_FUNCTIONS_H_
#define CAST_FUNCTIONS_H_

#include <stdio.h> 
#include <stdlib.h> 
#include <ctype.h>
#include <stdbool.h>

char* int_to_string(int input);
int string_to_int(const char* input);
bool is_number(char *str);

#endif /* CAST_FUNCTIONS_H_ */