/**
 * @file spinlock.h
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
 * Description de ce que fait le fichier
 */

#ifndef _SPINLOCK_H
#define _SPINLOCK_H

/* XXX Attention, cette implementation du spinlock est MAUVAISE:
 * while(lock);
 * lock=1;
 * n'est pas atomique, le process peut donc etre preempte apres 
 * le while, mais avant l'assignation, et il est donc possible 
 * qu'un autre process passe le while en meme temps. */

#define CREATE_SPINLOCK(LOCK) static unsigned char LOCK = 0
#define SPINLOCK(LOCK) while(LOCK);LOCK=1
#define SPINUNLOCK(LOCK) LOCK=0

#endif /* _SPINLOCK_H */
