/**
 * @file kernel.c
 *
 * @author TacOS developers 
 *
 *
 * @section LICENSE
 *
 * Copyright (C) 2010-2014 TacOS developers.
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
 * Point d'entrée du kernel. Initialisation du système.
 */

#include <clock.h>
#include <events.h>
#include <fpu.h>
#include <gdt.h>
#include <i8259.h>
#include <idt.h>
#include <init.h>
#include <interrupts.h>
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
#include <round_robin.h>
#include <scheduler.h>
#include <syscall_values.h>
#include <vmm.h>
#include <vfs.h>
#include <mmap.h>

/* Includes des drivers */
#include <drivers/beeper.h>
#include <drivers/console.h>
#include <drivers/floppy.h>
#include <drivers/keyboard.h>
#include <drivers/serial.h>
#include <drivers/video.h>

#include <fs/devfs.h>

/**
 * Options passées au kernel.
 */
typedef struct
{
	uint8_t use_azerty; /**< Utilise un layout azerty */
} kernel_options;

/* Forward declarations. */
void cmain (unsigned long magic, unsigned long addr);
static void initKernelOptions(const char *cmdLine, kernel_options *options);

paddr_t ramdisk_start = 0;
paddr_t ramdisk_end = 0;

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

	/* RAMDISK Initialization */
/*
	if(mbi->mods_count>0) {
		multiboot_module_t* module = (multiboot_module_t*) mbi->mods_addr;
		ramdisk_start = module->mod_start;
		ramdisk_end = module->mod_end;		
	}
*/
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
	interrupt_set_routine(IRQ_COM1, serial_isr, 0);
	interrupt_set_routine(IRQ_COM2, serial_isr, 0);
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
	syscall_set_handler(SYS_SELECT,	(syscall_handler_t)sys_select);
	syscall_set_handler(SYS_EXEC, 	(syscall_handler_t)sys_exec);
	syscall_set_handler(SYS_SLEEP, 	(syscall_handler_t)sys_sleep);
	syscall_set_handler(SYS_SEMCTL, (syscall_handler_t)sys_ksem);
	syscall_set_handler(SYS_PROC, 	(syscall_handler_t)sys_proc);
	syscall_set_handler(SYS_VMM, 	(syscall_handler_t) sys_vmm);
	syscall_set_handler(SYS_SEEK, 	(syscall_handler_t) sys_seek);
	syscall_set_handler(SYS_READLINK, (syscall_handler_t) sys_readlink);
	syscall_set_handler(SYS_SIGNAL,	(syscall_handler_t) sys_signal);
	syscall_set_handler(SYS_SIGPROCMASK, 	(syscall_handler_t) sys_sigprocmask);
	syscall_set_handler(SYS_KILL,	(syscall_handler_t) sys_kill);
	syscall_set_handler(SYS_CLOSE,	(syscall_handler_t) sys_close);
	syscall_set_handler(SYS_READDIR,	(syscall_handler_t) sys_readdir);
	syscall_set_handler(SYS_MKDIR,	(syscall_handler_t) sys_mkdir);
	syscall_set_handler(SYS_RMDIR,	(syscall_handler_t) sys_rmdir);
	syscall_set_handler(SYS_SIGRET,	(syscall_handler_t) sys_sigret);
	syscall_set_handler(SYS_SIGSUSPEND,	(syscall_handler_t) sys_sigsuspend);
	syscall_set_handler(SYS_GETCLOCK,	(syscall_handler_t) sys_getclock);
	syscall_set_handler(SYS_GETDATE,	(syscall_handler_t) sys_getdate);
	syscall_set_handler(SYS_FCNTL,	(syscall_handler_t) sys_fcntl);
	syscall_set_handler(SYS_STAT,	(syscall_handler_t) sys_stat);
	syscall_set_handler(SYS_UNLINK,	(syscall_handler_t) sys_unlink);
	syscall_set_handler(SYS_DUP,	(syscall_handler_t) sys_dup);
	syscall_set_handler(SYS_DUP2,	(syscall_handler_t) sys_dup2);
	syscall_set_handler(SYS_MKNOD,	(syscall_handler_t) sys_mknod);
	syscall_set_handler(SYS_WAITPID, (syscall_handler_t) sys_waitpid);
	syscall_set_handler(SYS_CHMOD, (syscall_handler_t) sys_chmod);
	syscall_set_handler(SYS_CHOWN, (syscall_handler_t) sys_chown);
	syscall_set_handler(SYS_UTIMES, (syscall_handler_t) sys_utimes);
	syscall_set_handler(SYS_RENAME, (syscall_handler_t) sys_rename);
	syscall_set_handler(SYS_SYMLINK, (syscall_handler_t) sys_symlink);
	syscall_set_handler(SYS_CHDIR, (syscall_handler_t) sys_chdir);
	syscall_set_handler(SYS_MMAP, (syscall_handler_t) sys_mmap);

	if(ramdisk_start != 0) {
		/*init_ramdisk(ramdisk_start, ramdisk_end);*/
	}
	
	/* Lancement du scheduler */
	init_scheduler(5);
	set_scheduler(&round_robin);
	klog("Scheduler initialized.");
	
	// Création du processus par défaut.
	create_kprocess("init", (void*) init, 0x100000);
	events_init();
	
	start_scheduler();

	// Never goes here	
	while(1){}
}


static void defaultOptions(kernel_options *options)
{
	options->use_azerty = 1;
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
				options->use_azerty = 0;
				break;
		//default: printf("Unknown option %c\n", opt);
		}
	}
	selectLayout(options->use_azerty);
}
