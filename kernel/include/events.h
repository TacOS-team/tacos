/**
 * @file events.h
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

#ifndef _EVENTS_H_
#define _EVENTS_H_

/**
* @file events.h
*/

#include <time.h>

typedef void *(*callback_t) (void *);

/** 
* @brief structure stockant un évènement à declencher
*/
struct event_t
{
  int id;
  struct timeval date;
  callback_t callback;
  void *data;
};

/** 
* @brief initialise le support des evenements
*/
void events_init();

/**
 * @brief met à jour l'évènement de l'ordonnanceur
 *
 * @param call la fonction qui sera lancée
 * @param data un pointeur qui sera passé à la fonction
 * @param time la fonction sera lancée dans "time" usec
 */
void set_scheduler_event(callback_t call, void *data, time_t dtime_usec);

/**
 * Désactive l'évènement de l'ordonnanceur
 */
void unset_scheduler_event();

/** 
 * @brief ajoute un évènement à declencher
 * 
 * @param call la fonction qui sera lancée
 * @param data un pointeur qui sera passé à la fonction
 * @param time la fonction sera lancée dans "time" usec
 *
 * @return l'id du nouvel évènement
 */
int add_event(callback_t call, void* data, clock_t time);

/**
 * Supprime un évènement à partir de son id
 *
 * @param id de l'évenement
 */
int del_event(int id);

#endif

