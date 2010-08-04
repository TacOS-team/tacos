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
#include <video.h>
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
	char buffer[80];
	scanf("%s", buffer);
	//printf("%sEND\n",buffer);
	catenate(buffer);
	return 0;
}

static int cat_demo_cmd()
{
	char buffer[80];
	scanf("%s", buffer);
	//printf("%sEND\n",buffer);
	catenate_demo(buffer);
	return 0;
}

static int resize_cmd()
{
	int x, y;
	scanf("%d %d", &x, &y);
	resize_text_window(get_current_process()->fd[1].ofd->extra_data, x, y);
  return 0;
}

static int resize_cmd_pid() {
	int pid, x, y;
  process_t *p;
	
  scanf("%d %d %d", &pid, &x, &y);
  p = find_process(pid);

  if(p != NULL) {
  	resize_text_window(p->fd[1].ofd->extra_data, x, y);
    return 0;
  }

  return 1;
}

static int move_pid() {
	int pid, x, y;
  process_t *p;

	scanf("%d %d %d", &pid, &x, &y);
  p = find_process(pid);

  if(p != NULL) {
  	move_text_window(p->fd[1].ofd->extra_data, x, y);
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
	exec((paddr_t)shell, "Mishell"); // XXX: exec est défini dans le kernel...
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

int top(int argc __attribute__ ((unused)), char** argv __attribute__ ((unused)))
{
	disable_cursor(1);
	while(1)
	{
		printf("\033[1;1H");
		print_process_list();
		sleep(1);
	}
}
static int exec_top()
{
		exec((paddr_t)top,"top");
		return 0;
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
	add_builtin_cmd(test_sprintf, "test_sprintf");
	add_builtin_cmd((func_ptr)print_process_list, "ps");
	/*add_builtin_cmd(test_sscanf, "test_sscanf");*/
	//add_builtin_cmd(kmalloc_print_mem, "kmalloc_print_mem");
	/*add_builtin_cmd((func_ptr)test_kmalloc, "test_kmalloc");*/
	/*add_builtin_cmd((func_ptr)test_memory_reserve_page_frame, "test_reserve_frame");*/
	add_builtin_cmd(ls_cmd, "ls");
	add_builtin_cmd(ll_cmd, "ll");
	add_builtin_cmd((func_ptr)print_Boot_Sector, "mount");
	add_builtin_cmd((func_ptr)print_working_dir, "pwd");
	add_builtin_cmd(debug_fat, "debugfat");
	add_builtin_cmd((func_ptr)clean_process_list, "clean_proclist");
	add_builtin_cmd(kill_cmd, "kill");
	add_builtin_cmd(test_ansi, "test_ansi");
	add_builtin_cmd(cd_cmd, "cd");
	add_builtin_cmd(cat_cmd, "cat");
	add_builtin_cmd(cat_demo_cmd, "catdemo");
	add_builtin_cmd(test_task, "test_task");
	add_builtin_cmd(test_semaphores, "test_sem");
	add_builtin_cmd(calc_pi, "pi");
	add_builtin_cmd(exec_shell, "Mishell");
	add_builtin_cmd(main_fiinou, "Fiinou");
	add_builtin_cmd(launch_pres, "presentation");
	add_builtin_cmd(launch_matrix, "matrix");
	add_builtin_cmd(resize_cmd, "resize");
	add_builtin_cmd(resize_cmd_pid, "resize_pid");
	add_builtin_cmd(move_pid, "move_pid");
	add_builtin_cmd(snake_main, "snake");
	add_builtin_cmd(cube_launcher, "cube");
	add_builtin_cmd(noxeyes, "noxeyes");
	add_builtin_cmd(sleep_shell, "sleep");
	add_builtin_cmd(exec_top, "top");
	add_builtin_cmd(test_write_serial, "write_serial");
	add_builtin_cmd(test_read_serial, "read_serial");
	add_builtin_cmd(test_elf, "test_elf");
    add_builtin_cmd(test_ctype, "test_ctype");

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
