/**
 * @file kfcntl.c
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

#include <kprocess.h>
#include <ksyscall.h>
#include <scheduler.h>
#include <stdio.h>
#include <kfat.h>
#include <string.h>
#include <kmalloc.h>

#include <keyboard.h>
#include <console.h>
#include <tty.h>

void init_stdfd(process_t *new_proc) {
    struct _file_descriptor *fd0 = &(new_proc->fd[0]);
    struct _file_descriptor *fd1 = &(new_proc->fd[1]);
    struct _file_descriptor *fd2 = &(new_proc->fd[2]);
    terminal_t *t;

    if (TRUE || new_proc->ppid == 0) { // pas de père, on peut pas récupérer son tty.
        t = kmalloc(sizeof(terminal_t));
        int console = get_available_console(t);
        focus_console(console);
        tty_init(t, new_proc, (void *)console,(void (*)(void *, char)) kputchar);
        //new_proc->ctrl_tty = t;
    } else {
        /*
        process_t *pprocess = get_process(new_proc->ppid);
        terminal_t *t = pprocess->ctrl_tty;
        //focus_console
        */
    }
	fd0->used = TRUE; /* stdin */
	fd0->ofd = kmalloc(sizeof(open_file_descriptor));
	fd0->ofd->read = tty_read;
	fd0->ofd->extra_data = t;
	fd1->used = TRUE; /* stdout */
	fd1->ofd = kmalloc(sizeof(open_file_descriptor));
	fd1->ofd->write = tty_write;
	fd1->ofd->extra_data = t;
	fd2->used = TRUE; /* stderr */
	fd2->ofd = kmalloc(sizeof(open_file_descriptor));
	fd2->ofd->write = tty_write;
	fd2->ofd->extra_data = t;
}


SYSCALL_HANDLER3(sys_open, uint32_t fd_id, uint32_t p_path , uint32_t flags) {
	int i=0;
	
	//process_t * process = (process_t*) p_process;
	char* path = (char*) p_path;
	process_t* process = get_current_process();
	
	// recherche d une place dans la file descriptor table du process
	while(process->fd[i].used==TRUE) 
		i++;
	
	// creation d un open_file_descriptor
	process->fd[i].ofd = kmalloc(sizeof(open_file_descriptor));
	process->fd[i].used = TRUE;
	
	// ouverture du fichier (sur fd0 pour le moment)
	if (fat_open_file(path, process->fd[i].ofd, flags) == 0) {
		// C'est ici qu'on devrait vérifier si le fichier ouvert est spécial 
		// pour savoir si on doit binder au file system ou à un driver.
		process->fd[i].ofd->write = write_file;
		process->fd[i].ofd->read = read_file;
		process->fd[i].ofd->seek = seek_file;
		*((int *)fd_id) = i;
	} else {
		*((int *)fd_id) = -1;
	}
}

