#ifndef KSEM_H
#define KSEM_H

#include <types.h>

int init_semaphores();
void sys_ksem(uint32_t param1, uint32_t param2, uint32_t param3);

int ksemget(uint8_t key);
int ksemcreate(uint8_t key);
int ksemdel(uint32_t semid);
int ksemP(uint32_t semid);
int ksemV(uint32_t semid);

#endif //KSEM_H

