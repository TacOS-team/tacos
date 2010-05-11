#ifndef KSEM_H
#define KSEM_H

#include <types.h>

int init_semaphores();
void ksem_syscallhandle(uint32_t param1, uint32_t param2, uint32_t param3);

#endif //KSEM_H

