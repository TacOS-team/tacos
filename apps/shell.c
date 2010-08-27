#include <fat.h>
#include <floppy.h>
#include <kmalloc.h>
#include <memory.h>
#include <mouse.h>
#include <pci.h>
#include <process.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <shell_utils.h>
#include <apps.h>
#include <sem.h>
#include <debug.h>
#include <serial.h>
#include <unistd.h>

int shell(int argc __attribute__ ((unused)), char** argv __attribute__ ((unused)));

static int help_cmd()
{
	show_builtin_cmd();
	return 0;
}

static int date_cmd()
{
	time_t curr_time = time(NULL);
	printf("%s",ctime(&curr_time));	
	return 0;
}

static int kill_cmd()
{
	int pid;
	scanf("%d",&pid);
	kill(pid);
	return 0;
}

static int ls_cmd()
{
	list_segments(0);
	return 0;
}

static int ll_cmd()
{
	list_segments(1);
	return 0;
}

static int cd_cmd()
{
	char buffer[80];
	scanf("%s", buffer);
	//printf("%sEND\n",buffer);
	change_dir(buffer);
	return 0;
}

static int cat_cmd()
{
// NON TESTÉ !
	char buf[80];
    char chemin[80];
    strcpy(chemin, "fd0:/");
	scanf("%s", buf);
    strcat(chemin, buf);

	FILE *file = fopen(chemin, "r");

	char buffer[100];
	while (fread(buffer, sizeof(buffer), sizeof(char), file) == 100) {
        buffer[99] = '\0';
        printf("%s", buffer);
    }

	fflush(file);

	return 0;
}

static int resize_cmd()
{
	int x, y;
	scanf("%d %d", &x, &y);
	// XXX: resize_text_window(get_process(CURRENT_PROCESS)->fd[1].ofd->extra_data, x, y);
  return 0;
}

static int resize_cmd_pid() {
	int pid, x, y;
  process_t *p;
	
  scanf("%d %d %d", &pid, &x, &y);
  p = get_process(pid);

  if(p != NULL) {
  	// XXX:resize_text_window(p->fd[1].ofd->extra_data, x, y);
    return 0;
  }

  return 1;
}

static int move_pid() {
	int pid, x, y;
  process_t *p;

	scanf("%d %d %d", &pid, &x, &y);
  p = get_process(pid);

  if(p != NULL) {
	//XXX : move_text_window(p->fd[1].ofd->extra_data, x, y);
    return 0;
  }

  return 1;
}

static int cls_cmd() {
	printf("\033[2J");
	fflush(stdout);
	return 0;
}

static int exec_shell()
{
	exec((paddr_t)shell, "Mishell");
	return 0;
}

static int sleep_shell()
{
	int time = 0;
	scanf("%d", &time);
	printf("Asleep for %d ms...\n", time);
	usleep(time);
	return 0;
}

int ps()
{
	process_t* aux = get_process_list(FIRST_PROCESS);

	const int clk_per_ms = CLOCKS_PER_SEC / 1000;
	long int ms;
	int s;
	int m;
	int h;

	printf("pid\tname\t\ttime\t\t%CPU\tstate\n");   
	while(aux!=NULL)
	{
			
			/* Calcul du temps d'execution du processus */
			ms = aux->user_time / clk_per_ms;
			s = ms / 1000;
			
			m = s / 60;
			s = s % 60;
			h = m / 60;
			m = m % 60;
			
			/*if (aux->process == active_process) {
					printf("*");
			}*/
			
			printf("%d\t%s\t\t%dh %dm %ds\t%d\%\t",aux->pid, aux->name, h, m ,s, (int)(((float)aux->last_sample/(float)CPU_USAGE_SAMPLE_RATE)*100.f));
			
			switch(aux->state)
			{
					case PROCSTATE_IDLE:
							printf("IDLE\n");
							break;
					case PROCSTATE_RUNNING:
							printf("RUNNING\n");
							break;
					case PROCSTATE_WAITING:
							printf("WAITING\n");
							break;
					case PROCSTATE_TERMINATED:
							printf("TERMINATED\n");
							break;
					default:
							break;
			}
			
			aux = get_process_list(NEXT_PROCESS);
	}
	return 0;
}


static int shell_exec_elf()
{
	char filename[80];
	scanf("%s",filename);
	exec_elf(filename);
}

void print_logo()
{
	printf("_|_|_|_|_|                      _|_|      _|_|_|\n");
	printf("    _|      _|_|_|    _|_|_|  _|    _|  _|      \n");
	printf("    _|    _|    _|  _|        _|    _|    _|_|  \n");
	printf("    _|    _|    _|  _|        _|    _|        _|\n");
	printf("    _|      _|_|_|    _|_|_|    _|_|    _|_|_|    ");
	printf("(codename:fajitas)\n\n\n");
}

void print_logo_serial()
{
	serial_puts(COM1, "_|_|_|_|_|                      _|_|      _|_|_|\n");
	serial_puts(COM1, "    _|      _|_|_|    _|_|_|  _|    _|  _|      \n");
	serial_puts(COM1, "    _|    _|    _|  _|        _|    _|    _|_|  \n");
	serial_puts(COM1, "    _|    _|    _|  _|        _|    _|        _|\n");
	serial_puts(COM1, "    _|      _|_|_|    _|_|_|    _|_|    _|_|_|    ");
	serial_puts(COM1, "(codename:fajitas)\n\n\n");
}

int shell(int argc __attribute__ ((unused)), char** argv __attribute__ ((unused)))
{
	process_t *self = get_process(get_pid());
	
	init_stdfiles(&self->stdin, &self->stdout, &self->stderr);
	
	char buffer[80];

	add_builtin_cmd(help_cmd, "help");
	add_builtin_cmd(date_cmd, "date");
	add_builtin_cmd(cls_cmd, "clear");
	add_builtin_cmd((func_ptr)pci_list, "lspci");
	//add_builtin_cmd((func_ptr)memory_print, "print_memory");
	add_builtin_cmd(test_mouse, "test_mouse");
	add_builtin_cmd(test_scanf, "test_scanf");
	add_builtin_cmd(test_fgets, "test_fgets");
	add_builtin_cmd(test_fputs, "test_fputs");
	add_builtin_cmd(test_fwrite, "test_fwrite");
	add_builtin_cmd(test_fread, "test_fread");
	add_builtin_cmd(test_fseek, "test_fseek");
	add_builtin_cmd(ps, "ps");
	//add_builtin_cmd(kmalloc_print_mem, "kmalloc_print_mem");
	/*add_builtin_cmd((func_ptr)test_memory_reserve_page_frame, "test_reserve_frame");*/
	add_builtin_cmd(ls_cmd, "ls");
	add_builtin_cmd(ll_cmd, "ll");
	add_builtin_cmd((func_ptr)print_Boot_Sector, "mount");
	add_builtin_cmd((func_ptr)print_working_dir, "pwd");
	/*add_builtin_cmd((func_ptr)clean_process_list, "clean_proclist");*/
	add_builtin_cmd(kill_cmd, "kill");
	add_builtin_cmd(test_ansi, "test_ansi");
	add_builtin_cmd(cd_cmd, "cd");
	add_builtin_cmd(cat_cmd, "cat");
	add_builtin_cmd(test_task, "test_task");
	add_builtin_cmd(test_semaphores, "test_sem");
	add_builtin_cmd(calc_pi, "pi");
	add_builtin_cmd(exec_shell, "Mishell");
	add_builtin_cmd(main_fiinou, "Fiinou");
	add_builtin_cmd(launch_pres, "presentation");
	add_builtin_cmd(resize_cmd, "resize");
	add_builtin_cmd(resize_cmd_pid, "resize_pid");
	add_builtin_cmd(move_pid, "move_pid");
	add_builtin_cmd(snake_main, "snake");
	add_builtin_cmd(noxeyes, "noxeyes");
	add_builtin_cmd(sleep_shell, "sleep");
	add_builtin_cmd(test_write_serial, "write_serial");
	add_builtin_cmd(test_read_serial, "read_serial");
	add_builtin_cmd(test_elf, "elf_info");
	add_builtin_cmd(test_ctype, "test_ctype");
	add_builtin_cmd(test_stdio, "test_stdio");
	add_builtin_cmd(shell_exec_elf, "exec");

	disable_cursor(0);
	
	print_logo();
	print_logo_serial();

	for(;;)
	{
		//time_t curr_time = time(NULL);
		printf("\n");
		print_working_dir();
		printf(">");
		
		fflush(stdout);
		fflush(stdin);
		
		scanf("%s", buffer);
		//printf("\n");
		if(exec_builtin_cmd(buffer) != 0)
			printf("commande introuvable.\n"); 
		// Si on ne trouve pas la commande en builtin, on devrait alors chercher si y'a un executable qui correspond, dans un futur proche j'espere :p
	}
}
