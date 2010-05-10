#ifndef KSEM_H
#define KSEM_H

#include <types.h>

int init_semaphores();
void ksem_syscallhandle(uint32_t param1, uint32_t param2, uint32_t param3);
int ksemget(uint8_t key);
int ksemcreate(uint8_t key);
int ksemdel(uint8_t semid);
int ksemP(uint8_t semid);
int ksemV(uint8_t semid);

#endif //KSEM_H

