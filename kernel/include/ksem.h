/**
 * @file ksem.h
 *
 * @author TacOS developers 
 *
 *
 * @section LICENSE
 *
 * Copyright (C) 2010 - TacOS developers.
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
 * Coté noyau de la gestion des sémaphores
 */

#ifndef KSEM_H
#define KSEM_H

#include <ksyscall.h>
#include <types.h>

#define SEM_NEW 255
#define SEM_CREATE 1

#define SEM_GETVAL 1
#define SEM_DEL 2
#define SEM_ZCNT 3
#define SEM_SET 4

/**
 * @brief Initialisation du système des sémaphores
 * 
 * @return 0
 */
int init_semaphores();

/**
 * @brief Handler de l'appel système SYS_SEM
 * Dans la pratique, sys_ksem ne fait qu'exécuter la fonction correspondant à l'opération demandée
 * 
 * @param op	Type d'opération à exécuter
 * @param param	Dépend du type d'opération
 * @param ret	Adresse pour la valeur de retour
 */
SYSCALL_HANDLER3(sys_ksem, uint32_t op, uint32_t param, uint32_t ret);

/**
 * @brief Obtenir ou créer un sémaphore.
 * ksemget retourne un semid propre au processus, associé au sémaphore désigné par key
 * 
 * @param key clé identifiant le sémaphore
 * 
 * @return semid associé au sémaphore identifié par key
 * -1 en cas d'erreur
 */
int ksemget(uint8_t key, int flags);

/**
 * @brief Manipulation d'un sémaphore.
 * ksemctl permet de manipuler un sémaphore (suppression, libération, valeur, etc.)
 */
int ksemctl(uint8_t key, int cmd, void* res);

/**
 * @brief Opération P sur un sémaphore
 * Réalise l'opération P sur le sémaphore.
 * Le processus essaye donc de prendre le sémaphore si il est libre (ie Sa valeur des supérieur à 0).
 * Si ce n'est pas le cas, il met son pid dans la fifo du sémaphore pour 
 * pouvoir être réveillé en temps voulu, et se met en état d'attente.
 * 
 * @param semid Identifiant du sémaphore sur lequel l'opération doit être réalisée
 * 
 * @return 0 en cas de succès, -1 sinon
 *
 */
int ksemP(uint8_t key);

/**
 * @brief Opération V sur un sémaphore
 * Réalise l'opération V sur le sémaphore.
 * Le processus libère le sémaphore. Si la fifo est vide, il incrémente la valeur du sémaphore.
 * Si un processus est présent dans la fifo, il le réveille.
 * 
 * @param semid Identifiant du sémaphore sur lequel l'opération doit être réalisée
 * 
 * @return 0 en cas de succès, -1 sinon
 *
 */
int ksemV(uint8_t key);

#endif //KSEM_H

