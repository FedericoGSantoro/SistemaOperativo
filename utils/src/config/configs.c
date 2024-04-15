#include "./configs.h"

t_config* iniciar_config(char* path, t_log* logger_error, void*(*terminar_programa)()){
    t_config* modulo_config = config_create(path);
    if (modulo_config == NULL ) {
        log_error(logger_error, "No se pudo cargar la configuracion");
        terminar_programa();
        abort();
    }
    return modulo_config;
}