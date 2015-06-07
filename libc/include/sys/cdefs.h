/**
 * @file cdefs.h
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

#ifndef _SYS_CDEFS_H_
#define _SYS_CDEFS_H_

/* C++ needs to know that types and declarations are C, not C++. */
#ifdef  __cplusplus
# define __BEGIN_DECLS  extern "C" {
# define __END_DECLS  }
#else
# define __BEGIN_DECLS
# define __END_DECLS
#endif

#endif
