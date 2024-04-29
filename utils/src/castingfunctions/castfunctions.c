#include "./castfunctions.h"

char* int_to_string(int input){
    
    char* int_convertido = "";
    sprintf(int_convertido, "%d", input);
    return int_convertido;
}