/**
 * @file klog.h
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
 * Logging macros
 */

#ifndef _KLOG_H
#define _KLOG_H

//#define DEBUG

#include <kstdio.h>

extern void klog_systime();

#define klog(message, ...) do{klog_systime();kprintf("[%s] "message"\n", __FILE__, ##__VA_ARGS__);}while(0)

#define kerr(message, ...) do{kprintf("\033[031m");klog_systime();kprintf("\033[031m[%s:%d in %s] ERROR: "message"\033[0m\n", __FILE__, __LINE__, __func__, ##__VA_ARGS__);}while(0)

#ifdef DEBUG
	#define kdebug(message, ...) do{klog_systime();kprintf("[%s] DEBUG: "message"\n", __FILE__, ##__VA_ARGS__);}while(0)
#else
	#define kdebug(message, ...)
#endif

#endif
