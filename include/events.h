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

