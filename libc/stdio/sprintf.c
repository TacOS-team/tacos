/**
 * @file sprintf.c
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
 * Description de ce que fait le fichier
 */

#include "stdio.h"

int sprintf(char *str, const char *format, ...) {
	va_list arg;
	int result;
 
	va_start(arg, format);
	result = vsprintf(str, format, arg);
	va_end(arg);
 
	return result;
}

int snprintf(char *str, size_t size, const char *format, ...) {
	va_list arg;
	int result;
 
	va_start(arg, format);
	result = vsnprintf(str, size, format, arg);
	va_end(arg);
 
	return result;
}

int vsprintf(char *str, const char *format, va_list ap) {
	FILE * stream = fmemopen(str, 10000, "r");
	int r = vfprintf(stream, format, ap);
	str[r] = '\0';
	return r;
}

int vsnprintf(char *str, size_t size, const char *format, va_list ap) {
	if (size > 0) {
		str[size - 1] = '\0';
		FILE * stream = fmemopen(str, size, "r");
		return vfprintf(stream, format, ap);
	}
	return 0;
}
