#include <multiboot.h>
#include <idt.h>
#include <i8259.h>
#include <stdio.h>
#include <stdlib.h>
#include <mempage.h>
#include <gdt.h>
#include <exception.h>
#include <interrupts.h>
#include <pci.h>
#include <pci_config.h>
#include <scheduler.h>
#include <dummy_process.h>
#include <keyboard.h>

typedef struct
{
  uint8_t lol;
} kernel_options;

/* Forward declarations. */
void cmain (unsigned long magic, unsigned long addr);
static void testPageReservation();
static void initKernelOptions(const char *cmdLine, kernel_options *options);

static void testhandlerexception(int error_id)
{
	/* TODO : L'erreur renvoyée devrait être 0 car il n'y a pas de code retour. */
	printf("Exception : %d\n", error_id);
}

static void tick(int interrupt_id)
{
  static int tic = 0, tac = 0;
  tic++;
  tic %= 100;
  if(tic == 0)
  {
    tac++;
    //printf("clock %d\n", tac);
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

	/* The Hello World */
	//printf("\nHello World !\n\n");
	/* Mieux! */
	printf("_|_|_|_|_|                      _|_|      _|_|_|\n");
	printf("    _|      _|_|_|    _|_|_|  _|    _|  _|      \n");
	printf("    _|    _|    _|  _|        _|    _|    _|_|  \n");
	printf("    _|    _|    _|  _|        _|    _|        _|\n");
	printf("    _|      _|_|_|    _|_|_|    _|_|    _|_|_|    ");
  printf("(codename:fajitas)\n\n\n");

	printf("Memoire disponible : %dMio\n", (mbi->mem_upper>>10) + 1); /* Grub balance la mémoire dispo -1 Mio... Soit.*/

	//gdt_setup();

	/* Mise en place de la table qui contient les descripteurs d'interruption (idt) */
	idt_setup();

	/* Configuration du i8259 qui s'occupe des interruptions. */
	i8259_setup();

	exception_set_routine(EXCEPTION_DIVIDE_ERROR, testhandlerexception);
	interrupt_set_routine(IRQ_TIMER, tick);
  interrupt_set_routine(IRQ_KEYBOARD, keyboardInterrupt);

	asm volatile ("sti\n");

	/* Configuration de la pagination */
	mempage_setup((mbi->mem_upper << 10) + (1 << 20));

	//mempage_print_free_pages();
	//mempage_print_used_pages();

	//printf("Div 0 : %d.\n", 3/0);
	pci_scan();
	pci_list();



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
	
  
	for(;;)
  {
    char c;
    printf("\n> ");
    while((c = getchar()) != '\n')
      putchar(c);
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
  mempage_print_free_pages();
  mempage_print_used_pages();
  waitReturn();

  addr = mempage_reserve_page();
  mempage_print_free_pages();
  mempage_print_used_pages();
  waitReturn();

  mempage_free_page(addr);
  mempage_print_free_pages();
  mempage_print_used_pages();
  waitReturn();
}

void testMalloc()
{
  int *nyu1, *nyu2, *nyu3, *nyu4;
  printf("NYU : %d\n", nyu1 = (int *) malloc(10));
  printMallocated();
  printf("NYU : %d\n", nyu2 = (int *) malloc(20));
  printMallocated();
  printf("NYU : %d\n", nyu3 = (int *) malloc(42));
  printMallocated();
  printf("NYU : %d\n", nyu4 = (int *) malloc(1337));
  printMallocated();

  free(nyu3);
  printMallocated();

  printf("NYU : %d\n", nyu3 = (int *) malloc(10));
  printMallocated();
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

