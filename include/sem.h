/**
 * @file sem.h
 *
 * @author TacOS developers 
 *
 * @brief l'interface de manipulation des semaphores
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
 * Description de ce que fait le fichier
 */

#ifndef _SEM_H_
#define _SEM_H_

#include <types.h>

/** 
* @brief donne l'identifiant (semid) d'un sémaphore déjà existant
* 
* @param key la clef du sémaphore, cette clef est commune au système
* 
* @return le semid correspondant au sémaphore, -1 si une erreur s'est produite.
*/
int semget(uint8_t key);

/** 
* @brief crée un sémaphore
* 
* @param key la clef du sémaphore
* 
* @return le semid correspondant au sémaphore, -1 si une erreur s'est produite.
*/
int semcreate(uint8_t key);

/** 
* @brief supprime un sémaphore
* 
* @param semid l'identifiant du sémaphore à supprimer.
* 
* @return 
*/
int semdel(uint32_t semid);
int semP(uint32_t semid);
int semV(uint32_t semid);

#endif //_SEM_H_

