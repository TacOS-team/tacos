/**
 * @file ksyscall.h
 *
 * @author TacOS developers 
 *
 *
 * @section LICENSE
 *
 * Copyright (C) 2010, 2011, 2012 - TacOS developers.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 3 of
 * the License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details at
 * http://www.gnu.org/copyleft/gpl.html
 *
 * You should have received a copy of the GNU General Public License 
 * along with this program; if not, see <http://www.gnu.org/licenses>.
 *
 * @section DESCRIPTION
 *
 * Description de ce que fait le fichier
 */

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

SYSCALL_HANDLER3(sys_dummy, uint32_t a,uint32_t b,uint32_t c);

#endif
