#include <multiboot.h>
#include <idt.h>
#include <i8259.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <memory.h>
#include <pagination.h>
#include <gdt.h>
#include <exception.h>
#include <interrupts.h>
#include <pci.h>
#include <pci_config.h>
#include <scheduler.h>
#include <time.h>
#include <dummy_process.h>
#include <keyboard.h>
#include <mouse.h>
#include <events.h>
#include <floppy.h>
#include <kpanic.h>
#include <process.h>
#include <kmalloc.h>
#include <vmm.h>
#include <ioports.h>
#include "msr.h"
#include "fat.h"
#include <fpu.h>
#include <vm86.h>
#include <beeper.h>

typedef struct
{
  uint8_t lol;
} kernel_options;

/* Forward declarations. */
void cmain (unsigned long magic, unsigned long addr);
int shell(int argc, char* argv[]);
static void testPageReservation();
static void initKernelOptions(const char *cmdLine, kernel_options *options);
static void test_kmalloc();

void LPT1_routine(int id)
{
  // XXX : avoid segment_not_present
}

void exit(uint32_t value)
{
	syscall(0,value,0,0);
	while(1); // Pour ne pas continuer à executer n'importe quoi alors que le processus est sensé être arrété
}

uint32_t get_pid()
{
	int pid;
	syscall(1,&pid, 0, 0);
	return pid;
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

int test_task1(int argc, char** argv)
{
	int pid = get_pid();
	printf("\nTache n%d\n",pid);
	
	exit(0);
  return 0;
}


void* sys_exit(uint32_t ret_value, uint32_t zero1, uint32_t zero2)
{
	process_t* current;
	// On cherche le processus courant:
	current = get_current_process();
	
	// On a pas forcement envie de supprimer le processus immédiatement
	current->state = PROCSTATE_TERMINATED; 
	
	printf("DEBUG: exit(process %d ralut !turned %d)\n", current->pid, ret_value);

  return NULL;
}

void* sys_getpid(uint32_t* pid, uint32_t zero1, uint32_t zero2)
{
	process_t* process = get_current_process();
	*pid = process->pid;
  
  return NULL;
}

void cmain (unsigned long magic, unsigned long addr) {
	multiboot_info_t *mbi;
	kernel_options options;
	uint32_t esp_tss;

	/* Clear the screen. */
	cls ();

	/* Am I booted by a Multiboot-compliant boot loader? */
	if (magic != MULTIBOOT_BOOTLOADER_MAGIC)
	{
		printf ("Invalid magic number: 0x%x\n", (unsigned) magic);
		return;
	}

	/* Set MBI to the address of the Multiboot information structure. */
	mbi = (multiboot_info_t *) addr;
	initKernelOptions((char *)mbi->cmdline, &options);

	printf("_|_|_|_|_|                      _|_|      _|_|_|\n");
	printf("    _|      _|_|_|    _|_|_|  _|    _|  _|      \n");
	printf("    _|    _|    _|  _|        _|    _|    _|_|  \n");
	printf("    _|    _|    _|  _|        _|    _|        _|\n");
	printf("    _|      _|_|_|    _|_|_|    _|_|    _|_|_|    ");
	printf("(codename:fajitas)\n\n\n");

	//beep();

	printf("Memoire disponible : %dMio\n", (mbi->mem_upper>>10) + 1); /* Grub balance la mémoire dispo -1 Mio... Soit.*/

	gdt_setup((mbi->mem_upper << 10) + (1 << 20));
	
	asm("":"=a"(esp_tss));
	init_tss(esp_tss);

	/* Mise en place de la table qui contient les descripteurs d'interruption (idt) */
	idt_setup();

	/* Configuration du i8259 qui s'occupe des interruptions. */
	i8259_setup();

	kpanic_init();
  
	interrupt_set_routine(IRQ_KEYBOARD, keyboardInterrupt, 0);
	interrupt_set_routine(IRQ_LPT1, LPT1_routine, 0);
	mouseInit();
	init_syscall();
	floppy_init_interrupt();

  init_fpu();

	events_init(); 
	
	asm volatile ("sti\n");

	/* Configuration de la pagination */
			//asm("hlt");
	memory_setup((mbi->mem_upper << 10) + (1 << 20));

	pagination_setup();

	//memory_print_free_pages();
	//memory_print_used_pages();

  /* Initialisation de la vmm */
  init_vmm();
  init_kmalloc();
//	printf("Div 0 : %d.\n", 3/0);
//	pci_scan();
//	pci_list();

	floppy_detect_drives();
	printf("Floppy controller version: 0x%x.\n", floppy_get_version());
	
	if(init_floppy() != 0)
		printf("Initialisation du lecteur a echoue.\n");
		
	
	/*   Test FAT    */
	//mount_fat_fs ();
	mount_FAT12 ();
	
	
	/* Test du scheduler */
	
	init_scheduler(10);

	paddr_t _addr = shell;
	create_process(_addr,0,NULL,1024,3);
	//process_print_regs();
/*
	_addr = test_task1;
	kmalloc(64);
	create_process(_addr, 0, NULL, 64, 3);
	//while(1);
*/
	printf("vm86:%d\n",check_vm86());
	
	syscall_set_handler(0,sys_exit);
	syscall_set_handler(1,sys_getpid);
	start_scheduler();
  //shell(0, NULL);

	while(1){}
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
			
		while((c = getchar()) != '\n') {
			buffer[i%80] = c;
			i++;
		}
		buffer[i%80] = '\0';
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
			process_t* proc = create_process(proc_addr,0,NULL,1024,3);
			//add_process(*proc);
			//while(1);
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
			print_file(stdin);
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
			//printf("root directory:\n");
			list_segments ();
		}
		if (strcmp(buffer, "mount") == 0) {
			print_Boot_Sector ();
		}
		if (strcmp(buffer, "pwd") == 0) {
			print_working_dir ();
		}
		if (strcmp(buffer, "debugfat") == 0) {
			print_path ();
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

void waitReturn()
{
	while(getchar() != '\n')
		;
}

void testPageReservation()
{
	paddr_t addr;
	memory_print_free_pages();
	memory_print_used_pages();
	waitReturn();

	addr = memory_reserve_page_frame();
	memory_print_free_pages();
	memory_print_used_pages();
	waitReturn();

	memory_free_page_frame(addr);
	memory_print_free_pages();
	memory_print_used_pages();
	waitReturn();
}

static void defaultOptions(kernel_options *options)
{
	options->lol = 0;
}

static char get_opt(char **cmdLine)
{
	
	while(**cmdLine != '-' && **cmdLine != '\0') 
		(*cmdLine)++;

	if(**cmdLine == '\0')
		return -1;

	*cmdLine += 2;
	return *(*cmdLine - 1);
}

static void initKernelOptions(const char *cmdLine, kernel_options *options)
{
	char *cmd = cmdLine;
	char opt;

	printf("Command line : %s\n", cmdLine);

	defaultOptions(options);
	while((opt = get_opt(&cmd)) != -1)
	{
		switch(opt)
		{
		case 'l':
				options->lol = 1;
				enableFinnouMode(1);
				break;
		default: printf("Unknown option %c\n", opt);
		}
	}
}

static void test_kmalloc()
{
  kmalloc_print_mem();
  getchar();
  int *a = kmalloc(300*sizeof(int));
  a[0] = 100;
  kmalloc_print_mem();
  getchar();
  a = kmalloc(300*sizeof(int));
  a[0] = 100;
  kmalloc_print_mem();
  getchar();
  a = kmalloc(300*sizeof(int));
  a[0] = 100;
  kmalloc_print_mem();
  getchar();
  a = kmalloc(300*sizeof(int));
  a[0] = 100;
  kmalloc_print_mem();
  getchar();
  a = kmalloc(300*sizeof(int));
  a[0] = 100;
  kmalloc_print_mem();
  getchar();
  a = kmalloc(300*sizeof(int));
  a[0] = 100;
  kmalloc_print_mem();
  getchar();
  a = kmalloc(300*sizeof(int));
  a[0] = 100;
  kmalloc_print_mem();
  getchar();
  printf("LAST\n");
  a = kmalloc(300*sizeof(int));
  a[0] = 100;
  kmalloc_print_mem();
  getchar();
}

