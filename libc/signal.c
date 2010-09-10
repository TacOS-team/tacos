/**
 * @file signal.c
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

#include <signal.h>
#include <syscall.h>

int kill(unsigned int pid, int sig)
{
	int ret;
	
	syscall(SYS_KILL, (uint32_t) pid, (uint32_t) sig, (uint32_t) &ret);
	
	return ret;
}


sighandler_t signal(int sig, sighandler_t func)
{
	sighandler_t ret;
	
	syscall(SYS_SIGNAL, (uint32_t)sig, (uint32_t)func, (uint32_t)&ret);
	
	return ret;
}

int sigprocmask(int how, const sigset_t *set, sigset_t *oldset)
{
	int ret = -1;
	if(set != NULL)
	{
		ret = 0;
		syscall(SYS_SIGPROCMASK, (uint32_t)how, (uint32_t)set, (uint32_t)oldset);
	}
	return ret;
}


int sigemptyset(sigset_t *set)
{
	int ret = -1;
	if(set != NULL)
	{
		ret = 0;
		*set = 0;
	}
	return ret;
}

int sigfillset(sigset_t *set)
{
	int ret = -1;
	if(set != NULL)
	{
		ret = 0;
		*set = 0xFFFFFFFF;
	}
	return ret;
}

int sigaddset(sigset_t *set, int signum)
{
	int ret = -1;
	if(set != NULL)
	{
		ret = 0;
		*set |= (1<<signum);
	}
	return ret;
}

int sigdelset(sigset_t *set, int signum)
{
	int ret = -1;
	if(set != NULL)
	{
		ret = 0;
		*set &= ~(1<<signum);
	}
	return ret;
}

int sigismember(const sigset_t *set, int signum)
{
	int ret = -1;
	if(set != NULL)
		ret = 0x1 & (*set>>signum);
	
	return ret;
}
	

