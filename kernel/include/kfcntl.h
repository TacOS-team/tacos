/**
 * @file kfcntl.h
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

#ifndef _KFCNTL_H
#define _KFCNTL_H

/**
 * @file kfcntl.h
 */

#include <types.h>
#include <libio.h>
#include <kprocess.h>
#include <ksyscall.h>

struct _file_descriptor;

typedef struct _open_file_descriptor {
	uint32_t flags;
	uint8_t buffer[512];
	uint32_t current_octet_buf;
	uint32_t current_cluster;
	uint32_t current_octet;
	uint32_t first_cluster;
	uint32_t file_size;
	void * extra_data;
	size_t (*write)(struct _open_file_descriptor *, const void*, size_t);
	size_t (*read)(struct _open_file_descriptor *,void*, size_t);
	int (*seek)(struct _open_file_descriptor *, long, int);
	int (*ioctl)(struct _open_file_descriptor*, unsigned int, void *);
	int (*open) (struct _open_file_descriptor*);
	int (*close) (struct _open_file_descriptor*);
	int (*flush) (struct _open_file_descriptor*);
} open_file_descriptor;

/** 
 * @brief Initialise les descripteurs de fichiers standards.
 * 
 *	Initialise les descripteurs de fichiers standards.
 *
 * @param fd0 stdin
 * @param fd1 stdout
 * @param fd2 stderr
 */
void init_stdfd(process_t *new_proc);

/**
 *	@brief Ferme tous les file descriptors ouverts.
 */
void close_all_fd();

SYSCALL_HANDLER3(sys_open, uint32_t fd_id, uint32_t p_path , uint32_t flags);
SYSCALL_HANDLER2(sys_close, uint32_t fd_id, uint32_t* ret);

#endif
