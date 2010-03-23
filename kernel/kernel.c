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
//#include <scheduler.h>
#include <dummy_process.h>
#include <keyboard.h>
#include <clock.h>
#include <events.h>
#include <floppy.h>
#include <kpanic.h>
#include <process.h>
#include <kmalloc.h>
#include <vmm.h>
#include <ioports.h>
#include "msr.h"
#include <mbr.h>

typedef struct
{
  uint8_t lol;
} kernel_options;

/* Forward declarations. */
void cmain (unsigned long magic, unsigned long addr);
int shell(int argc, char* argv[]);
static void testPageReservation();
static void initKernelOptions(const char *cmdLine, kernel_options *options);

/* pour le test des processus */
process_t task[3];
uint32_t sys_stack[3][1024];
uint32_t user_stack[3][1024];

int test_task1(int argc, char** argv)
{
	int i = 0;
	printf("---- Test Task1 ----\n");
	
	while(1)
	{
		if(i%5000000 == 0)
		{
			printf("\ntask1 dit:\"Je tourne!!!\"\n");
		}
		i++;
	}
}

int test_task2(int argc, char** argv)
{
	int i = 0;
	printf("---- Test Task2 ----\n");
	
	while(1)
	{
		if(i%5000000 == 0)
		{
			printf("\ntask2 dit:\"Je tourne!!!\"\n");
		}
		i++;
	}
}
	
void cmain (unsigned long magic, unsigned long addr) {
	multiboot_info_t *mbi;
	kernel_options options;

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

	printf("Memoire disponible : %dMio\n", (mbi->mem_upper>>10) + 1); /* Grub balance la mémoire dispo -1 Mio... Soit.*/

	gdt_setup((mbi->mem_upper << 10) + (1 << 20));
	init_tss(sys_stack+1023);
	
	init_syscall();

	/* Mise en place de la table qui contient les descripteurs d'interruption (idt) */
	idt_setup();

	/* Configuration du i8259 qui s'occupe des interruptions. */
	i8259_setup();

	kpanic_init();
  
	interrupt_set_routine(IRQ_KEYBOARD, keyboardInterrupt);
	floppy_init_interrupt();
	
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
		
	/*char MBR[FLOPPY_SECTOR_SIZE];
	floppy_read_sector(0,0,0,MBR);
	printf("MBR Signature:0x%x%x.\n",0xFF&MBR[0x01FE], 0xFF&MBR[0x01FF]);*/
	
	mbr_read ();
	mbr_print_signature ();
	mbr_print_part_table ();
	
	/* Test du scheduler */
	init_scheduler(5);

	paddr_t _addr = shell;
	//process_t* shell = create_process(_addr,0,NULL,64,3);

	/*_addr = test_task1;
	process_t* test = create_process(_addr, 0, NULL, 64, 3);
		while(1);
*/
	//add_process(*shell);

	//start_scheduler();
  shell(1, NULL);

	while(1){}
}

int shell(int argc, char* argv[])
{
	char buffer[80];
	int i = 0;
	for(;;)
	{
		char c;
		date_t date = get_date();
		printf("\n%d\\%d\\%d : %s%dh%s%dm%s%ds > ", date.day, date.month, date.year,
												date.hour < 10 ? "0" : "", date.hour, date.minute < 10 ? "0" : "", date.minute, date.sec < 10 ? "0" : "", date.sec);
		while((c = getchar()) != '\n') {
			buffer[i%80] = c;
			putchar(c);
			i++;
		}
		buffer[i%80] = '\0';
		i = 0;

		if (strcmp(buffer, "help") == 0) {
			printf("\nCommandes dispos : reboot, halt, clear, sleep, lspci, switchdebug, switchstd, erase_mbr, test_task, print_memory\n");
		}
		if (strcmp(buffer, "reboot") == 0) {
			printf("\nReboot non implemente, desole !");
		}
		if (strcmp(buffer, "halt") == 0) {
			printf("\nHalt !");
			asm("cli");
			asm("hlt");
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
			printf("\n/!\\ ERASING MBR MOUHAHA /!\\\n");
			floppy_write_sector(0,0,0,zeros);
			reset_attribute();
		}		
		if(strcmp(buffer,"test_task") == 0)
		{
				paddr_t proc_addr = test_task1;
				process_t* proc = create_process(proc_addr,0,NULL,64,3);
				add_process(*proc);
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

