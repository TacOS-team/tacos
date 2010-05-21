#ifndef UNISTD_H
#define UNISTD_H

/**
 * @file unistd.h
 */

#include <types.h>

/** 
 * @brief Endort le processus pour une durée déterminée (en secondes).
 *
 * Endort le processus pour une durée déterminée (en secondes).
 * 
 * @see usleep
 *
 * @param seconds Le nombre de secondes pendant lesquelles le processus
 * doit être endormi.
 * 
 * @return 0 si le temps prévu s'est écoulé.
 */

unsigned int sleep(unsigned int seconds);

/** 
 * @brief Endort le processus pour une durée déterminée (en microsecondes).
 * 
 * Endort le processus pour une durée déterminée (en microsecondes).
 *
 * @param milliseconds Le nombre de microsecondes pendant lesquelles le 
 * processus doit être endormi.
 * 
 * @return 0 en cas de succès, -1 en cas d'erreur.
 */
unsigned int usleep(unsigned int milliseconds);

size_t write(int fd, const void *buf, size_t count);

void* sys_write(uint32_t fd, uint32_t p_buf, uint32_t count);

#endif //UNISTD_H
