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
#include <unistd.h>
#include <list.h>
#include <serial.h>
#include <elf.h>
#include <ctype.h>

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
	exec((paddr_t)gui_task,"tache_gui" );
	return 0;
}

int test_fwrite() {
	FILE *file = fopen("fd0:/system/toto.txt", "w+");

	fwrite("Hello World !", sizeof(char), 13, file);
	fflush(file);
	return 0;
}

int test_fread() {
	FILE *file = fopen("fd0:/doc.txt", "r");

	char buffer[100];
	fread(buffer, sizeof(buffer), sizeof(char), file);

	buffer[99] = '\0';
	printf("%s", buffer);

	fflush(file);
	return 0;
}


int sem_t2(int argc __attribute__ ((unused)), char** argv __attribute__ ((unused)))
{
	int semid1 = semget(4);
	int semid2 = semget(42);
	int i;
	while(1) {
			semP(semid1);
			for(i=0; i<0x005FFFFE; i++){}
			printf("T2:Pong!\n");
			semV(semid2);
			
	}

	return 0;
}

int sem_t1(int argc __attribute__ ((unused)), char** argv __attribute__ ((unused)))
{
	int semid1 = semget(4);
	int semid2 = semget(42);
	unsigned int i = 0;

	while(1){
		semP(semid2);
		for(i=0; i<0x005FFFFE; i++){}
		printf("T1:Ping?\n");
		semV(semid1);
	}
	return 0;
}

int test_semaphores()
{
	/* Test des sémaphores: Deux taches effectuant un calcul (ici un boucle) s'alterne
	 * Pour se convaincre de l'efficacité, on peut utilise la commande "ps" pour constater
	 * que les deux tachent deviennent bien inactives (et donc ne consomment pas de CPU) l'une 
	 * après l'autre
	 */
	int semid1 = semcreate(4); 
	int semid2;
	
	semid2 = semcreate(42);
	semP(semid1);

	exec((paddr_t)sem_t1,"ping");
	exec((paddr_t)sem_t2,"pong");
	
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
	kmalloc_print_mem();
	getchar();

	a = (int *) kmalloc(300*sizeof(int));
	b = (int *) kmalloc(2048*sizeof(int));
	c = (int *) kmalloc(300000*sizeof(int));
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

int test_task1(int argc , char** argv __attribute__ ((unused)))
{
	//while(1);
	int pid = get_pid();
	int i;
	printf("pid %d received %d args:\n", pid, argc);	
	for(i=0; i<argc; i++)
	{
		printf("#%d - %s\n",i, argv[i]);
	}
	return 0;
}

int test_task()
{

	create_process("test",(paddr_t)test_task1,"il est plus d'une heure du matin et je suis fatigué",0x1000,3);

	return 0;
}

int calc_pi()
{
	exec((paddr_t)pi, "pi");
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
	while(1);
	return 0;
}

int test_mouse()
{
	exec((paddr_t)test_mouse_task,"mouse");
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
	sprintf(buffer, "Test : %d", 42);
    //sprintf(buffer, "%s", buffer); // Déconne ;)
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

int proc_write_serial(int argc __attribute__ ((unused)), char** argv __attribute__ ((unused)))
{
	char buffer[80];
	printf(">");
	while(1)
	{
		
		fflush(stdout);
		scanf("%s",buffer);
	
		serial_puts(COM1, buffer);
		serial_putc(COM1, ' ');
	}
	return 0;
}

int test_write_serial()
{
	exec((paddr_t)proc_write_serial, "wserial");
	return 0;
}

int proc_read_serial(int argc __attribute__ ((unused)), char** argv __attribute__ ((unused)))
{
	char buffer[64];
	
	while(1)
	{
		serial_gets(COM1, buffer, 64);
		printf("%s",buffer);
		fflush(stdout);
	}
}

void tu_ctype(const char * nom_test, int (*p)(), const char * resultat_attendu) {
    char buf[256];
    buf[0] = '\0';
    int len = 0;
    unsigned char c;
    for (c = 0; c < 255; c++) {
        if (p(c)) {
            buf[len++] = c;
        }
    }
    buf[len] = '\0';
    if (strcmp(buf, resultat_attendu) == 0) {
        printf("%s [OK]\n", nom_test);
    } else {
        printf("%s [failed] :(\n", nom_test);
        printf("Attendu : %s\n Obtenu : %s\n", resultat_attendu, buf);
    }
}

int test_ctype()
{
    tu_ctype("isupper", isupper, "ABCDEFGHIJKLMNOPQRSTUVWXYZ");    
    tu_ctype("islower", islower, "abcdefghijklmnopqrstuvwxyz");    
    tu_ctype("isdigit", isdigit, "0123456789");    
    //tu_ctype(isspace);    
    //tu_ctype(isalpha);    

    // TODO : compléter.

    return 0;
}

int test_read_serial()
{
	exec((paddr_t)proc_read_serial, "rserial");
	return 0;
}

char programme[0x3000];

#define TEST_EXEC
int test_elf()
{
	FILE* fd = NULL;

#ifdef TEST_EXEC
	int (*pp)(int argc, char** argv);
	int entry;
	int resultat;
#endif

	printf("Openning %s...\n");
	//fd = fopen("fd0:/system/kernel.bin", "r");
	fd = fopen("fd0:/a.out", "r");

	if(fd == NULL)
	{
		printf("failed.\n");
	}
	else
	{
		printf("Ok!\n");
#ifndef TEST_EXEC
		elf_info(fd);
#endif
		
#ifdef	TEST_EXEC
		entry = load_elf(fd, programme);
		pp = &(programme[entry]);
		resultat = pp(0,NULL);
		printf("resultat = %d\n",resultat);
#endif
	}
	/*close(fd);*/
	return 0;	
}
