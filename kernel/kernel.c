/**
 * @file kernel.c
 *
 * @author TacOS developers 
 *
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

#include "msr.h"
#include <clock.h>
#include <debug.h>
#include <events.h>
#include <exception.h>
#include <fpu.h>
#include <gdt.h>
#include <i8259.h>
#include <idt.h>
#include <init.h>
#include <interrupts.h>
#include <ioports.h>
#include <kdirent.h>
#include <kfcntl.h>
#include <klog.h>
#include <kmalloc.h>
#include <kpanic.h>
#include <kprocess.h>
#include <ksem.h>
#include <ksignal.h>
#include <ksyscall.h>
#include <kunistd.h>
#include <memory.h>
#include <multiboot.h>
#include <pagination.h>
#include <pci.h>
#include <pci_config.h>
#include <round_robin.h>
#include <rtl8139.h>
#include <scheduler.h>
#include <string.h>
#include <sys/syscall.h>
#include <time.h>
#include <vga.h>
#include <video.h>
#include <vm86.h>
#include <vmm.h>

/* Includes des fs */
#include <fs/devfs.h>
#include <fs/fat.h>
#include <fs/procfs.h>

/* Includes des drivers */
#include <drivers/console.h>
#include <drivers/dummy_driver.h>
#include <drivers/floppy.h>
#include <drivers/keyboard.h>
#include <drivers/mouse.h>
#include <drivers/serial.h>
#include <drivers/beeper.h>

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
		kerr("Invalid magic number: 0x%x", (unsigned) magic);
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
	
	if(mbi->flags & MULTIBOOT_INFO_ELF_SHDR) 
	{
		
	}
	else
		kerr("ELF section headers unavailable");
  
	interrupt_set_routine(IRQ_KEYBOARD, keyboardInterrupt, 0);
	interrupt_set_routine(IRQ_LPT1, LPT1_routine, 0);
	interrupt_set_routine(IRQ_COM1, serial_isr, 0);
	init_fpu();

	asm volatile ("sti\n");

	/* Configuration de la pagination */
	memory_setup((mbi->mem_upper << 10) + (1 << 20));

	pagination_setup();

	/* Initialisation de la vmm */
	init_kmalloc();


	/* Initialisation des semaphores */
	init_semaphores();
	
	/* Initialisation des syscall */
	//klog("syscall initialization...");
	init_syscall();
	
	syscall_set_handler(SYS_DUMMY,	(syscall_handler_t) sys_dummy);
	syscall_set_handler(SYS_EXIT,	(syscall_handler_t)sys_exit);
	syscall_set_handler(SYS_GETPID,	(syscall_handler_t)sys_getpid);
	syscall_set_handler(SYS_GETPPID,	(syscall_handler_t)sys_getppid);
	syscall_set_handler(SYS_OPEN,	(syscall_handler_t)sys_open);
	syscall_set_handler(SYS_WRITE,	(syscall_handler_t)sys_write);
	syscall_set_handler(SYS_READ,	(syscall_handler_t)sys_read);
	syscall_set_handler(SYS_IOCTL,	(syscall_handler_t)sys_ioctl);
	syscall_set_handler(SYS_EXEC, 	(syscall_handler_t)sys_exec);
	syscall_set_handler(SYS_SLEEP, 	(syscall_handler_t)sys_sleep);
	syscall_set_handler(SYS_SEMCTL, (syscall_handler_t)sys_ksem);
	syscall_set_handler(SYS_PROC, 	(syscall_handler_t)sys_proc);
	syscall_set_handler(SYS_VMM, 	(syscall_handler_t) sys_vmm);
	syscall_set_handler(SYS_SEEK, 	(syscall_handler_t) sys_seek);
	syscall_set_handler(SYS_SIGNAL,	(syscall_handler_t) sys_signal);
	syscall_set_handler(SYS_SIGPROCMASK, 	(syscall_handler_t) sys_sigprocmask);
	syscall_set_handler(SYS_KILL,	(syscall_handler_t) sys_kill);
	syscall_set_handler(SYS_CLOSE,	(syscall_handler_t) sys_close);
	syscall_set_handler(SYS_OPENDIR,	(syscall_handler_t) sys_opendir);
	syscall_set_handler(SYS_READDIR,	(syscall_handler_t) sys_readdir);
	syscall_set_handler(SYS_MKDIR,	(syscall_handler_t) sys_mkdir);
	syscall_set_handler(SYS_SIGRET,	(syscall_handler_t) sys_sigret);
	syscall_set_handler(SYS_SIGSUSPEND,	(syscall_handler_t) sys_sigsuspend);
	syscall_set_handler(SYS_GETCLOCK,	(syscall_handler_t) sys_getclock);
	syscall_set_handler(SYS_GETDATE,	(syscall_handler_t) sys_getdate);
	syscall_set_handler(SYS_VGASETMODE,	(syscall_handler_t) sys_vgasetmode);
	syscall_set_handler(SYS_VGAWRITEBUF,	(syscall_handler_t) sys_vgawritebuf);
	syscall_set_handler(SYS_FCNTL,	(syscall_handler_t) sys_fcntl);
	syscall_set_handler(SYS_STAT,	(syscall_handler_t) sys_stat);
	syscall_set_handler(SYS_UNLINK,	(syscall_handler_t) sys_unlink);
	

	devfs_init();
	vfs_mount(NULL, "dev", "DevFS");

	console_init();
	serial_init();
	beeper_init();

	pci_scan();
		
	int irq = rtl8139_driver_init();
	interrupt_set_routine(irq,  rtl8139_isr, 0);
	
	floppy_detect_drives();
	kdebug("Floppy controller version: 0x%x.", floppy_get_version());
	
	if(init_floppy() != 0)
		kerr("Initialisation du lecteur a echoue.");
	
	fat_init();
	procfs_init();

	vfs_mount(NULL, "proc", "ProcFS");
	
	vfs_mount("/dev/fd0", "tacos", "FAT");
	
	init_process_array();
	
	/* Lancement du scheduler */
	klog("Init scheduler...");
	init_scheduler(20);
	set_scheduler(&round_robin);
	
	// Création du processus par défaut.
	
	process_init_data_t init_process;
	
	init_process.name = "init";
	init_process.args = "init";
	init_process.exec_type = EXEC_KERNEL;
	init_process.data = (void*)init;
	init_process.mem_size = 0;
	init_process.entry_point = 0;
	init_process.stack_size = 0x1000;
	init_process.priority = 0;
	init_process.ppid = 0;
	
	scheduler_add_process(create_process(&init_process));
	
	events_init();
	
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
