/**
 * @file stdint.h
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

#ifndef _STDINT_H_
#define _STDINT_H_

#include <sys/cdefs.h>

__BEGIN_DECLS

#ifndef _TYPES_INTX_T_
#define _TYPES_INTX_T_
typedef signed char int8_t;  /**< 8 bits signé */
typedef short int int16_t;  /**< 16 bits signé */
typedef long int int32_t;  /**< 32 bits signé */
typedef long long int int64_t;  /**< 64 bits signé */
#endif

#ifndef _TYPES_UINTX_T_
#define _TYPES_UINTX_T_
typedef unsigned char uint8_t;  /**< 8 bits non signé */
typedef unsigned short int uint16_t;  /**< 16 bits non signé */
typedef unsigned long int uint32_t;  /**< 32 bits non signé */
typedef unsigned long long int uint64_t;  /**< 64 bits non signé */
#endif

__END_DECLS

#endif
