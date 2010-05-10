#ifndef SEM_H
#define SEM_H

#include <types.h>

int semget(uint8_t key);
int semcreate(uint8_t key);
int semdel(uint8_t semid);
int semP(uint8_t semid);
int semV(uint8_t semid);

#endif //SEM_H

