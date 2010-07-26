#ifndef KSEM_H
#define KSEM_H

#include <types.h>

int init_semaphores();
void sys_ksem(uint32_t param1, uint32_t param2, uint32_t param3);

#endif //KSEM_H

