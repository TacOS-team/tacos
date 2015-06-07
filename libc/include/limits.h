/**
 * @file libc/include/limits.h
 *
 * @author TacOS developers 
 *
 * @section LICENSE
 *
 * Copyright (C) 2010-2015 TacOS developers.
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
 */

#ifndef _LIMITS_H_
#define _LIMITS_H_

/* Minimum and maximum values a `signed char' can hold.  */
#define SCHAR_MIN	(-128)
#define SCHAR_MAX	127

/* Maximum value an `unsigned char' can hold.  (Minimum is 0.)  */
#define UCHAR_MAX	255

/* Minimum and maximum values a `char' can hold.  */
#define CHAR_MIN	SCHAR_MIN
#define CHAR_MAX	SCHAR_MAX

/* Minimum and maximum values a `signed short int' can hold.  */
#define SHRT_MIN	(-32768)
#define SHRT_MAX	32767

/* Maximum value an `unsigned short int' can hold.  (Minimum is 0.)  */
#define USHRT_MAX	65535

/* Minimum and maximum values a `signed int' can hold.  */
#define INT_MIN	(-INT_MAX - 1)
#define INT_MAX	2147483647

/* Maximum value an `unsigned int' can hold.  (Minimum is 0.)  */
#define UINT_MAX	4294967295U

/* Minimum and maximum values a `signed long int' can hold.  */
#define LONG_MAX	2147483647L
#define LONG_MIN	(-LONG_MAX - 1L)

/* Maximum value an `unsigned long int' can hold.  (Minimum is 0.)  */
#define ULONG_MAX	4294967295UL

#endif
