#include "./configs.h"

t_config* iniciar_config(char* path){
    t_config* modulo_config = config_create(path);
    if (modulo_config == NULL ) {
        abort();
    }
    return modulo_config;
}