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
#include "msr.h"

typedef struct
{
  uint8_t lol;
} kernel_options;

/* Forward declarations. */
void cmain (unsigned long magic, unsigned long addr);
int shell(int argc, char* argv[]);
static void testPageReservation();
static void initKernelOptions(const char *cmdLine, kernel_options *options);
/*
static void processA (paddr_t* pStackA, paddr_t* pStackB) {
	int i;
	for (i=0;i<5;i++) {
		printf(" PA(%d) ",i);
		cpu_ctxt_switch(pStackA, pStackB);
	}
	cpu_ctxt_switch(pStackA, pStackB);
}
static void processB (paddr_t* pStackA, paddr_t* pStackB, paddr_t* pStackMain) {
	int i;
	for (i=0;i<5;i++) {
		printf(" PB(%d) ",i);
		cpu_ctxt_switch(pStackB, pStackA);
	}
	cpu_ctxt_switch(pStackB, pStackMain);
}
*/

int test_task(int argc, char** argv)
{
	int i = 0;
	printf("---- Test Task ----\n");
	
	while(1)
	{
		printf("i=%d\n",i);
		i++;
	}
}


static void* sched(void* data)
{
	uint32_t* stack_ptr;
	process_t current;
	/* On récupere un pointeur de pile pour acceder aux registres empilés */
	asm("mov (%%ebp), %%eax; mov %%eax, %0" : "=m" (stack_ptr) : );
	
	/* On met le context dans la structure "process" */
	current.regs.ss = stack_ptr[19];
	current.regs.esp = stack_ptr[18];
	current.regs.eflags = stack_ptr[17];
	current.regs.cs  = stack_ptr[16];
	current.regs.eip = stack_ptr[15];
	current.regs.eax = stack_ptr[14];
	current.regs.ecx = stack_ptr[13];
	current.regs.edx = stack_ptr[12];
	current.regs.ebx = stack_ptr[11];
	//->esp kernel, on saute
	current.regs.ebp = stack_ptr[9];
	current.regs.esi = stack_ptr[8];
	current.regs.edi = stack_ptr[7];
	current.regs.fs = stack_ptr[6];
	current.regs.gs = stack_ptr[5];
	current.regs.ds = stack_ptr[4];
	current.regs.es = stack_ptr[3];
/*	
	printf("ss: 0x%x\n", current.regs.ss);
	printf("ss: 0x%x\n", current.regs.ss);		
	printf("esp: 0x%x\n", current.regs.esp); 
	printf("flags: 0x%x\n", current.regs.eflags);
	printf("cs: 0x%x\n", current.regs.cs); 
	printf("eip: 0x%x\n", current.regs.eip); 
	printf("eax: 0x%x\n", current.regs.eax); 
	printf("ecx: 0x%x\n", current.regs.ecx); 
	printf("edx: 0x%x\n", current.regs.edx); 
	printf("ebx: 0x%x\n", current.regs.ebx); 
	printf("ebp: 0x%x\n", current.regs.ebp); 
	printf("esi: 0x%x\n", current.regs.esi); 
	printf("edi: 0x%x\n", current.regs.edi); 
	printf("fs: 0x%x\n", current.regs.fs); 
	printf("gs: 0x%x\n", current.regs.gs);  
	printf("ds: 0x%x\n", current.regs.ds); 
	printf("es: 0x%x\n", current.regs.es); 
*/
	add_event(sched,NULL,10);
	printf("SCHED!\n");
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
	init_tss(0x1FFF0);
	
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
	memory_setup((mbi->mem_upper << 10) + (1 << 20));
	//
	//memory_print_free_pages();
	//memory_print_used_pages();

//	printf("Div 0 : %d.\n", 3/0);
	pci_scan();
//	pci_list();

	floppy_detect_drives();
	printf("Floppy controller version: 0x%x.\n", floppy_get_version());
	
	if(init_floppy() != 0)
		printf("Initialisation du lecteur a echoue.\n");
		
	char MBR[FLOPPY_SECTOR_SIZE];
	floppy_read_sector(0,0,0,MBR);
	printf("MBR Signature:0x%x%x.\n",0xFF&MBR[0x01FE], 0xFF&MBR[0x01FF]);
	
	// Là normalement on lance le scheduler avec le process d'initialisation ou un shell
	//init_scheduler(10, shell, 0, NULL);
	//add_event(sched,NULL,10);
	//exec_task(test_task, 2, NULL);
	exec_task(shell, 2, NULL);
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
		if( strcmp(buffer, "erase_mbr") == 0)
		{
			char zeros[FLOPPY_SECTOR_SIZE];
			set_attribute(0, 4);
			cls();
			printf("\n/!\\ ERASING MBR MOUHAHA /!\\\n");
			floppy_write_sector(0,0,0,zeros);
			reset_attribute();
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

