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
#include <debug.h>
#include <unistd.h>
#include <list.h>
#include <serial.h>
#include <elf.h>
#include <ctype.h>

static int color;

static void handleClick(struct widget_t* wdg, int x __attribute__ ((unused)), int y __attribute__ ((unused))) {
	color = (color + 1) % 8;
	setWidgetProperties(wdg, 2, 2, 5, 10 + color, color, 6);
}

int gui_task(int argc __attribute__ ((unused)), char* argv[] __attribute__ ((unused))) {
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

int test_gui() {
	exec((paddr_t) gui_task, "tache_gui");
	return 0;
}

int test_fwrite() {
	FILE *file = fopen("fd0:/doc.txt", "w+");

	fwrite("Hello World !", sizeof(char), 13, file);
	fflush(file);
	return 0;
}

int test_fread() {
	FILE *file = fopen("fd0:/doc.txt", "r");

	char buffer[100];
	fread(buffer, sizeof(char), sizeof(buffer), file);

	buffer[99] = '\0';
	printf("%s", buffer);

	/*fflush(file);*/
	return 0;
}

void tu_seek0(const char * nom_test, const char * resultat_attendu, long offset) {
	FILE *file = fopen("fd0:/doc.txt", "r");
	int c;

	char buffer[11];
	fseek(file, offset, SEEK_SET);

	if (fread(buffer, sizeof(buffer), sizeof(char), file) > 0) {
		buffer[10] = '\0';
	} else {
		buffer[0] = '\0';
	}

	if (strcmp(buffer, resultat_attendu) == 0) {
		printf("%s [OK]\n", nom_test);
	} else {
		printf("%s [failed] :(\n", nom_test);
		printf("Attendu : %s\n Obtenu : %s\n", resultat_attendu, buffer);
		printf("Ou encore : ");
		for (c = 0; c < 11; c++) {
			printf("(%d)", buffer[c]);
		}
		printf("\n Attendu : ");
		for (c = 0; c < 11; c++) {
			printf("(%d)", resultat_attendu[c]);
		}
		printf("\n");
	}
}

int test_fseek() {
	tu_seek0("fseek debut+10", "86 refers ", 10);
	tu_seek0("fseek debut+100", "as launche", 100);
	tu_seek0("fseek debut+1000", "f servers ", 1000);
	tu_seek0("fseek debut+10000", "", 10000);

	//fclose(file);
	return 0;
}

int sem_t2(int argc __attribute__ ((unused)), char** argv __attribute__ ((unused))) {
	int semid1 = semget(4);
	int semid2 = semget(42);
	int i;
	while (1) {
		semP(semid1);
		for (i = 0; i < 0x005FFFFE; i++) {
		}
		printf("T2:Pong!\n");
		semV(semid2);

	}

	return 0;
}

int sem_t1(int argc __attribute__ ((unused)), char** argv __attribute__ ((unused))) {
	int semid1 = semget(4);
	int semid2 = semget(42);
	unsigned int i = 0;

	while (1) {
		semP(semid2);
		for (i = 0; i < 0x005FFFFE; i++) {
		}
		printf("T1:Ping?\n");
		semV(semid1);
	}
	return 0;
}

int test_semaphores() {
	/* Test des sémaphores: Deux taches effectuant un calcul (ici un boucle) s'alterne
	 * Pour se convaincre de l'efficacité, on peut utilise la commande "ps" pour constater
	 * que les deux tachent deviennent bien inactives (et donc ne consomment pas de CPU) l'une 
	 * après l'autre
	 */
	int semid1 = semcreate(4);
	int semid2;

	semid2 = semcreate(42);
	semP(semid1);

	exec((paddr_t) sem_t1, "ping");
	exec((paddr_t) sem_t2, "pong");

	return 0;
}

void test_memory_reserve_page_frame() {
	int i;
	scanf("%d", &i);
	while (i--)
		printf("Cadre de page réservé : %d\n", memory_reserve_page_frame());
}

int tu_rand() {
	int tab[100];
	int i;
	int n_tirages = 1000000;
	int n;
	char *nom_test = "rand";

	// init
	for (i = 0; i < 100; i++) {
		tab[i] = 0;
	}

	// tirages
	for (n = 0; n < n_tirages; n++) {
		i = (double) rand() / ((double) (RAND_MAX) + 1.0) * 100.0;
		tab[i]++;
	}

	double Q = 0;
	for (i = 0; i < 100; i++) {
		Q += (double) ((tab[i] - 10000) * (tab[i] - 10000)) / 10000.0;
	}

	if (Q < 123.23) { // 5% d'erreur (http://www.apprendre-en-ligne.net/random/tablekhi2.html)
		printf("%s [OK] (%d.%d)\n", nom_test, (int) Q, (int) ((Q - (int) Q)
				* 100));
		return 1;
	} else {
		printf("%s [failed] :(\n", nom_test);
		printf("Attendu : < 123.23\n Obtenu : %d.%d\n", (int) Q, (int) ((Q
				- (int) Q) * 100));
		printf("Ou encore : ");
		return 0;
	}
}

int pi(int argc __attribute__ ((unused)), char** argv __attribute__ ((unused))) {
	const double prec = 0.0001;
	double x = 0.0, y = 0.0;
	double res;
	int p_in = 0, p_tot = 0;
	for (x = 0; x < 1.0; x += prec) {
		for (y = 0; y < 1.0; y += prec) {
			if (x * x + y * y < 1.0)
				p_in++;

			p_tot++;
		}
	}
	res = 4.0 * (double) p_in / (double) p_tot;

	printf("Tesultat tache: P_in=%d, P_tot=%d, res=%d\n", p_in, p_tot,
			(int) (res * 1000.0));

	return 0;
}

int test_task1(int argc, char** argv __attribute__ ((unused))) {
	/*while(1)
		printf("Coucou task1\n");*/
	malloc(10);
	/*printf("Test task!\n");*/

	return 0;
}

int test_task() {
	exec_elf("fd0:/a.out");
	//create_process("test",(paddr_t)test_task1,"il est plus d'une heure du matin et je suis fatigué",0x1000,3);
	//create_process_test("test", (paddr_t)test_task1, 0x100 , "1 2", 0x1000, 3);

	return 0;
}

int calc_pi() {
	exec((paddr_t) pi, "pi");
	return 0;
}

int test_mouse_task() {
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

	while (1) {
		if (i % 100000 == 0) {
			getMouseCoord(&x, &y);

			if (getMouseBtn(0)) {
				if (!btn1_frz)
					col1 = (col1 + 1) % 0xF;
				btn1_frz = 1;
			} else {
				btn1_frz = 0;
			}

			if (getMouseBtn(1)) {
				if (!btn2_frz)
					col2 = (col2 + 1) % 0xF;
				btn2_frz = 1;
			} else {
				btn2_frz = 0;
			}

			x = x * 80 / 640;
			y = 25 - (y * 25 / 480);
			// XXX: 
			//set_attribute_position(0, col2, old_x, old_y);
			//set_attribute_position(col1, col2, x, y);
			old_x = x;
			old_y = y;
		}
		i++;
	}
	while (1)
		;
	return 0;
}

int test_mouse() {
	exec((paddr_t) test_mouse_task, "mouse");
	return 0;
}

int test_scanf() {
	printf("Entre un mot : \n");
	char b[100];
	scanf("%s", b);
	printf("Tu as tape le mot : %s\n", b);
	return 0;
}

int test_fgets() {
	printf("Entre une phrase : \n");
	char b[100];
	fgets(b, sizeof(b), stdin);
	printf("Tu as tape la phrase : %s", b);
	return 0;
}

int test_fputs() {
	fputs("Hello world!\n", stdout);
	return 0;
}

int test_ansi() {
	printf("\033[2J");
	fflush(stdout);
	printf("\033[8;20H\033[33m=====");
	printf("\033[8;60H\033[33m=====");

	printf("\033[14;41H\033[35m===");
	printf("\033[20;33H\033[37m===================\033[0m\n");
	return 0;
}

int proc_write_serial(int argc __attribute__ ((unused)), char** argv __attribute__ ((unused))) {
	char buffer[80];
	printf(">");
	while (1) {

		fflush(stdout);
		scanf("%s", buffer);

		serial_puts(COM1, buffer);
		serial_putc(COM1, ' ');
	}
	return 0;
}

int test_write_serial() {
	exec((paddr_t) proc_write_serial, "wserial");
	return 0;
}

int proc_read_serial(int argc __attribute__ ((unused)), char** argv __attribute__ ((unused))) {
	char buffer[64];

	while (1) {
		serial_gets(COM1, buffer, 64);
		printf("%s", buffer);
		fflush(stdout);
	}
}

void tu_ctype(const char * nom_test, int(*p)(), const char * resultat_attendu) {
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
		printf("Ou encore : ");
		for (c = 0; c < len; c++) {
			printf("(%d)", buf[c]);
		}
		printf("\n Attendu : ");
		for (c = 0; c < strlen(resultat_attendu); c++) {
			printf("(%d)", resultat_attendu[c]);
		}
		printf("\n");
	}
}

int tu_sscanf() {
	char * str = "World 42 Hello";
	char s[10];
	char s2[10];
	int d;
	char *resultat_attendu = "TU : Hello (42) World";
	char buf[80];

	sscanf(str, "%s %d %s", s, &d, s2);

	sprintf(buf, "TU : %s (%d) %s", s2, d, s);

	if (strcmp(buf, resultat_attendu) == 0) {
		printf("%s [OK]\n", "sscanf");
		return 1;
	} else {
		printf("%s [failed] :(\n", "sscanf");
		printf("Attendu : %s\n Obtenu : %s\n", resultat_attendu, buf);
		return 0;
	}
}

int tu_sprintf() {
	char *t = "Hello";
	char *t2 = "World";
	int d = 42;
	char *resultat_attendu = "TU : Hello (42) World";
	char buf[80];
	int r = sprintf(buf, "TU : %s (%d) %s", t, d, t2);

	if (strcmp(buf, resultat_attendu) == 0 && r == (int) strlen(
			resultat_attendu)) {
		printf("%s [OK]\n", "sprintf");
		return 1;
	} else {
		printf("%s [failed] :(\n", "sprintf");
		printf("Attendu : %s\n Obtenu : %s\n", resultat_attendu, buf);
		printf("Code retour : %d\n", r);
		printf("Code retour attendu : %d\n", strlen(resultat_attendu));
		return 0;
	}
}

int test_ctype() {
	tu_ctype("isupper", isupper, "ABCDEFGHIJKLMNOPQRSTUVWXYZ");
	tu_ctype("islower", islower, "abcdefghijklmnopqrstuvwxyz");
	tu_ctype("isalpha", isalpha,
			"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz");
	tu_ctype("isdigit", isdigit, "0123456789");
	tu_ctype("isalnum", isalnum,
			"0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz");
	tu_ctype("ispunct", ispunct, "!\"#$%&'()*+,-./:;<=>?@[\\]^_`{|}~");
	tu_ctype("isspace", isspace, "\t\n\v\f\r ");
	tu_ctype(
			"isprint",
			isprint,
			" !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~");

	// TODO : compléter.

	return 0;
}

int test_stdio() {
	tu_rand(); // TODO : le bouger ailleurs.

	tu_sprintf() && tu_sscanf();
}

int test_read_serial() {
	exec((paddr_t) proc_read_serial, "rserial");
	return 0;
}


int test_elf() {
	FILE* fd = NULL;
	char filename[80];
	scanf("%s", filename);
	printf("Openning %s...\n", filename);
	//fd = fopen("fd0:/system/kernel.bin", "r");
	fd = fopen(filename, "r");

	if (fd == NULL) {
		printf("failed.\n");
	} else {
		elf_info(fd);
		printf("MEMSIZE: 0x%x\n", elf_size(fd));
	}
	
	/*close(fd);*/
	return 0;
}
