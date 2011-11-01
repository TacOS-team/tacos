/**
 * @file ioports.h
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

#ifndef _IOPORTS_H_
#define _IOPORTS_H_

/**
 * @file ioports.h
 *
 * Intel-specific I/O space access routines.
 */

/* This macro allows to write to an I/O port */
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
        "inb %w1,%0"                                            \
        :"=a" (_v)                                              \
        :"Nd" (port)                                            \
        );                                                      \
  _v;                                                           \
})

// write value (word) on port
#define outw(value, port)                                       \
  __asm__ volatile (                                            \
       "outw %w0,%w1"                                          \
        ::"a" (value),"Nd" (port)                               \
        )                                                       \

// read one word from port
#define inw(port)                                               \
({                                                              \
  unsigned int _v;                                              \
  __asm__ volatile (                                            \
        "inw %w1,%w0"                                           \
        :"=a" (_v)                                              \
        :"Nd" (port)                                            \
        );                                                      \
  _v;                                                           \
})

// write value (word) on port
#define outl(value, port)                                       \
  __asm__ volatile (                                            \
        "outl %0,%w1"                                           \
        ::"a" (value),"Nd" (port)                               \
        )                                                       \

// read one word from port
#define inl(port)                                               \
({                                                              \
  unsigned int _v;                                              \
  __asm__ volatile (                                            \
        "inl %w1,%0"                                            \
        :"=a" (_v)                                              \
        :"Nd" (port)                                            \
        );                                                      \
  _v;                                                           \
})


#endif /* _IOPORTS_H_ */
