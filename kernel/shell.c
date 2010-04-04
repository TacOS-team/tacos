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


int test_task1(int argc, char** argv)
{
	int pid = get_pid();
	printf("\nTache n%d\n",pid);
	printf("Parameters:\nargc=%d\nargv=0x%x\n",argc, argv);
	return 0;
}


int test_mouse(int argc, char** argv)
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
}



int shell(int argc, char* argv[])
{
	char buffer[80];
	//kmalloc(10);
	//asm("xchg %bx, %bx");
	int i = 0;
	
	for(;;)
	{
		char c;
		
		time_t curr_time = time(NULL);
		printf("\n > ");
		fflush(stdout);
		
		scanf("%s", buffer);	
		printf("\n");

		i = 0;
		if (strcmp(buffer, "help") == 0) {
			printf("Commandes dispos : reboot, halt, clear, sleep, lspci, switchdebug, switchstd, erase_mbr, test_task, print_memory, date, test_mouse\n");
		}
		if (strcmp(buffer, "reboot") == 0) {
			printf("Reboot non implemente, desole !");
		}
		if (strcmp(buffer, "halt") == 0) {
			printf("Halt !");
			asm("cli");
			asm("hlt");
		}
		if( strcmp(buffer, "date") == 0)
		{
			time_t curr_time = time(NULL);
			printf("%s",ctime(&curr_time));
		}
		if( strcmp(buffer, "clear") == 0)
			cls();
		if( strcmp(buffer, "sleep") == 0)
			sleep(1);
		if( strcmp(buffer, "lspci") == 0)
			pci_list();
		if (strcmp(buffer, "switchdebug") == 0) {
			switchDebugBuffer();
		}
		if (strcmp(buffer, "switchstd") == 0) {
			switchStandardBuffer();
		}
		if (strcmp(buffer, "print_memory") == 0) {
			memory_print();
		}
		if( strcmp(buffer, "erase_mbr") == 0)
		{
			char zeros[FLOPPY_SECTOR_SIZE];
			set_attribute(0, 4);
			cls();
			printf("/!\\ ERASING MBR MOUHAHA /!\\\n");
			floppy_write_sector(0,0,0,zeros);
			reset_attribute();
		}		
		if(strcmp(buffer,"test_task") == 0)
		{
			paddr_t proc_addr =  test_task1;
			process_t* proc = create_process(proc_addr,42,NULL,1024,3);
		}
		if(strcmp(buffer,"test_mouse") == 0)
		{
			test_mouse(0,NULL);
		}
		if(strcmp(buffer,"syscall") == 0)
		{
			//syscall(0x42,0,1,2);
			exit(-1);
		}
		if (strcmp(buffer, "scanf") == 0) {
			printf("Entre un mot : \n");
			char b[100];
			scanf("%s", b);
			printf("Tu as tape le mot : %s\n", b);
		}
		if(strcmp(buffer,"ps")==0)
		{
			print_process_list();
		}
		if (strcmp(buffer, "test_sscanf") == 0) {
			char * str = "Hello 42 bla";
			char s[10];
			char s2[10];
			int d;

			sscanf(str, "%s %d %s", s, &d, s2);

			printf("%s %d %s\n", s2, d, s);
		}
		if(strcmp(buffer, "kmalloc_print_mem") == 0)
			kmalloc_print_mem();
		if(strcmp(buffer, "test_kmalloc") == 0)
			test_kmalloc();
		if (strcmp(buffer, "ls") == 0) {
			list_segments ();
		}
		if (strcmp(buffer, "mount") == 0) {
			print_Boot_Sector ();
		}
		if (strcmp(buffer, "pwd") == 0) {
			print_working_dir();
		}
		if (strcmp(buffer, "debugfat") == 0) {
			print_path();
			//open("lklk",21);
		}
		if (strcmp(buffer, "clean_proclist") == 0)
		{
			printf("cleaning proclist...\n");
			clean_process_list();
		}
		
		if (strcmp(buffer, "cd") == 0) {
			while((c = getchar()) != '\n') {
				buffer[i%80] = c;
				i++;
			}
			buffer[i%80] = '\0';
			i = 0;
			change_dir(buffer);
		}
	}
	
	return 0;
}
