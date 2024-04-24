#ifndef INSTRUCTION_FETCHER_H_
#define INSTRUCTION_FETCHER_H_


#include "../../../../utils/src/config/configs.h"
#include "../../../../utils/src/types/types.h"
#include <commons/collections/queue.h>
#include "../memoria_vars.h"

t_list* fetch_instruction(char* path);
t_list* mapear_instruciones_pseudocodigo(char* file_instructions);

#endif /* INSTRUCTION_FETCHER_H_ */