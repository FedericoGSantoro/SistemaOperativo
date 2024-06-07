#include "mmu.h"

int solicitar_numero_de_marco(int num_pagina, int pid)
{
    t_paquete *paquete = crear_paquete(DEVOLVER_MARCO);

    agregar_a_paquete(paquete, &num_pagina, sizeof(int));
    agregar_a_paquete(paquete, &pid, sizeof(int));

    enviar_paquete(paquete, fd_memoria);

    int numero_marco;

    if(recv(fd_memoria, &numero_marco, sizeof(int), MSG_WAITALL) <= 0)
    {
        log_error(logger_aux_cpu, "Ocurrio un error al recibir el numero de marco");
        return -1;
    }

   
    return numero_marco;
}

int numero_pagina(int dir_logica)
{
    return floor(dir_logica / tam_pagina);
}

int traducir_direccion_mmu(int dir_logica, int pid)
{
    int desplazamiento = dir_logica - numero_pagina(dir_logica) * tam_pagina; // esto seria el resto entre la division de DL y tamanio de pagina (cuyo cociente es el numero de pagina)
   
    int num_marco = solicitar_numero_de_marco(numero_pagina(dir_logica), pid);
    if(num_marco == -1)
    {
        return -1;
    }
    
    int dir_fisica = (num_marco * tam_pagina) + desplazamiento;
    return dir_fisica;
}