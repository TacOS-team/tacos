#ifndef _KSYSCALL_H_
#define _KSYSCALL_H_

/**
 * @file string.h
 *
 */

#include <types.h>

#define MAX_SYSCALL_NB 256

typedef void (*syscall_handler_t)(uint32_t,uint32_t,uint32_t);

/** 
 * @brief Initialisation des appels systemes.
 * 
 * Initialise le système pour pouvoir exécuter correctement les appels systèmes
 *
 */
void init_syscall();

/**
 * @brief Associe un identifiant d'appel système à une handler
 * 
 * @param syscall_id Identifian de l'appel système.
 * @param handler Adresse du handler
 * 
 * @return 0 en cas de succès, une autre valeur sinon
 */
int syscall_set_handler(uint32_t syscall_id, syscall_handler_t handler);

#endif
