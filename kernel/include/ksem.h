#ifndef KSEM_H
#define KSEM_H

/**
 * @file ksem.h
 * Coté noyau de la gestion des sémaphores
 * 
 */

#include <types.h>

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
void sys_ksem(uint32_t op, uint32_t param, uint32_t ret);

/**
 * @brief Obtenir un semid
 * ksemget retourn un semid propre au processus, associé au sémaphore désigné par key
 * 
 * @param key clé identifiant le sémaphore
 * 
 * @return semid associé au sémaphore identifié par key
 * -1 en cas d'erreur
 */
int ksemget(uint8_t key);

/**
 * @brief Créer un sémaphore
 * Crée le sémaphore associé à la clé key, et retourne un semid à la manière de ksemget
 * 
 * @param key clé identifiant le sémaphore
 * 
 * @return semid associé au sémaphore identifié par key
 * -1 en cas d'erreur
 */
int ksemcreate(uint8_t key);

/**
 * @brief Créer un sémaphore sans définir la clef.
 * Crée le sémaphore associé à la clé key, et retourne un semid à la manière de ksemget
 * 
 * @return semid associé au sémaphore identifié par key
 * -1 en cas d'erreur
 */
int ksemcreate_without_key();

/**
 * @brief Supprime un sémaphore
 * Supprime le sémaphore associé à semid (TODO: et libère les processus en attente)
 * 
 * @param semid Identifiant du sémaphore
 * 
 * @return 0 en cas de succès, -1 sinon
 */
int ksemdel(uint32_t semid);

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
int ksemP(uint32_t semid);

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
int ksemV(uint32_t semid);

#endif //KSEM_H

