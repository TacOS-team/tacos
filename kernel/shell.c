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

#include <debug.h>

static void test_kmalloc()
{
	int *a, *b, *c;
	kmalloc_print_mem();
	getchar();

	a = (int *) kmalloc(300*sizeof(int));
	b = (int *) kmalloc(2048*sizeof(int));
	c = (int *) kmalloc(300*sizeof(int));
	printf("A : %x\nB : %x\nC : %x\n", a, b, c);
	kmalloc_print_mem();
	getchar();

	printf("REMOVING %x\n", b);
	kfree(b);
	kmalloc_print_mem();
	getchar();

	b = (int *) kmalloc(300*sizeof(int));
	kmalloc_print_mem();
	getchar();

	kfree(a);
	kfree(b);
	kfree(c);
	kmalloc_print_mem();
	getchar();
}


static int pi(int argc __attribute__ ((unused)), char** argv __attribute__ ((unused)))
{
	const double prec = 0.001;
	double x = 0.0, y = 0.0;
	double res;
	int p_in = 0, p_tot = 0;
	for(x=0; x<1.0; x+=prec)
	{
		for(y=0; y<1.0; y+=prec)
		{
			if(x*x+y*y<1.0)
				p_in++;
			
			p_tot++;
		}
	}
	res = 4.0*(double)p_in/(double)p_tot;
	
	printf("Tesultat tache: P_in=%d, P_tot=%d, res=%d\n", p_in, p_tot,(int)(res*1000.0));
	
	return 0;
}

static int test_task1(int argc, char** argv)
{
	int pid = get_pid();
	printf("\nTache n%d\n",pid);
	printf("Parameters:\nargc=%d\nargv=0x%x\n",argc, argv);
	while(1);
	return pid;
}

static int test_task()
{
	create_process("tache test", test_task1, 42, 0xba, 512, 3);
	//exec(test_task1, "test");
	return 0;
}


static int test_mouse_task()
{
	int i = 0;
	int x;
	int y;
	int old_x = 0;
	int old_y = 0;
	int col1 = 2;
	int col2 = 5;
	int btn1_frz = 0;
	int btn2_frz = 0;
	printf("---- Test Mouse ----\n");
	
	while(1)
	{
		if(i%100000 == 0)
		{
			getMouseCoord(&x,&y);
			
			if(getMouseBtn(0))
			{
				if(!btn1_frz) col1 = (col1+1)%0xF;
				btn1_frz = 1;
			}
			else
			{
				btn1_frz = 0;
			}
			
			if(getMouseBtn(1))
			{
				if(!btn2_frz) col2 = (col2+1)%0xF;
				btn2_frz = 1;
			}
			else
			{
				btn2_frz = 0;
			}
			
			x = x*80/640;
			y = 25 - (y*25/480);
			set_attribute_position(0, col2, old_x, old_y);
			set_attribute_position(col1, col2, x, y);
			old_x = x;
			old_y = y;
		}
		i++;
	}
	return 0;
}

static int test_mouse()
{
	create_process("tache mouse", test_mouse_task, 42, 0xba, 512,3);
	return 0;
}

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

static int test_scanf()
{
	printf("Entre un mot : \n");
	char b[100];
	scanf("%s", b);
	printf("Tu as tape le mot : %s\n", b);
	return 0;
}

static int test_fgets()
{
	printf("Entre une phrase : \n");
	char b[100];
	fgets(b, sizeof(b), stdin);
	printf("Tu as tape la phrase : %s", b);
	return 0;
}

static int test_fputs()
{
	fputs("Hello world!\n", stdout);
	return 0;
}

static int test_fwrite() 
{
	char chaine[] = "Hello world (fwrite)!\n";
	fwrite(chaine, 2, sizeof(chaine)/2, stdout);
	return 0;
}

static int test_sprintf()
{
	char buffer[80];
	sprintf(buffer, "Test : %d\n", 42);
	printf("%s\n", buffer);
	return 0;
}

static int test_sscanf()
{
	char * str = "Hello 42 bla";
	char s[10];
	char s2[10];
	int d;

	sscanf(str, "%s %d %s", s, &d, s2);

	printf("%s %d %s\n", s2, d, s);
	return 0;
}	

static int debug_fat()
{
	print_path();
	//open("lklk",21);
	return 0;
}

static int kill_cmd()
{
	int pid;
	scanf("%d",&pid);
	kill(pid);
	return 0;
}


static int test_ansi()
{
	printf("\033[2J");
	fflush(stdout);
	printf("\033[8;20H\033[33m=====");
	printf("\033[8;60H\033[33m=====");

	printf("\033[14;41H\033[35m===");
	printf("\033[20;33H\033[37m===================\033[0m\n");
	return 0;
}

static int cd_cmd()
{
	char buffer[80];
	scanf("%s", buffer);
	change_dir(buffer);
	return 0;
}

static int cls() {
	printf("\033[2J");
	fflush(stdout);
	return 0;
}

int shell(int argc __attribute__ ((unused)), char** argv __attribute__ ((unused)))
{
	char buffer[80];
	
	add_builtin_cmd(help_cmd, "help");
	add_builtin_cmd(date_cmd, "date");
	add_builtin_cmd(cls, "clear");
	add_builtin_cmd((func_ptr)pci_list, "lspci");
	add_builtin_cmd((func_ptr)memory_print, "print_memory");
	add_builtin_cmd(test_mouse, "test_mouse");
	add_builtin_cmd(test_scanf, "test_scanf");
	add_builtin_cmd(test_fgets, "test_fgets");
	add_builtin_cmd(test_fputs, "test_fputs");
	add_builtin_cmd(test_fwrite, "test_fwrite");
	add_builtin_cmd(test_sprintf, "test_sprintf");
	add_builtin_cmd((func_ptr)print_process_list, "ps");
	add_builtin_cmd(test_sscanf, "test_sscanf");
	//add_builtin_cmd(kmalloc_print_mem, "kmalloc_print_mem");
	add_builtin_cmd((func_ptr)test_kmalloc, "test_kmalloc");
	add_builtin_cmd((func_ptr)list_segments, "ls");
	add_builtin_cmd((func_ptr)print_Boot_Sector, "mount");
	add_builtin_cmd((func_ptr)print_working_dir, "pwd");
	add_builtin_cmd(debug_fat, "debugfat");
	add_builtin_cmd((func_ptr)clean_process_list, "clean_proclist");
	add_builtin_cmd(kill_cmd, "kill");
	add_builtin_cmd(test_ansi, "test_ansi");
	add_builtin_cmd(cd_cmd, "cd");
	add_builtin_cmd(test_task, "test_task");
		
	for(;;)
	{
		//time_t curr_time = time(NULL);
		printf("\n > ");
		
		fflush(stdout);
		fflush(stdin);
		
		scanf("%s", buffer);
		printf("\n");
		if(exec_builtin_cmd(buffer) != 0)
			printf("Commande introuvable.\n"); 
		// Si on ne trouve pas la commande en builtin, on devrait alors chercher si y'a un executable qui correspond, dans un futur proche j'espere :p
	}
}
