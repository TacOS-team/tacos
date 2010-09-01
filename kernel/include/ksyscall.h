#ifndef _KSYSCALL_H_
#define _KSYSCALL_H_

/**
 * @file string.h
 *
 */

#include <types.h>

#define UNUSED(X) uint32_t unused_ ## X __attribute__ ((unused))
#define SYSCALL_HANDLER0(name) void name(UNUSED(0), UNUSED(1), UNUSED(2))
#define SYSCALL_HANDLER1(name, param) void name(param, UNUSED(1), UNUSED(2))
#define SYSCALL_HANDLER2(name, param1, param2) void name(param1, param2, UNUSED(2))
#define SYSCALL_HANDLER3(name, param1, param2, param3) void name(param1, param2, param3)


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
