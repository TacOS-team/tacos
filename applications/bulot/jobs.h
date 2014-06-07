/**
 * \file jobs.h
 * \brief Gestion des jobs et des processus
 * \author Bul0t Team
 * \date mai 2012
 */

#ifndef JOBS_H_
#define JOBS_H_
#include "shell.h"

extern job *first_job_list;


/**
 * \fn void create_process(char ** command[], int nb_commandes, char* line, int infile, int outfile, int background)
 * \brief Initialise la structure liée à un job et à un (des) processus
 */
void create_process(char ** command[], int nb_commandes, char* line, int infile, int outfile, int background);

/**
 * \fn void launch_process(process *p, pid_t pgid, int infile, int outfile, int errfile)
 * \brief Permet de configurer et d'exécuter de nouveaux processus
 * \param p processus à exécuter
 * \param pgid pgid du processus p
 * \param infile fichier d'entrée du processus p
 * \param outfile fichier de sortie du processus p
 * \param errfile fichier d'erreur du processus p
 */
void launch_process(process *p, pid_t pgid, int infile, int outfile, int errfile);

/**
 * \fn void launch_job(job *j, int foreground)
 * \brief Permet de configurer des jobs et appelle le lancement de nouveaux processus
 * \param j job à lancer
 * \param foreground booléen indiquant si le job sera en foreground (1) ou background (0)
 */
void launch_job(job *j, int foreground);

/**
 * \fn void format_job_info(job *j, const char *status)
 * \brief Affiche des informations relatives à un job sur la sortie d'erreur
 * \param j job
 * \param status
 */
void format_job_info(job *j, const char *status);

/**
 * \fn process* find_process(pid_t pid)
 * \brief Cherche un processus par son PID
 * \param pid PID
 */
process* find_process(pid_t pid);

/**
 * \fn void wait_job(int block)
 * \param block
 */
void wait_job(int block, int pid);

/**
 * \fn void continue_job(job *j, int foreground)
 * \param j job
 * \param foreground
 */
void continue_job(job *j, int foreground);

/**
 * \fn void mark_job_as_running(job *j)
 * \param j job
 */
void mark_job_as_running(job *j);

/**
 * \fn job *get_last_job()
 * \brief Renvoie le dernier élément de la liste chaînée de jobs
 */
job *get_last_job();

/**
 * \fn job *get_job_by_id(int id)
 * \param id
 * \brief Renvoie l'element de la liste de jobs correspondant à l'id
 */
job *get_job_by_id(int id);

/**
 * \fn int job_is_finished(job *j)
 * \param j job
 */
int job_is_finished(job *j);

/**
 * \fn int job_is_stopped(job *j)
 * \param j job
 */
int job_is_stopped(job *j);

/**
 * \fn void put_job_foreground(job *j, int cont)
 * \param j
 * \param cont
 */
void put_job_foreground(job *j, int cont);

/**
 * \fn void put_job_background (job *j, int cont)
 * \param j
 * \param cont
 */
void put_job_background (job *j, int cont);

/**
 * \fn void sigchld_handler_notif()
 */
void sigchld_handler_notif();

/**
 * \fn void free_job(pid_t pgid)
 * \param: pgid
 */
void free_job(pid_t pgid);

/**
 * \fn void free_processes_job(pid_t pgid)
 * \param: pgid
 */
void free_processes_job(pid_t pgid);


/**
 * \fn void print_jobs_pid()
 * \brief Affiche les pgid des jobs sous la forme : jobs list : -> pgid1 -> pgid2 ...
 */
void print_jobs_pid();

/**
 * \fn void print_processes_pid(pid_t pgid)
 * \brief Affiche les pgid des processus du job pgid sous la forme : job pgid - processes list : pid1 -> pid2 ...
 * \param pgid
 */
void print_processes_pid(pid_t pgid);

/**
 * \fn void print_jobs()
 */
void print_jobs();

#endif /* JOBS_H_ */
