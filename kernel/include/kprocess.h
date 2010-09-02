#ifndef _KPROCESS_H_
#define _KPROCESS_H_


#include <process.h> // TODO: Dans process.h ya pleins de trucs qui devraient probablement migrer dans kprocess.h...
#include <types.h>
#include <ksyscall.h>
/** 
* @brief (OUTDATED)Crée un nouveau processus.
* Crée un nouveau processus et l'ajoute à la liste des processus avec un état d'exécution PROCSTATE_IDLE.
* @param init_data informations sur le processus à créer
* @see process_init_data_t
* 
* @return Pid du processus créé.
*/
process_t* create_process(process_init_data_t* init_data);

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

SYSCALL_HANDLER1(sys_exit,uint32_t ret_value __attribute__ ((unused)));
SYSCALL_HANDLER1(sys_getpid, uint32_t* pid);
SYSCALL_HANDLER1(sys_exec, process_init_data_t* init_data);
SYSCALL_HANDLER3(sys_proc, uint32_t sub_func, uint32_t param1, uint32_t param2);

void add_process(process_t* process);

void inject_idle(process_t* proc);
#endif /* _K_PROCESS_H_ */
