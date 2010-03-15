#include <multiboot.h>
#include <idt.h>
#include <i8259.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include <pagination.h>
#include <gdt.h>
#include <exception.h>
#include <interrupts.h>
#include <pci.h>
#include <pci_config.h>
#include <scheduler.h>
#include <dummy_process.h>
#include <keyboard.h>
#include <clock.h>
#include <events.h>
#include <floppy.h>
#include <kpanic.h>

typedef struct
{
  uint8_t lol;
} kernel_options;

/* Forward declarations. */
void cmain (unsigned long magic, unsigned long addr);
static void testPageReservation();
static void initKernelOptions(const char *cmdLine, kernel_options *options);

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
	pagination_setup();

	//memory_print_free_pages();
	//memory_print_used_pages();

//	printf("Div 0 : %d.\n", 3/0);
	pci_scan();
//	pci_list();

	/* PLANTE !!!!

	//----------- TEST CONTEXT SWICHING ------------------------- //
	//Creation des piles pour les Processus A et B
	paddr_t pPage, pStackA, pStackB, pCurrentStack;
	pPage = memory_reserve_page_frame();
	pStackA = pPage + PAGE_SIZE -1;
	pPage = memory_reserve_page_frame();
	pStackB = pPage + PAGE_SIZE -1;
	
	asm volatile (" movl %%esp, %0;" :: "m"(pCurrentStack) );
 
	cpu_ctxt_init(processA);
	cpu_ctxt_init(processB);

	//cpu_ctxt_switch(&pCurrentStack, &pStackA);

	//recopie de dummy process plus loin en memoire
	int i;
	paddr_t rec = (mbi->mem_upper+mbi->mem_lower)/2;
	for(i=0 ; i<mbi->mem_upper/10 ; i++) 
	{
		*((uint32_t*)(rec+i)) = *((uint32_t*)(dummy_process_main+i));
	}
	
	// execution de dummy process	
	printf("\nExecuting process dummy 1\n");
	char* args[] = {"dummy","1"};
	add_process(rec,2,(uint8_t**)args);
	
	*/

	floppy_detect_drives();
	printf("Floppy controller version: 0x%x.\n", floppy_get_version());
	
	if(init_floppy() != 0)
		printf("Initialisation du lecteur a echoue.\n");
		
	char MBR[512];
	floppy_read_sector(0,0,0,MBR);
	printf("MBR Signature:0x%x%x.\n",0xFF&MBR[0x01FE], 0xFF&MBR[0x01FF]);

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
			// ça marche, merci floppy
			floppy_read_sector(0xFFFF,0,0,0,MBR);
			printf("\nReboot non implemente, desole !");
		}
		if (strcmp(buffer, "halt") == 0) {
			printf("\nHalt !");
			asm("cli");
			asm("hlt");
		}
		if( strcmp(buffer, "clear") == 0)
			cls();
		if( strcmp(buffer, "lspci") == 0)
			pci_list();
			
	}
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

