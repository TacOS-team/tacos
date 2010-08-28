#include <multiboot.h>
#include <idt.h>
#include <i8259.h>
#include <stdio.h>
#include <stdlib.h>
#include <kunistd.h>
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
#include <keyboard.h>
#include <mouse.h>
#include <events.h>
#include <floppy.h>
#include <kpanic.h>
#include <kprocess.h>
#include <kmalloc.h>
#include <vmm.h>
#include <ioports.h>
#include "msr.h"
#include <fat.h>
#include <fpu.h>
#include <vm86.h>
#include <beeper.h>
#include <kfcntl.h>
#include <debug.h>
#include <shell.h>
#include <ksyscall.h>
#include <syscall.h>
#include <ksem.h>
#include <video.h>
#include <serial.h>

typedef struct
{
  uint8_t lol;
} kernel_options;

/* Forward declarations. */
void cmain (unsigned long magic, unsigned long addr);
static void initKernelOptions(const char *cmdLine, kernel_options *options);

void LPT1_routine(int id __attribute__ ((unused)))
{
  // XXX : avoid segment_not_present
}

void cmain (unsigned long magic, unsigned long addr) {
	multiboot_info_t *mbi;
	kernel_options options;
	uint32_t esp_tss;

	/* Am I booted by a Multiboot-compliant boot loader? */
	if (magic != MULTIBOOT_BOOTLOADER_MAGIC)
	{
		kprintf ("Invalid magic number: 0x%x\n", (unsigned) magic);
		return;
	}

	/* Set MBI to the address of the Multiboot information structure. */
	mbi = (multiboot_info_t *) addr;
	initKernelOptions((char *)mbi->cmdline, &options);

	gdt_setup((mbi->mem_upper << 10) + (1 << 20));
	
	asm("":"=a"(esp_tss));
	init_tss(esp_tss);

	/* Mise en place de la table qui contient les descripteurs d'interruption (idt) */
	idt_setup();

	/* Configuration du i8259 qui s'occupe des interruptions. */
	i8259_setup();

	init_video();

	kpanic_init();
  
	interrupt_set_routine(IRQ_KEYBOARD, keyboardInterrupt, 0);
	interrupt_set_routine(IRQ_LPT1, LPT1_routine, 0);

	mouseInit();

	floppy_init_interrupt();

	init_fpu();

	 
	
	asm volatile ("sti\n");

	/* Configuration de la pagination */
	memory_setup((mbi->mem_upper << 10) + (1 << 20));

	pagination_setup();

	/* Initialisation de la vmm */
	init_kmalloc();

	//beep();

	//kprintf("Memoire disponible : %dMio\n", (mbi->mem_upper>>10) + 1); /* Grub balance la mémoire dispo -1 Mio... Soit.*/

	floppy_detect_drives();
	kprintf("Floppy controller version: 0x%x.\n", floppy_get_version());
	
	if(init_floppy() != 0)
		kprintf("Initialisation du lecteur a echoue.\n");
		
	
	/*   Test FAT    */
	//mount_fat_fs ();
	mount_FAT12 ();
	
	pci_scan();

	kprintf("vm86:%d\n",check_vm86());
	
	if(serial_init(COM1, "8N1", 38400, ECHO_ENABLED) != 0)
		kprintf("Erreur d'initialisation de COM1 \n");
		
	/* Initialisation des syscall */
	init_syscall();
	
	syscall_set_handler(SYS_EXIT,	(syscall_handler_t)sys_exit);
	syscall_set_handler(SYS_GETPID,	(syscall_handler_t)sys_getpid);
	syscall_set_handler(SYS_OPEN,	(syscall_handler_t)sys_open);
	syscall_set_handler(SYS_KILL,	(syscall_handler_t)sys_kill);
	syscall_set_handler(SYS_WRITE,	(syscall_handler_t)sys_write);
	syscall_set_handler(SYS_READ,	(syscall_handler_t)sys_read);
	syscall_set_handler(SYS_EXEC, 	(syscall_handler_t)sys_exec);
	syscall_set_handler(SYS_SLEEP, 	(syscall_handler_t)sys_sleep);
	syscall_set_handler(SYS_VIDEO_CTL, (syscall_handler_t)sys_video_ctl);
	syscall_set_handler(SYS_SEMCTL, (syscall_handler_t)sys_ksem);
	syscall_set_handler(SYS_PROC, 	(syscall_handler_t)sys_proc);
	syscall_set_handler(SYS_VMM, 	(syscall_handler_t) sys_vmm);
	syscall_set_handler(SYS_SEEK, 	(syscall_handler_t) sys_seek);
	
	// Création du processus par défaut: notre shell
	create_process("Mishell", (paddr_t)shell,"coucou les petits clous",0x10000,3);
	//create_process("Mishell", (paddr_t)test_proc,0,NULL,1024,3);
	
	events_init();
	
	/* Lancement du scheduler */
	init_scheduler(4);

	/* Initialisation des semaphores */
	init_semaphores();
	
	start_scheduler();

	// Never goes here	
	while(1){}
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
	char *cmd = (char*)cmdLine;
	char opt;

	//printf("Command line : %s\n", cmdLine);

	defaultOptions(options);
	while((opt = get_opt(&cmd)) != -1)
	{
		switch(opt)
		{
		case 'l':
				options->lol = 1;
				break;
		//default: printf("Unknown option %c\n", opt);
		}
	}
}
