/**
 * @file sem.c
 *
 * @author TacOS developers 
 *
 *
 * @section LICENSE
 *
 * Copyright (C) 2010, 2011, 2012 - TacOS developers.
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

#include <sem.h>
#include <unistd.h>
#include <sys/syscall.h>

#define MAX_SEM 256
#define KSEM_GET 1
#define KSEM_CREATE 2
#define KSEM_DEL 3
#define KSEM_P 4
#define KSEM_V 5

int semget(uint8_t key)
{
	int ret;
	int data = key;
	syscall(SYS_SEMCTL, KSEM_GET, (uint32_t)data, (uint32_t)&ret);
	return ret;
}

int semcreate(uint8_t key)
{
	int ret;
	int data = key;
	syscall(SYS_SEMCTL, KSEM_CREATE, (uint32_t)data, (uint32_t)&ret);
	return ret;
}

int semdel(uint32_t semid)
{
	int ret;
	syscall(SYS_SEMCTL, KSEM_DEL, semid, (uint32_t)&ret);
	return ret;
}

int semP(uint32_t semid)
{
	int ret;
	syscall(SYS_SEMCTL, KSEM_P, semid, (uint32_t)&ret);
	return ret;
}

int semV(uint32_t semid)
{
	int ret;
	syscall(SYS_SEMCTL, KSEM_V, semid, (uint32_t)&ret);
	return ret;
}

