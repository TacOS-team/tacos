/**
 * @file tests.c
 *
 * @author TacOS developers 
 *
 * Maxime Cheramy <maxime81@gmail.com>
 * Nicolas Floquet <nicolasfloquet@gmail.com>
 * Benjamin Hautbois <bhautboi@gmail.com>
 * Ludovic Rigal <ludovic.rigal@gmail.com>
 * Simon Vernhes <simon@vernhes.eu>
 *
 * @section LICENSE
 *
 * Copyright (C) 2010 - TacOS developers.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 3 of
 * the License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details at
 * http://www.gnu.org/copyleft/gpl.html
 *
 * You should have received a copy of the GNU General Public License 
 * along with this program; if not, see <http://www.gnu.org/licenses>.
 *
 * @section DESCRIPTION
 *
 * Description de ce que fait le fichier
 */

#include <fat.h>
#include <floppy.h>
#include <kmalloc.h>
#include <process.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <apps.h>
#include <sem.h>
#include <memory.h>
#include <debug.h>
#include <unistd.h>
#include <elf.h>
#include <ctype.h>
#include <errno.h>
#include <rtl8139.h>


int test_fwrite() {
	FILE *file = fopen("fd0:/test.txt", "w+");
	fwrite("Hello World !", sizeof(char), 13, file);
	fflush(file);
	return 0;
}

int test_fread() {
	FILE *fd = fopen("fd0:/bin/cube", "r");
	uint32_t i;
	char* ptr1;
	char* ptr2;
	Elf32_Ehdr header1;
	Elf32_Ehdr header2;
	
	/* Charge le header avec fread */
	fseek(fd, 0, SEEK_SET);
	fread(&header1, sizeof(Elf32_Ehdr), 1, fd);
	
	/* Charge le header avec une boucle de fgetc */
	fseek(fd, 0, SEEK_SET);	
	char* pointeur = (char*) &header2;
	for(i=0; i<sizeof(Elf32_Ehdr); i++)
	{
		*pointeur = fgetc(fd);
		pointeur++;
	}
	
	/* Comparaison des résultats */
	ptr1 = (char*)&header1;
	ptr2 = (char*)&header2;
	
	for(i=0; i<sizeof(Elf32_Ehdr); i++)
	{
		if(ptr1[i] != ptr2[i])
		{
			printf("[Echec] A l'offset 0x%x: fgetc donne 0x%x, fread donne 0x%x.\n",i,ptr2[i], ptr1[i]);	
			break;
		}
	}
	
	printf("\n");
	/*fflush(file);*/
	return 0;
}

int test_ofd_flags_rdonly() {
	int ofd = open("fd0:/doc.txt", O_RDONLY);
  int res = write(ofd, "42", 2);

  printf("EOF : %c\n", res == -1 ? 'y' : 'n');
  printf("errno : %d\n", errno);
  /*close(ofd)*/

	return 0;
}

int test_ofd_flags_wronly() {
  char buf[16] = "123456789012345";
	int ofd = open("fd0:/doc.txt", O_WRONLY);
  int res = read(ofd, &buf, 15);
  buf[16] = '\0';

  printf("EOF : %c\n", res == -1 ? 'y' : 'n');
  printf("errno : %d\n", errno);
  printf("buffer : %s\n", buf);
  /*close(ofd)*/

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



int test_ansi() {
	printf("\033[2J");
	fflush(stdout);
	printf("\033[8;20H\033[33m=====");
	printf("\033[8;60H\033[33m=====");

	printf("\033[14;41H\033[35m===");
	printf("\033[20;33H\033[37m===================\033[0m\n");
	return 0;
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

	return tu_sprintf() && tu_sscanf();
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

int test_send_packet()
{
	#if 1
	char packet[1500];
	
	/* Dest MAC addr */
	packet[0] = 0xff;
	packet[1] = 0xff;
	packet[2] = 0xff;
	packet[3] = 0xff;
	packet[4] = 0xff;
	packet[5] = 0xff;
	
	/* Source address */
	packet[6] = 0x11;
	packet[7] = 0x22;
	packet[8] = 0x33;
	packet[9] = 0x44;
	packet[10] = 0x55;
	packet[11] = 0x66;
	
	/* Ethertype */
	packet[12] = 0x08;
	packet[13] = 0x06;
	
	/* Payload */
	packet[14] = 0x00;
	packet[15] = 0x01;
	packet[16] = 0x08;
	packet[17] = 0x00;
	packet[18] = 0x06;
	packet[19] = 0x04;
	packet[20] = 0x00;
	packet[21] = 0x01;
	
	packet[22] = 0x11; /* Mac origine */
	packet[23] = 0x22;
	packet[24] = 0x33;
	packet[25] = 0x44;
	packet[26] = 0x55;
	packet[27] = 0x66;
	
	packet[28] = 0xc0;/* IP origine */
	packet[29] = 0xa8;
	packet[30] = 0x00;
	packet[31] = 0x02;
	
	packet[32] = 0x00;
	packet[33] = 0x00;
	packet[34] = 0x00;
	packet[35] = 0x00;
	packet[36] = 0x00;
	packet[37] = 0x00;
	
	packet[38] = 0xc0;
	packet[39] = 0xa8;
	packet[40] = 0x00;
	packet[41] = 0x01;
	
	send_packet(packet, 42);
	#endif
	/*
	uint32_t i = 0;
	while(1)
	{
		printf("\033[2J");
		fflush(stdout);
		rtl8139_debug_info();
		for(i=0; i<0x00ffffff; i++);
	}*/
	
	return 0;
}
