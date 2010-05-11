#ifndef SEM_H
#define SEM_H

#include <types.h>

int semget(uint8_t key);
int semcreate(uint8_t key);
int semdel(uint32_t semid);
int semP(uint32_t semid);
int semV(uint32_t semid);

#endif //SEM_H

