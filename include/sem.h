#ifndef _SEM_H_
#define _SEM_H_

/** 
* @file sem.h 
* @brief l'interface de manipulation des semaphores
*/

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

