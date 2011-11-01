/**
 * @file kdriver.c
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

#include <kdriver.h>
#include <string.h>

#define MAX_DRIVERS 64

typedef struct {
	char used;
	char* name;
	driver_interfaces* di;
}driver_entry;

static driver_entry driver_list[MAX_DRIVERS];


/** 
 * @brief Initialise la liste des drivers.
 * 	Initialise la liste des drivers.
 *
 */
void init_driver_list()
{
	int i;
	for(i=0; i<MAX_DRIVERS; i++)
	{
			driver_list[i].used = 0;
			driver_list[i].name = NULL;
			driver_list[i].di = NULL;
	}
}

/** 
 * @brief Trouve les interfaces d'un driver en fonction de son nom.
 * 
 *	Cherche dans la liste des drivers celui qui s'appelle name, et retourne ses interfaces
 *
 * @param name chaine identifiant le driver
 * 
 * @return interfaces du driver trouvé
 */
driver_interfaces* find_driver(char* name)
{
	int i = 0;
	driver_interfaces* ret = NULL;
	
	while(i<MAX_DRIVERS && ret == NULL)
	{
		if(driver_list[i].used)
		{
			if(strcmp(driver_list[i].name, name) == 0)
				ret = driver_list[i].di;
		}
		i++;
	}
	
	return ret;
}

/** 
 * @brief Enregistre le driver dans la liste.
 * 
 *	Enregistre le driver dans la liste.
 *
 * @param name chaine identifiant le driver
 * @param di structure contenant les interfaces du driver
 * 
 * @return -1 en cas d'erreur, 0 sinon
 */
int register_driver(const char* name, driver_interfaces* di)
{
	int i = 0;
	int done = 0;
	while(i<MAX_DRIVERS && !done)
	{
		if(!driver_list[i].used)
		{
			driver_list[i].used = 1;
			
			/* Pas de strdup dans le kernel, try again
			driver_list[i].name = strdup(name);
			*/
			driver_list[i].name = (char*)name;
			driver_list[i].di = di;
			done = 1;
		}
		
		i++;
	}
	return done-1; /* Retourne -1 en cas d'erreur, 0 sinon */
}
