/* Copyright (C) 2004  All GPL'ed OS
   Copyright (C) 1999  Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   as published by the Free Software Foundation; either version 2
   of the License, or (at your option) any later version.
   
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   
   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307,
   USA. 
*/
#ifndef _IOPORTS_H_
#define _IOPORTS_H_

/**
 * @ioports.h
 *
 * Intel-specific I/O space access routines.
 */

// write one byte to a port
#define outb(value, port)                                       \
  __asm__ volatile (                                            \
        "outb %b0,%w1"                                          \
        ::"a" (value),"Nd" (port)                               \
        )                                                       \

// read one byte from port
#define inb(port)                                               \
({                                                              \
  unsigned char _v;                                             \
  __asm__ volatile (                                            \
        "inb %1,%0"                                            \
        :"=a" (_v)                                              \
        :"Nd" (port)                                            \
        );                                                      \
  _v;                                                           \
})

// write one word to a port
#define outw(value, port)                                       \
  __asm__ volatile (                                            \
        "outb %w0,%w1"                                          \
        ::"a" (value),"Nd" (port)                               \
        )                                                       \

// read one word from port
#define inw(port)                                               \
({                                                              \
  unsigned short int _v;                                             \
  __asm__ volatile (                                            \
        "inw %w1,%0"                                            \
        :"=a" (_v)                                              \
        :"Nd" (port)                                            \
        );                                                      \
  _v;                                                           \
})

// write one double-word to a port
#define outl(value, port)                                       \
  __asm__ volatile (                                            \
        "outd %d0,%w1"                                          \
        ::"a" (value),"Nd" (port)                               \
        )                                                       \

// read one double-word from port
#define inl(port)                                               \
({                                                              \
  unsigned long int _v;                                             \
  __asm__ volatile (                                            \
        "ind %d1,%0"                                            \
        :"=a" (_v)                                              \
        :"Nd" (port)                                            \
        );                                                      \
  _v;                                                           \
})

#endif /* _IOPORTS_H_ */
