#include <fat.h>
#include <floppy.h>
#include <kmalloc.h>
#include <mouse.h>
#include <pci.h>
#include <process.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <apps.h>
#include <sem.h>
#include <gui.h>
#include <memory.h>
#include <video.h>
#include <debug.h>

static int color;

static void handleClick(struct widget_t* wdg, int x __attribute__ ((unused)), int y __attribute__ ((unused)))
{
	color = (color+1)%8;
	setWidgetProperties(wdg, 2, 2, 5, 10+color, color, 6);
}

int gui_task(int argc __attribute__ ((unused)), char* argv[] __attribute__ ((unused)))
{
	color = 0;
	struct window_t* win;
	static struct widget_t* wdg;
	win = createWindow(4, 7);
	wdg = addButton(win, "123");
	setWidgetProperties(wdg, 5, 5, 5, 5, 1, 6);
	setOnClick(wdg, handleClick);

	runWindow(win);

	return 0;
}

int test_gui()
{
	exec(gui_task,"tache_gui" );
	return 0;
}

int semaphore_task(int argc, char** argv __attribute__ ((unused)))
{
	int semid1;
	int semid2;
	int i=0;
	semid1 = semget(1);
	semid2 = semget(argc);
	for(i=0 ; i<5 ; i++)
	{
		semP(semid1);
		printf("ping %d\n",get_pid());
		semV(semid1);
		//getchar();
		int j;
		for(j=0 ; j<10000000 ; j++);
	}

	semV(semid2);
	return 0;
}

int test_fwrite() {
	FILE *file = fopen("fd0:/toto.txt", "w+");

	fwrite("Hello World !", sizeof(char), 13, file);
	fflush(file);
	return 0;
}

int test_semaphores()
{
	int semid1 = semcreate(1); // semaphore alternant les taches
	int semid2 = semcreate(2); // semaphore assurant la fin de la tache 1
	int semid3 = semcreate(3); // semaphore assurant la fin de la tache 2
	semP(semid2);
	semP(semid3);
	create_process("tache_semaphore1", (paddr_t) semaphore_task, 2, NULL, 512, 3);
	create_process("tache_semaphore2", (paddr_t) semaphore_task, 3, NULL, 512, 3);
	while(1);// DEBUG
	semP(semid2);
	semP(semid3);
	semdel(semid1);
	semdel(semid2);
	semdel(semid3);

	return 0;
}

void test_memory_reserve_page_frame() {
	int i;
	scanf("%d", &i);
	while(i--)
		printf("Cadre de page réservé : %d\n", memory_reserve_page_frame());
}

void test_kmalloc()
{
	int *a, *b, *c;
//	kmalloc_print_mem();
//	getchar();

	a = (int *) kmalloc(300*sizeof(int));
	b = (int *) kmalloc(2048*sizeof(int));
	c = (int *) kmalloc(300000*sizeof(int));
//	printf("A : %x\nB : %x\nC : %x\n", a, b, c);
//	kmalloc_print_mem();
//	getchar();

//	printf("REMOVING %x\n", b);
	kfree(b);
//	kmalloc_print_mem();
//	getchar();

//	b = (int *) kmalloc(300*sizeof(int));
//	kmalloc_print_mem();
//	getchar();

	kfree(a);
	kfree(b);
	kfree(c);
//	kmalloc_print_mem();
//	getchar();
}


int pi(int argc __attribute__ ((unused)), char** argv __attribute__ ((unused)))
{
	const double prec = 0.0001;
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

int test_task1(int argc __attribute__ ((unused)), char** argv __attribute__ ((unused)))
{
	int pid = get_pid();
	printf("\nTache n%d\n",pid);
	//printf("Parameters:\nargc=%d\nargv=0x%x\n",argc, argv);
	idle();
	printf("LOL!!\n");
	while(1);
	return 0;
}

int test_task()
{
	exec(test_task1, "test");
	return 0;
}

int calc_pi()
{
	exec(pi, "pi");
	return 0;
}

int test_mouse_task()
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
	printf("\n---- Test Mouse ----\n");

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

int test_mouse()
{
	exec(test_mouse_task,"mouse");
	return 0;
}

int test_scanf()
{
	printf("Entre un mot : \n");
	char b[100];
	scanf("%s", b);
	printf("Tu as tape le mot : %s\n", b);
	return 0;
}

int test_fgets()
{
	printf("Entre une phrase : \n");
	char b[100];
	fgets(b, sizeof(b), stdin);
	printf("Tu as tape la phrase : %s", b);
	return 0;
}

int test_fputs()
{
	fputs("Hello world!\n", stdout);
	return 0;
}

/*
int test_fwrite() 
{
	char chaine[] = "Hello world (fwrite)!\n";
	fwrite(chaine, 2, sizeof(chaine)/2, stdout);
	return 0;
}
*/
int test_sprintf()
{
	char buffer[80];
	sprintf(buffer, "Test : %d\n", 42);
	printf("%s\n", buffer);
	return 0;
}

int test_sscanf()
{
	char * str = "Hello 42 bla";
	char s[10];
	char s2[10];
	int d;

	sscanf(str, "%s %d %s", s, &d, s2);

	printf("%s %d %s\n", s2, d, s);
	return 0;
}	

int debug_fat()
{
	print_path();
	//open("lklk",21);
	return 0;
}

int test_ansi()
{
	printf("\033[2J");
	fflush(stdout);
	printf("\033[8;20H\033[33m=====");
	printf("\033[8;60H\033[33m=====");

	printf("\033[14;41H\033[35m===");
	printf("\033[20;33H\033[37m===================\033[0m\n");
	return 0;
}

