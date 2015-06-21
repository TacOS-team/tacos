/**
 * @file ksignal.h
 *
 * @author TacOS developers 
 *
 * @section LICENSE
 *
 * Copyright (C) 2010-2015 TacOS developers.
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
 * Gestion des signaux côté kernel.
 */

#ifndef _KSIGNAL_H
#define _KSIGNAL_H

#include <ksyscall.h>
#include <signal_types.h>
#include <types.h>
#include <kprocess.h>

/**
 * Configure le handler à appeler pour un signal en particulier.
 *
 * @param signum Numéro du signal à configurer.
 * @param handler Adresse de la fonction à appeler.
 * @param ret Adresse dans lequel placer l'ancien handler.
 */
SYSCALL_HANDLER3(sys_signal, uint32_t signum, sighandler_t handler, sighandler_t* ret);

/**
 * Modifie les signaux bloqués.
 *
 * @param how Action à réaliser (block, unblock, set)
 * @param set Nouveau masque (selon how).
 * @param oldset Ancien masque.
 */
SYSCALL_HANDLER3(sys_sigprocmask, uint32_t how, sigset_t* set, sigset_t* oldset);

/**
 * Envoie un signal.
 *
 * @param pid PID du processus destinataire du signal.
 * @param signum Numéro du signal à envoyer.
 * @param ret Mis à 0 en cas de succès.
 */
SYSCALL_HANDLER3(sys_kill, int pid, int signum, int* ret);

/**
 * Syscall appelé lors de la fin d'exécution d'un handler.
 */
SYSCALL_HANDLER0(sys_sigret);

/**
 * Attend l'arrivée d'un signal.
 *
 * @param mask masque des signaux bloqués.
 */
SYSCALL_HANDLER1(sys_sigsuspend, sigset_t* mask);

/**
 * Configure un processus pour exécuter le prochain signal en attente.
 *
 * @param process Processus concerné.
 *
 * @return 1 si handler mis en place.
 */
int exec_sighandler(process_t* process);

/**
 * Test si le processus a un signal en attente.
 *
 * @param process Processus à tester.
 *
 * @return 1 si un signal est en attente.
 */
int signal_pending(process_t* process);

#endif /* _KSIGNAL_H */
