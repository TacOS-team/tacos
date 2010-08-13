#ifndef _KPROCESS_H_
#define _KPROCESS_H_

#include <process.h> // TODO: Dans process.h ya pleins de trucs qui devraient probablement migrer dans kprocess.h...
#include <types.h>

/** 
* @brief (OUTDATED)Crée un nouveau processus.
* Crée un nouveau processus et l'ajoute à la liste des processus avec un état d'exécution PROCSTATE_IDLE.
* @param name Nom du processus.
* @param prog Pointeur vers le point d'entrée du processus.
* @param argc Nombre d'arguments passés au processus.
* @param argv Tableau contenant les arguments à passer au processus.
* @param stack_size Taille de la pile utilisateur du processus.
* @param ring Niveau d'exécution du processus, toujours à 3.
* 
* @return Pid du processus créé.
*/
int create_process(char* name, paddr_t prog, char* param, uint32_t stack_size, uint8_t ring __attribute__ ((unused)));

/** 
* @brief Retire un processus de la liste.
* 
* @param pid Pid du processus à retirer de la liste.
* 
* @return 0 la plupart du temps.
*/
int delete_process(int pid);

/** 
* @brief Cherche le process_t* en correspondant à un pid donné.
*
* @param pid Pid du processus à chercher.
* 
* @return process_t* correspondant au pid.
*/
process_t* find_process(int pid);

/** 
* @brief Retourne le processus en cours d'utilisation
* 
* @return processus en cours d'utilisation.
*/
process_t* get_current_process();

/** 
* @brief Retourne le processus suivant le processus courant dans la liste
* 
* @return processus suivant.
*/
process_t* get_next_process();

/** 
* @brief Retourne le nombre de processus dans la liste.
* 
* @return nombre de processus dans la liste.
*/
uint32_t get_proc_count();

/** 
* @brief Nettoie la liste des processus.
* Retire de la liste tous les processus en état PROCSTATE_TERMINATED.
*/
void clean_process_list();

void sample_CPU_usage();

/** 
* @brief Donne le focus au prochain processus actif dans la liste.
*/
void change_active_process();
process_t* get_active_process();

/** 
* @brief Coté kernel de l'appel système kill.
* Change le statut du processus appelant à PROCSTATE_TERMINATED.
* @param ret_value ???
* @param zero1 Inutilisé
* @param zero2 Inutilisé
* 
* @return 
*/
void sys_exit(uint32_t ret_value, uint32_t zero1, uint32_t zero2);
void sys_getpid(uint32_t* pid, uint32_t zero1, uint32_t zero2);
void sys_kill(uint32_t pid, uint32_t zero1, uint32_t zero2);
void sys_proc(uint32_t sub_func, uint32_t param1, uint32_t param2);


#endif /* _K_PROCESS_H_ */
