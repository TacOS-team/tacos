
/**
 * jobs.c
 * Description:
 *
 * Auteurs: Alexandre, Guillaume, Mathieu, Léo
 */

#define _POSIX_SOURCE 1

#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <errno.h>

#include "command.h"
#include "customs.h"
#include "jobs.h"
#include "shell.h"

/* On initialise la liste des jobs */
job *first_job_list = NULL;

void create_process(char **command[], int nb_commandes, char *line, int infile, int outfile, int background)
{
	int i;
	process **tab_process = cmalloc(sizeof(process*) * (nb_commandes + 1));

	process *p = cmalloc(sizeof(process));
	job *j = cmalloc(sizeof(job));

	if (first_job_list == NULL) {
		first_job_list = j;
	} else {
		get_last_job()->next = j;
	}

	tab_process[0] = p;

	j->next = NULL;
	j->command = cmalloc(sizeof(char) * LENGTH_LINE + 1);
	strncpy(j->command,line,strlen(line) + 1);
	j->first_process = tab_process[0];
	j->pgid = 0;

	j->notified = 0;
	j->tmodes = shell.shell_tmodes;
	j->stdin = (infile) ? infile : STDIN_FILENO;
	j->stdout = (outfile) ? outfile : STDOUT_FILENO;
	j->stderr = STDERR_FILENO;

	for (i = 1; i < nb_commandes; ++i) {
		tab_process[i] = cmalloc(sizeof(process));
	}
	tab_process[nb_commandes] = NULL;

	for (i = 0; i < nb_commandes; ++i) {
		tab_process[i]->next = tab_process[i + 1];
		tab_process[i]->argv = command[i];
		tab_process[i]->pid = 0;
		tab_process[i]->completed = 0;
		tab_process[i]->stopped = 0;
		tab_process[i]->status = 0;
	}

	if (background)
		launch_job(j, 0);
	else
		launch_job(j, 1);
}

void launch_process(process *p, pid_t pgid, int infile, int outfile, int errfile)
{
	pid_t pid;

	pid = getpid();

	if (pgid == 0) {
		pgid = pid;
	}


#ifdef FONCTIONS_AVANCEES
	setpgid(pid, pgid);
	tcsetpgrp(shell.shell_terminal, pgid);
#endif

	signal(SIGINT, SIG_DFL);
	signal(SIGQUIT, SIG_DFL);
	signal(SIGTSTP, SIG_DFL);
	signal(SIGTTIN, SIG_DFL);
	signal(SIGTTOU, SIG_DFL);
	signal(SIGCHLD, SIG_DFL);

	if (infile != STDIN_FILENO) {
		dup2(infile, STDIN_FILENO);
		close(infile);
	}
	if (outfile != STDOUT_FILENO) {
		dup2(outfile, STDOUT_FILENO);
		close(outfile);
	}
	if (errfile != STDERR_FILENO) {
		dup2(errfile, STDERR_FILENO);
		close(errfile);
	}

	if (execv(p->argv[0], p->argv) < 0)
	    printf("Commande inconnue\n");

}

void launch_job(job *j, int foreground)
{
	process *p;
	pid_t pid;
	int mypipe[2], infile, outfile;

	infile = j->stdin;
	for (p = j->first_process; p; p = p->next) {
		if (p->next) {
#ifdef FONCTIONS_AVANCEES
			if (pipe(mypipe) < 0) {
				perror("pipe");
				exit(1);
			}
			outfile = mypipe[1];
#endif
		} else {
			outfile = j->stdout;
		}

		launch_process(p, j->pgid, infile, outfile, j->stderr);

		/* Processus père */
		p->pid = pid;
		if (!j->pgid) {
			j->pgid = pid;
		}

		if (infile != j->stdin)
			close(infile);
		if (outfile != j->stdout)
			close(outfile);
		infile = mypipe[0];
	}

	if (foreground)
		put_job_foreground(j, 0);
	else
		put_job_background(j, 0);
}

void format_job_info(job *j, const char *status)
{
	fprintf(stderr, "%ld (%s): %s\n", (long) j->pgid, status, j->command);
}

process *find_process(pid_t pid)
{
	job *j;
	process *p;
	if (pid > 0) {
		for (j = first_job_list; j; j = j->next)
			for (p = j->first_process; p; p = p->next) {
				/* printf("process %s trouvé, pids %d(arg)/%d :\n",p->argv[0],pid,p->pid); */
				if (p->pid == pid) {
					return p;
				}
			}
		fprintf(stderr, "No child process %d.\n", pid);
		return NULL;
	}
	return NULL;
}

void wait_job(int block)
{
	int status;
	process *p;
	int pid;

#ifdef FONCTIONS_AVANCEES
	if (block) {
		/* Attend tous processus fils, sauvegarde leur status (fonction bloquante) */
		pid = waitpid(-1, &status, WUNTRACED);
	} else {
		pid = waitpid(-1, &status, WUNTRACED|WNOHANG);
	}
#endif
	//waitpid(-1);

	if (pid > 0) {
		if ((p = find_process(pid)) != NULL) {
			p->status = status;
#ifdef FONCTIONS_AVANCEES
			if (WIFSTOPPED(status)) {
				p->stopped = 1;
				printf("processus stoppé\n");
			} else {
				p->completed = 1;
				if (WIFSIGNALED(status)) {
					printf("terminaison par signal %i\n", WTERMSIG(status));
				}
			}
#endif
		} else {
		}
#ifdef FONCTIONS_AVANCEES
	} else if (pid == 0 || errno == ECHILD) {
#ifdef DEBUG
		printf("pas de processus trouvé");
#endif
#endif
	}
}


void continue_job(job *j, int foreground)
{
	if ( j == NULL) {
		printf("tâche inexistante\n");
	} else {
		mark_job_as_running(j);
		if (foreground)
			put_job_foreground(j, 1);
		else
			put_job_background (j, 1);
	}
}

void mark_job_as_running(job *j)
{
	process *p;
	for (p = j->first_process; p; p = p->next)
		p->stopped = 0;
	j->notified = 0;
}

job *get_last_job()
{
	job *j = first_job_list;
	if (j != NULL){
		while (j->next != NULL) {
			j = j->next;
		}
	} else {
		j = NULL;
	}
	return j;
}

job *get_job_by_id(int id) {
	job *j = first_job_list;
	if (j != NULL){
		while (j->next != NULL && (id-- != 0)) {
			j = j->next;
		}
	} else {
		j = NULL;
	}
	return j;
}


int job_is_finished(job *j)
{
	process *p;
	for (p = j->first_process; p; p = p->next) {
		if (!p->completed)
			return 0;
	}
	return 1;
}

int job_is_stopped(job *j)
{

	process *p;
	for (p = j->first_process; p; p = p->next) {
		if (!p->completed && !p->stopped)
			return 0;
	}
	return 1;
}

void put_job_foreground(job *j, int cont)
{
	if (cont) {
#ifdef FONCTIONS_AVANCEES
		tcsetpgrp(shell.shell_terminal, j->pgid);
		tcsetattr(shell.shell_terminal, TCSADRAIN, &j->tmodes);
#endif

		if (kill(-j->pgid, SIGCONT) < 0)
			perror("kill (SIGCONT)");
	}

	do {
		wait_job(1);

	} while (!job_is_finished(j) && !job_is_stopped(j));

	if (job_is_finished(j)) {
		/* TODO: free tous les process du job (déclarer une fonction) */
		free_processes_job(j->pgid);
		/* TODO: free du job (raccordement de la liste chainée de jobs - déclarer une fonction) */
		free_job(j->pgid);
	}
	
#ifdef FONCTIONS_AVANCEES
	/* Redonne la main au shell */
	tcsetpgrp(shell.shell_terminal, shell.shell_pgid);
#endif

	//tcgetattr(shell.shell_terminal, &j->tmodes);
	//tcsetattr(shell.shell_terminal, TCSADRAIN, &shell.shell_tmodes);
}


void put_job_background (job *j, int cont)
{
	if (cont)
		if (kill (-j->pgid, SIGCONT) < 0)
			perror ("kill (SIGCONT)");

#ifdef FONCTIONS_AVANCEES
	while (!cont && tcgetpgrp(shell.shell_terminal) == shell.shell_pgid);
	tcsetpgrp(shell.shell_terminal, shell.shell_pgid);

	tcgetattr(shell.shell_terminal, &j->tmodes);
	tcsetattr(shell.shell_terminal, TCSADRAIN, &shell.shell_tmodes);
#endif
}

void sigchld_handler_notif()
{
	job *j, *jnext;

	wait_job(0);

	for (j = first_job_list; j; j = jnext)
	{
		jnext = j->next;

		if (job_is_finished(j)) {
			format_job_info (j, "completed");

			free_processes_job(j->pgid);
			free_job (j->pgid);
		}

		else if (job_is_stopped (j) && !j->notified) {
			format_job_info (j, "stopped");
			j->notified = 1;
		}
	}
}

void free_job(pid_t pgid)
{
	job *j = first_job_list;
	job *j1 = NULL;
	while (j != NULL && j->pgid != pgid) {
		j1 = j;
		j = j->next;
	}
	if (j != NULL) {
		if (j == first_job_list) {
			first_job_list = j->next;
			cfree(j->command);
			cfree(j);
		} else {
			j1->next = j->next;
			cfree(j->command);
			cfree(j);
		}
	}
}

void free_processes_job(pid_t pgid)
{
	job *j = first_job_list;
	process *p, *p1;
	while (j != NULL && j->pgid != pgid) {
		j = j->next;
	}
	p = j->first_process;
	while (p != NULL) {
		p1 = p;
		p = p->next;
		cfree(p1);
	}
}

void print_jobs_pid()
{
	job *j = first_job_list;
	printf("jobs_list :");
	while (j != NULL) {
		printf(" -> %d", j->pgid);
		j = j->next;
	}
	printf("\n");
}

void print_processes_pid(pid_t pgid)
{
	job *j = first_job_list;
	process *p = NULL;
	while (j != NULL && j->pgid != pgid) {
		j = j->next;
	}
	p = j->first_process;
	printf("job %d - processes list :", j->pgid);
	while (p != NULL) {
		printf(" -> %d", p->pid);
		p = p->next;
	}
	printf("\r\n");
}

void print_jobs() {
	job *j = first_job_list;
	int i = 0;
	if (!j) printf("\tAucun job trouvé \n");

	while (j != NULL) {
		printf("\t Jobs [%d] : %s %s(%d) \n",i,(job_is_stopped(j)) ? "Stoppé" : "En cours",j->command,j->pgid);
		i++;
		j = j->next;
	}

}
